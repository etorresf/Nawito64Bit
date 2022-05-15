/*
 * < Nawito is a chess engine UCI derived of Danasah507>
 * Copyright (C) <2022> <Ernesto Torres>
 *
 * E-mail:		<ernesto2@nauta.cu>
 * Source:		https://github.com/etorresf/Nawito64Bit
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "eval.h"


//====================================================================================================================================
int			BishopTropism[64][64],

			Weight[16],
			*MidGame			= Weight + 0,
			*EndGame			= Weight + 2,
			*MidEndGame			= Weight + 4,
			*AttackWeight		= Weight + 6,
			*AttackCount		= Weight + 8,
			*KingSafety			= Weight + 10,
			*RookTrapped		= Weight + 12;

BITBOARD	PawnAttBB[2],
			PieceAttBB[2],
			KingAttBB[2],
			KING_ZONE[2];


//====================================================================================================================================
#define		SQ_DEFENDED(sq, side, cside)	(PopCountBB(PAWN_ATTACK_BB(sq, side)) >= PopCountBB(PAWN_ATTACK_BB(sq, cside) | (PAWN_BB(cside) & SQ_BB(sq))))
#define		IN_FRONT_BB(sq, side, bb)		(ForwardBB[side][sq] & (bb))
#define		IS_OUTPOST(side, sq)			!(OutpostBB[side][sq] & PAWN_BB((side) ^ 1))
#define		PAWNS_IN_FRONT_BB(sq, side)		(PawnPassedBB[(side) ^ 1][sq] & PAWN_BB(side))
#define		COUNT_PAWNS_IN_FRONT(sq, side)	PopCountBB(PAWNS_IN_FRONT_BB(sq, side))
#define		COUNT_PAWN_ATTACK(sq, side)		PopCountBB(PAWN_ATTACK_BB(sq, side))
#define		MOBILITY_TARGET(cside)			(~ALL_PIECES_BB | (~PawnAttBB[cside] & PieceBB[cside]))
#define		IN_7th(sq_rel, side)			(RANK(sq_rel) == RANK_2 && (In7thBB[side] & (KING_BB((side) ^ 1) | PAWN_BB((side) ^ 1))))


//====================================================================================================================================
// Evalua la prescencia o falta de fiancheto en el enroque
int		FianchettoEval(PIECE piece)				{
	switch (piece)				{
		case PAWN: case KING:	return 0;
		case BISHOP:			return KING_FIANCHETTO_MID;
		default:				return -KING_FIANCHETTO_MID;
	}
}


// Evaluacion de otros paramatros del escudo del Rey.
int		ExtraShield(int file, TURN side)		{
	int		score	= 0;


	// Enroque corto
	if (file > FILE_F)			{
		score	+= (RookTrapped[side] & 2) ? 0 : KING_CASTTLE_DONE_MID
				+  FianchettoEval(Board[REL_SQ(G7, side)] ^ side);
	}

	// Enroque largo
	else if (file < FILE_D)		{
		score	+= (RookTrapped[side] & 1) ? 0 : KING_CASTTLE_DONE_MID
				+  FianchettoEval(Board[REL_SQ(B7, side)] ^ side);
	}

	// Rey en el centro
	else if (QUEENS(FLIP_TURN(side)) && !((Casttle >> side) & (BQ | BK)))
		score	-= KING_IN_CENTER_MID;


	return score;
}


// Evalua a uno de los Peones del escudo del Rey
int		KingPawnShield(SQUARE sq, TURN side)	{
	int		score	= 0;
	TURN	cside	= FLIP_TURN(side);
	sq				= REL_SQ(sq, side);


	if (!MATCH(PAWN | side, sq)){
		// Penalizacion, peon movido del escudo del Rey
		if (OPEN_FILE(sq, side))
			score	+= KING_NO_PAWN_IN_FILE;					// No hay peon en la columna

		else if (MATCH(PAWN | side, sq + Front[side]))
			score	+= KING_PAWN_IN_3;							// Peon movido 1 casilla

		else score	+= KING_PAWN_IN_4;							// Peon movido 2 o mas casillas
	}


	// Penalizacion, avalancha hacia el Rey de peones enemigos
	if (OPEN_FILE(sq, cside))
		score		+= KING_NO_PAWN_ENEMY_IN_FILE;				// No hay peones enemigos en la columna

	else if (MATCH(PAWN | cside, sq + Front[side]))
		score		+= KING_PAWN_ENEMY_IN_3;					// Peon enemigo en 3ra fila

	else if (MATCH(PAWN | cside, sq + 2 * Front[side]))
		score		+= KING_PAWN_ENEMY_IN_4;					// Peon enemigo en 4ta fila

	return score;
}


// Evalua a uno de los Peones del escudo del Rey
int		KingShield(TURN side)	{
	BITBOARD	bb;
	int			sq_file		= FILE(KING_SQ(side));


	switch (sq_file)			{
		// Enroque corto
		case FILE_G: case FILE_H:
			return KingPawnShield(H7, side) + KingPawnShield(G7, side) + KingPawnShield(F7, side) / 2;

		// Enroque largo
		case FILE_A: case FILE_B: case FILE_C:
			return KingPawnShield(A7, side) + KingPawnShield(B7, side) + KingPawnShield(C7, side) / 2;

		// Rey en el centro
		default:
			bb	= BLACK_PAWN_BB | WHITE_PAWN_BB;
			return KING_IN_OPEN_FILE * (!(FileBB[sq_file] & bb) + !(FileBB[sq_file - 1] & bb) + !(FileBB[sq_file + 1] & bb));
	}
}


// Peon retrasado. Solo tiene en cuenta a los peones y sus ataques para detectarlo
int		IsPawnBackward(SQUARE sq)				{
	SQUARE		to;
	TURN		side	= GET_COLOR(Board[sq]),
				cside	= FLIP_TURN(side);


	// Si no existe peon vecino mas atrasado y existen peones enemigos al frente
	if (IS_OUTPOST(cside, sq) && IN_FRONT_BB(sq, side, PawnAttBB[side] | PAWN_BB(cside)))
		for (to	= sq + Front[side];; to += Front[side])			{
			// Es retrasado, Peon enemigo lo bloquea
			if (MATCH(PAWN | cside, to))
				return 1;

			// Es retrasado, mayoria de Peones enemigos atacando, de lo contrario no lo es
			if (PAWN_ATTACK_BB(to, side))
				return COUNT_PAWN_ATTACK(to, cside) > COUNT_PAWN_ATTACK(to, side);

			// Es retrasado, mayoria de Peones enemigos atacando
			if (PAWN_ATTACK_BB(to, cside))
				return 1;

			// Es retrasado, Peon amigo lo bloquea
			if (MATCH(PAWN | side, to))
				return 1;
		}

	return 0;
}


// Peon semi-pasado. Solo tiene en cuenta a los peones y sus ataques para detectarlo
int		IsPawnCandidate(SQUARE sq, TURN side)	{
	TURN	cside		= FLIP_TURN(side);


	// Si no hay mayoria de peones enemigos al frente no es considerado como candidato
	if (COUNT_PAWNS_IN_FRONT(sq + Front[side], cside) >= COUNT_PAWNS_IN_FRONT(sq + Front[side], side))			{
		// Comprobando si puede ser defendido por la izquierda
		if (FILE(sq) != FILE_A && SQ_DEFENDED(sq - 1, side, cside))
			return 1;

		// Comprobando si puede ser defendido por la derecha
		if (FILE(sq) != FILE_H && SQ_DEFENDED(sq + 1, side, cside))
			return 1;
	}

	return 0;
}


// Actualiza ataques al Rey enemigo y la tabla de ataques
void	PutAttack(TURN side, BITBOARD bb, int weight, int count){
	// Actualizando tabla de ataques
	PieceAttBB[side]	|= bb;


	// Actualizando ataque al Rey enemigo
	if ((bb &= KING_ZONE[FLIP_TURN(side)]))		{
		AttackWeight[side]	+= weight * PopCountBB(bb);
		AttackCount[side]	+= count;							// K = 0, Q = 16, R = 12, B = 8, N = 8, P = 4
	}
}


// Evalua la estructura de Peones
void	PawnEval()				{
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, tmp_bb;
	int			view, center_file_dist, last_rank_dist,
				doubled, isolated, opened, backward, passed, candidate,
				mid_game			= 0,
				end_game			= 0,
				black_casttle		= 0,
				white_casttle		= 0,
				mid_game_pawn[2]	= {0, 0},
				end_game_pawn[2]	= {0, 0};


	// Evaluando caracteristicas de peones, si no estan en la hash de los peones
	if (ProbePawnScore(&mid_game, &end_game, &black_casttle, &white_casttle, PawnHash) == -MATE){
		TURN_LOOP(side)			{
			view	= side ? 56 : 0;
			cside	= FLIP_TURN(side);


			for (bb = PAWN_BB(side); bb; bb ^= SQ_BB(sq))		{
				sq				= LSB_BB(bb);
				sq_rel			= sq ^ view;
				center_file_dist= DistanceCenterFile[sq];
				last_rank_dist	= DISTANCE_LAST_RANK(sq_rel);

				doubled			= 4 * ((ForwardBB[side][sq] & PAWN_BB(side)) != 0);
				isolated		= !(FileBB[sq] & PawnAttBB[side]);
				opened			= !IN_FRONT_BB(sq, side, PAWN_BB(cside));
				passed			= !PAWNS_IN_FRONT_BB(sq, cside);
				backward		= !(passed || isolated) && IsPawnBackward(sq);
				candidate		= !(backward || isolated || passed) && opened && IsPawnCandidate(sq, side);


				// Material y PST
				mid_game_pawn[side]	-= PAWN_DIST_CENTER_FILE_MID * center_file_dist + PAWN_DIST_LAST_RANK_MID * last_rank_dist;
				end_game_pawn[side]	-= PAWN_DIST_CENTER_FILE_END * center_file_dist + PAWN_DIST_LAST_RANK_END * last_rank_dist;

				// Penalizando a peones centrales en sus casilla inicial
				if (sq_rel == D7 || sq_rel == E7)
					mid_game_pawn[side]	-= PAWN_CENTRAL_DEFAULT_POS_MID;

				// Penalizando a peones doblados, incluye menor cantidad de peones enemigos deteniendolo
				if (doubled)	{
					tmp_bb			= PawnPassedBB[side][sq];
					if (isolated && (PopCountBB(tmp_bb & PAWN_BB(cside)) <= PopCountBB(tmp_bb & PAWN_BB(side))))
						doubled		+= (tmp_bb & PAWN_BB(cside)) ? 2 : 1;

					mid_game_pawn[side]	-= ((PAWN_DOUBLED_MID - PAWN_DOUBLED_DIST_CENTER_FILE_MID * center_file_dist) * doubled) / 4;
					end_game_pawn[side]	-= ((PAWN_DOUBLED_END - PAWN_DOUBLED_DIST_CENTER_FILE_END * center_file_dist) * doubled) / 4;
				}

				// Penalizando a peones doblados
				if (isolated)	{
					mid_game_pawn[side]	-= (PAWN_ISOLATED_MID - PAWN_ISOLATED_DIST_CENTER_FILE_MID * center_file_dist) * (opened + 1);
					end_game_pawn[side]	-=  PAWN_ISOLATED_END - PAWN_ISOLATED_DIST_CENTER_FILE_END * center_file_dist;
				}

				// Penalizando a peones retrasados
				if (backward)	{
					mid_game_pawn[side]	-= (PAWN_BACKWARD_MID - PAWN_BACKWARD_DIST_CENTER_FILE_MID * center_file_dist) * (opened + 1);
					end_game_pawn[side]	-=  PAWN_BACKWARD_END - PAWN_BACKWARD_DIST_CENTER_FILE_END * center_file_dist;
				}

				// Bonificando peones semi-pasados
				if (candidate)	{
					mid_game_pawn[side]	+= PAWN_CANDIDATE_MID - PAWN_CANDIDATE_DIST_LAST_RANK_MID * last_rank_dist;
					end_game_pawn[side]	+= PAWN_CANDIDATE_END - PAWN_CANDIDATE_DIST_LAST_RANK_END * last_rank_dist;
				}

				// Bonificando peones semi-pasados
				if (passed)		{
					mid_game_pawn[side]	+= PAWN_PASSED_MID - PAWN_PASSED_DIST_LAST_RANK_MID * last_rank_dist;
					end_game_pawn[side]	+= PAWN_PASSED_END - PAWN_PASSED_DIST_LAST_RANK_END * last_rank_dist;

					// Los peones pasados protegidos son mas peligrosos
					if (PAWN_ATTACK_BB(sq + Front[side], side))	{
						mid_game_pawn[side]	+= PAWN_PASSED_PROTECTED;
						end_game_pawn[side]	+= PAWN_PASSED_PROTECTED;
					}

					else if (PAWN_ATTACK_BB(sq, side))			{
						mid_game_pawn[side]	+= PAWN_PASSED_PROTECTED / 2;
						end_game_pawn[side]	+= PAWN_PASSED_PROTECTED / 2;
					}

					// Tropismo de Reyes a PEON pasado y soporte por torres
					end_game_pawn[side]	+= (PAWN_PASSED_ENEMY_KING_TROPISM_END	* DistanceLineal[sq + Front[side]][KING_SQ(cside)]
										 -  PAWN_PASSED_MY_KING_TROPISM_END		* DistanceLineal[sq + Front[side]][KING_SQ(side)]) / 4;
				}
			}
		}


		mid_game		= mid_game_pawn[WHITE] - mid_game_pawn[BLACK];
		end_game		= end_game_pawn[WHITE] - end_game_pawn[BLACK];

		black_casttle	= KingShield(BLACK);
		white_casttle	= KingShield(WHITE);

		// Guardando en hash de peones la evaluacion
		StorePawnScore(mid_game, end_game, black_casttle, white_casttle, PawnHash);
	}


	// Evaluando caracteristicas de peones que no pueden almacenarse en hash
	TURN_LOOP(side)				{
		view	= side ? 56 : 0;

		// Peones bloqueados en C2, D2, E2
		if (MATCH(PAWN | side, C7 ^ view) && MATCH(KNIGHT | side, C6 ^ view) && MATCH(PAWN | side, D5 ^ view) && !MATCH(PAWN | side, E5 ^ view))
			MidGame[side]	-= PAWN_C2_BLOCKED_MID;

		if (MATCH(PAWN | side, E7 ^ view) && MATCH(BISHOP | side, F8 ^ view) && NO_EMPTY(E6 ^ view))
			MidGame[side]	-= PAWN_CENTRAL_BLOCKED_MID;

		if (MATCH(PAWN | side, D7 ^ view) && MATCH(BISHOP | side, C8 ^ view) && NO_EMPTY(D6 ^ view))
			MidGame[side]	-= PAWN_CENTRAL_BLOCKED_MID;
	}


	// Asignando valores de las tablas hash
	MidGame[WHITE]		+= mid_game;
	EndGame[WHITE]		+= end_game;

	KingSafety[BLACK]	+= black_casttle;
	KingSafety[WHITE]	+= white_casttle;
}


// Evalúa los Caballos
void	KnightEval()			{
	int			center_dist, last_rank_dist, view,
				pawn_attack, mob;
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, mob_bb, bb2;


	TURN_LOOP(side)
		if ((bb = KNIGHT_BB(side)))				{
			view	= side ? 56 : 0;
			cside	= FLIP_TURN(side);
			bb2		= MOBILITY_TARGET(cside);


			for (; bb; bb ^= SQ_BB(sq))			{
				sq				= LSB_BB(bb);
				sq_rel			= sq ^ view;
				last_rank_dist	= DISTANCE_LAST_RANK(sq_rel);
				center_dist		= DistanceCenter[sq];
				pawn_attack		= PopCountBB(PAWN_ATTACK_BB(sq, side));
				mob_bb			= KNIGHT_MOVES_BB(sq);
				mob				= CENTER_WEIGHT(mob_bb & bb2, 0);


				// Agregando ataques de Caballo a la tabla de ataques
				PutAttack(side, mob_bb, KNIGHT_ATTACK_KING, 8);

				// Material y PST
				MidGame[side]	+= (KNIGHT_MOB_MID * mob) / 4 - KNIGHT_DIST_CENTER_MID * center_dist - KNIGHT_DIST_LAST_RANK_MID * last_rank_dist;
				EndGame[side]	+= (KNIGHT_MOB_END * mob) / 4 - KNIGHT_DIST_CENTER_END * center_dist;

				// Bonus, acercandose al Rey enemigo
				KingSafety[cside]		+= KNIGHT_KING_TROPISM * Distance[sq][KING_SQ(cside)];

				// Desarrollo lento
				if (sq_rel == B8 || sq_rel == G8)
					MidGame[side]		-= KNIGHT_SLOW_DEVELOPMENT_MID;

				// Defendiendo a caballos con peones
				if (pawn_attack){
					// Apostado, no es posible atacarlo con peones, está defendido por peones y en casillas centrales
					if ((KNIGHT_OUTPOST_BB & SQ_BB(sq_rel)) && IS_OUTPOST(side, sq))
						MidGame[side]	+= (KNIGHT_OUTPOST_MID * pawn_attack * KnightOutpost[sq_rel]) / 4;

					else MidGame[side]	+= KNIGHT_PROTECTED_BY_PAWN_MID;
				}

				// Caballo atrapado
				if (mob < 5 && (SQ_BB(sq_rel) & KNIGHT_TRAPPED_A1_A2_BB))
					MidEndGame[side]	-= KNIGHT_TRAPPED * ((SQ_BB(sq_rel) & KNIGHT_TRAPPED_A1_BB) ? 3 : 2);
			}
		}
}


// Evalúa los Alfiles
void	BishopEval()			{
	int			center_dist, last_rank_dist, view,
				mob, two_bishop, bad_bishop;
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, mob_bb, bb2;


	TURN_LOOP(side)
		if ((bb = BISHOP_BB(side)))				{
			cside			= FLIP_TURN(side);
			view			= side ? 56 : 0;
			two_bishop		= (BISHOP_BB(side) & WHITE_SQUARES_BB) && (BISHOP_BB(side) & BLACK_SQUARES_BB);
			bb2				= MOBILITY_TARGET(cside);


			for (; bb; bb ^= SQ_BB(sq))			{
				sq				= LSB_BB(bb);
				sq_rel			= sq ^ view;
				last_rank_dist	= DISTANCE_LAST_RANK(sq_rel);
				center_dist		= DistanceCenter[sq];
				mob_bb			= BISHOP_MOVES_BB(sq, ALL_PIECES_BB);
				mob				= CENTER_WEIGHT(mob_bb & bb2, 1);


				// Agregando ataques de Alfiles a la tabla de ataques
				PutAttack(side, mob_bb, BISHOP_ATTACK_KING, 8);

				// Material y PST
				MidGame[side]	+= (BISHOP_MOB_MID * mob) / 4 - BISHOP_DIST_CENTER_MID * center_dist - BISHOP_DIST_LAST_RANK_MID * last_rank_dist;
				EndGame[side]	+= (BISHOP_MOB_END * mob) / 4 - BISHOP_DIST_CENTER_END * center_dist;

				// Bonus, acercandose al Rey enemigo
				KingSafety[cside]	+= (BISHOP_KING_TROPISM * BishopTropism[sq][KING_SQ(cside)]) / 4;

				// Desarrollo lento
				if (sq_rel == C8 || sq_rel == F8)
					MidGame[side]		-= BISHOP_SLOW_DEVELOPMENT_MID;

				// Defendiendo a alfiles con peones
				if (PAWN_ATTACK_BB(sq, side))
					MidGame[side]		+= BISHOP_PROTECTED_BY_PAWN_MID;

				// Alfil atrapado
				if (mob < 5 && (SQ_BB(sq_rel) & BISHOP_TRAPPED_A2_A3_BB))
					MidEndGame[side]	-= BISHOP_TRAPPED * ((SQ_BB(sq_rel) & BISHOP_TRAPPED_A2_BB) ? 3 : 1);

				// Alfil malo
				if (!two_bishop && mob < 8)		{
					mob_bb			= PAWN_BB(side) & SquareColorBB[sq];
					bad_bishop		= 5 * PopCountBB(mob_bb & BAD_B5) + 3 * PopCountBB(mob_bb & BAD_B3) + 2 * PopCountBB(mob_bb & BAD_B2) + PopCountBB(mob_bb & BAD_B1);

					MidGame[side]	-= (BAD_BISHOP_MID * bad_bishop) / 16;
					EndGame[side]	-= (BAD_BISHOP_END * bad_bishop) / 16;
				}
			}
		}
}


// Evalúa las Torres
void	RookEval()				{
	int			center_file_dist, last_rank_dist, view, tmp,
				mob;
	TURN		side, cside;
	SQUARE		sq, sq_rel, sq2;
	BITBOARD	bb, bb2, mob_bb, bb3;


	TURN_LOOP(side)
		if ((bb = ROOK_BB(side)))				{
			cside		= FLIP_TURN(side);
			view		= side ? 56 : 0;
			bb2			= MOBILITY_TARGET(cside);


			for (; bb; bb ^= SQ_BB(sq))			{
				sq				= LSB_BB(bb);
				sq_rel			= sq ^ view;
				center_file_dist= DistanceCenterFile[sq];
				last_rank_dist	= DISTANCE_LAST_RANK(sq_rel);
				mob_bb			= ROOK_MOVES_BB(sq, ALL_PIECES_BB);
				mob				= 2 * PopCountBB(mob_bb & FileBB[sq] & bb2) + PopCountBB(mob_bb & RankBB[sq] & bb2);


				// Agregando ataques de Torres a la tabla de ataques
				PutAttack(side, mob_bb, ROOK_ATTACK_KING, 12);

				// Material y PST
				MidGame[side]	+= (ROOK_MOB_MID * mob) / 4 - ROOK_DIST_CENTER_FILE_MID * center_file_dist;
				EndGame[side]	+= (ROOK_MOB_END * mob) / 4 - ROOK_DIST_CENTER_FILE_END * center_file_dist - ROOK_DIST_LAST_RANK_END * last_rank_dist;

				// Bonus, acercandose al Rey enemigo
				KingSafety[cside]	+= ROOK_KING_TROPISM * Distance[sq][KING_SQ(cside)];

				switch (GET_FILE_STATE(sq, side))				{
					case FILE_HALF_OPEN:
						// Torre en columna semi-abierta
						MidEndGame[side]		+= ROOK_IN_HALF_OPEN_FILE;

						// Atacando al Rey enemigo
						if (abs(FILE(KING_SQ(cside)) - FILE(sq)) < 2)
							AttackWeight[side]	+= ROOK_ATTACK_KING_HALF_OPEN_FILE;
						break;

					case FILE_OPEN:
						// Torre en columna abierta
						MidEndGame[side]		+= ROOK_IN_OPEN_FILE;

						// Atacando al Rey enemigo
						if (abs(FILE(KING_SQ(cside)) - FILE(sq)) < 2)
							AttackWeight[side]	+= ROOK_ATTACK_KING_OPEN_FILE;

						// Torres y Damas dobladas en columna abierta
						if (mob_bb & FileBB[sq] & (ROOK_BB(side) | QUEEN_BB(side)))
							MidGame[side]		+= ROOK_SUPPORT_MID * ((mob_bb & FileBB[sq] & ROOK_BB(side)) ? 2 : 1);
						break;

					default:;
				}

				// Atrapada, en esquina del tablero y peones amigos bloqueando las salidas. Idea de [CPW]
				if (mob < 6)	{
					switch (KING_SQ(side) ^ view)				{
						case F8: case G8:	tmp		= (DistanceLineal[sq_rel][H8] < 2) * 2;		break;
						case C8: case B8:	tmp		= (DistanceLineal[sq_rel][A8] < 2);			break;
						case H8: case G7:	tmp		= (sq == KING_SQ(side) + Front[side]) * 2;	break;
						case A8: case B7:	tmp		= (sq == KING_SQ(side) + Front[side]);		break;
						default:			tmp		= 0;
					}

					if (tmp)	{
						MidEndGame[side]	-= ROOK_TRAPPED;
						RookTrapped[side]	|= tmp;
					}
				}

				// Torre en 7ma fila si peones enemigos en 7ma o Rey enemigo en 8va
				if (IN_7th(sq_rel, side))		{
					MidGame[side]	+= ROOK_7_MID;
					EndGame[side]	+= ROOK_7_END;
				}

				// Torre inmobilizada por propios peones pasados
				if (RANK(sq_rel) < RANK_4 && MATCH(PAWN | side, sq - Front[side]) && !PAWNS_IN_FRONT_BB(sq, cside))
					EndGame[side]	-= OWN_ROOK_BLOCK_PASSED_PAWN_END * (RANK_4 - RANK(sq_rel));

				// Torre atrapadas por peones pasados
				/// se repite
				if (RANK(sq_rel) > RANK_5 && MATCH(PAWN | cside, sq + Front[side]) && !PAWNS_IN_FRONT_BB(sq, side))
					EndGame[side]	-= COMPELLING_END * (RANK(sq_rel) - RANK_5);

				// Torre apoyando a peones pasados
				bb3		= mob_bb & ForwardBB[side][sq] & PAWN_BB(side);
				if (bb3 && (sq2 = LSB_BB(bb3)) && !PAWNS_IN_FRONT_BB(sq2, cside))
					EndGame[side]	+= ROOK_SUPP_PASSED_END;

				// Torre atacando a peones pasados por detras
				bb3		= mob_bb & ForwardBB[cside][sq] & PAWN_BB(cside);
				if (bb3 && (sq2 = LSB_BB(bb3)) && !PAWNS_IN_FRONT_BB(sq2, side))
					EndGame[side]	+= ROOK_ATTACK_PASSED_END;
			}
		}
}


// Evalúa las Damas
void	QueenEval()				{
	int			last_rank_dist, center_dist, view, mob;
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, mob_bb;


	TURN_LOOP(side)
		if ((bb = QUEEN_BB(side))){
			cside		= FLIP_TURN(side);
			view		= side ? 56 : 0;


			for (; bb; bb ^= SQ_BB(sq))			{
				sq				= LSB_BB(bb);
				sq_rel			= sq ^ view;
				last_rank_dist	= DISTANCE_LAST_RANK(sq_rel);
				center_dist		= DistanceCenter[sq];
				mob_bb			= QUEEN_MOVES_BB(sq, ALL_PIECES_BB);
				mob				= PopCountBB(mob_bb & ~ALL_PIECES_BB);


				// Agregando ataques de Damas a la tabla de ataques
				PutAttack(side, mob_bb, QUEEN_ATTACK_KING, 16);

				// Material y PST
				MidGame[side]	+= (QUEEN_MOB_MID * mob) / 4 - QUEEN_DIST_CENTER_MID * center_dist - QUEEN_DIST_LAST_RANK_MID * last_rank_dist;
				EndGame[side]	+= (QUEEN_MOB_END * mob) / 4 - QUEEN_DIST_CENTER_END * center_dist;

				// Bonus, acercandose al Rey enemigo
				KingSafety[cside]	+= QUEEN_KING_TROPISM * Distance[sq][KING_SQ(cside)];

				// Dama en 7ma fila si peones enemigos en 7ma o Rey enemigo en 8va
				if (IN_7th(sq_rel, side))		{
					MidGame[side]	+= QUEEN_7_MID;
					EndGame[side]	+= QUEEN_7_END;
				}

				// Dama atrapadas por peones pasados
				/// se repite
				if (RANK(sq_rel) > RANK_5 && MATCH(PAWN | cside, sq + Front[side]) && !PAWNS_IN_FRONT_BB(sq, side))
					EndGame[side]	-= COMPELLING_END * 2 * (RANK(sq_rel) - RANK_5);
			}
		}
}


// Evalúa los Reyes
void	KingEval()				{
	int			last_rank_dist, center_dist, view, king_attack;
	TURN		side, cside;
	SQUARE		sq;
	BITBOARD	mob_bb, bb, bb2, king_front_bb;


	TURN_LOOP(side)				{
		cside			= FLIP_TURN(side);
		view			= side ? 56 : 0;
		sq				= KING_SQ(side);
		last_rank_dist	= DISTANCE_LAST_RANK(sq ^ view);
		center_dist		= DistanceCenter[sq];
		mob_bb			= KING_MOVES_BB(sq);


		// PST
		MidGame[side]	+= KING_MAT_MID - KING_DIST_CASTTLE_MID * DistanceCasttle[sq ^ view] - KING_DIST_LAST_RANK_MID * last_rank_dist
						+  ExtraShield(FILE(sq), side);
		EndGame[side]	+= KING_MAT_END - KING_DIST_CENTER_END * center_dist
						+  KING_TROPISM_WEAK_PAWN_END * PopCountBB(mob_bb & ~PawnAttBB[cside] & PAWN_BB(cside));

		/*
		 * ATAQUE AL REY
		 * Mezcla de la seguridad del rey de [Rebel], [Stockfish] y otras ideas.
		 * Tiene en cuenta la cantidad de ataques de piezas y su tipo de piezas a casillas cercanas al Rey [Stockfish].
		 * Tambien analiza si esas casillas estan defendidas o ucupadas por piezas del mismo bando o del
		 * enemigo o si no hay peones en esas casillas [Rebel].
		 */
		if (AttackCount[cside] > 16 && PieceMat[cside] > QUEEN_VALUE + BISHOP_VALUE)			{
			bb2			= EV_ATTACKED_BB(cside);				// casillas atacadas por el enemigo
			king_front_bb= KingFrontBB[side][sq];				// zona de enfrente del Rey
			bb			= king_front_bb & bb2 & ~PAWN_BB(side);	// zona de enfrente del Rey sin peones y atacada por el enemigo

			king_attack	= PAWN_ATTACK_KING * PopCountBB(KING_ZONE[side] & PawnAttBB[cside])			// Penaliza, Ataques de Peones a la zona del Rey
						+ AttackWeight[cside]														// Penaliza, Zona de Rey Atacada(segun atacante) o Rey en centro de tablero
						+ KING_ONLY_DEFEND * PopCountBB(mob_bb & bb2 & ~EV_ATTACKED_WITHOUT_KING_BB(side))				// Penaliza, Rey unico defensor de casillas atacadas
						+ KING_WITHOUT_FRIEND * PopCountBB(king_front_bb & ~bb2 & ~PieceBB[side])	// Penaliza, casillas frente al Rey no atacadas, pero vacia o pieza enemiga ocupandola
						+ KING_WITH_ENEMY * PopCountBB(bb & ~PieceBB[side]) + KING_WITH_ENEMY2 * PopCountBB(bb & PieceBB[side]);		// Penaliza, casillas frente a Rey atacadas no ocupadas por peon del mismo bando, duplicar si no ocupada por el mismo bando

			MidEndGame[side]	-= SafetyTable[MIN(99, king_attack / 4)];
		}
	}
}


// Algunos valores para el desvalance material segun Larry Kaufman
void	MaterialEval()			{
	TURN	side, cside;
	int		mid_game, end_game, mat_diff, mid_end_game,
			mid_game_mat[2]	= {0, 0},
			end_game_mat[2]	= {0, 0};


	if (ProbeMatScore(&mid_game, &end_game, (U32)HashMat) == -MATE)				{
		TURN_LOOP(side)			{
			cside	= FLIP_TURN(side);

			// Incrementa valor de los Caballos en posiciones cerradas y decrementa en abiertas, lo contrario con las Torres
			// Penaliza la existencia de pocos peones, mayor posibilidad de tablas
			mid_end_game	= PAWNS(side) * (KNIGHTS(side) * KNIGHT_ADJUST - ROOKS(side) * ROOK_ADJUST)
							- PAWN_ADJUST / (PAWNS(side) + 1);

			// Penalización, redundancia de piezas cuando se tiene mas de 1 caballo
			if (KNIGHTS(side) > 1)
				mid_end_game	-= KNIGHT_PAIR;

			// Penalización, redundancia de piezas cuando se tiene mas de 1 torre
			if (ROOKS(side) > 1)
				mid_end_game	-= ROOK_PAIR;

			// Bonificando pareja de alfiles
			if ((BISHOP_BB(side) & WHITE_SQUARES_BB) && (BISHOP_BB(side) & BLACK_SQUARES_BB))	{
				mid_end_game		+= BISHOP_PAIR_BASE - BISHOP_PAIR * ALL_PAWNS;

				if (!BISHOPS(cside))
					mid_end_game	+= KNIGHTS(cside) ? BISHOP_PAIR_WITH_MINOR_PIECE : BISHOP_PAIR_WITHOUT_MINOR_PIECE;
			}

			// Bonificando el material de piezas
			mid_game_mat[side]	+= KNIGHTS(side) * KNIGHT_MAT_MID + BISHOPS(side) * BISHOP_MAT_MID + ROOKS(side) * ROOK_MAT_MID + QUEENS(side) * QUEEN_MAT_MID
								+  PAWNS(side)   * PAWN_MAT_MID   + mid_end_game;
			end_game_mat[side]	+= KNIGHTS(side) * KNIGHT_MAT_END + BISHOPS(side) * BISHOP_MAT_END + ROOKS(side) * ROOK_MAT_END + QUEENS(side) * QUEEN_MAT_END
								+  PAWNS(side)   * PAWN_MAT_END   + mid_end_game;
		}


		mat_diff	= PieceMat[WHITE] - PieceMat[BLACK];
		side		= WHITE;

		if (mat_diff < 0)		{
			mat_diff	= -mat_diff;
			side		= BLACK;
		}


		// Mayor contra peones
		if (mat_diff >= ROOK_VALUE)				{
			mid_game_mat[side]	+= ROOK_VS_PAWN_MID;
			end_game_mat[side]	+= ROOK_VS_PAWN_END;
		}

		// Menor contra peones
		else if (mat_diff >= KNIGHT_VALUE)		{
			mid_game_mat[side]	+= MINOR_VS_PAWN_MID;
			end_game_mat[side]	+= MINOR_VS_PAWN_END;
		}

		// Menor contra torre
		else if (mat_diff >= ROOK_VALUE - BISHOP_VALUE)			{
			mid_game_mat[side]	+= ROOK_VS_MINOR_MID;
			end_game_mat[side]	+= ROOK_VS_MINOR_END;
		}

		else	{
			mat_diff	= KNIGHTS(WHITE) + BISHOPS(WHITE) - KNIGHTS(BLACK) - BISHOPS(BLACK);
			side		= WHITE;

			if (mat_diff < 0)	{
				mat_diff	= -mat_diff;
				side		= BLACK;
			}

			// Dos menores contra torre
			if (mat_diff > 1)	{
				mid_game_mat[side]	+= TWO_MINOR_VS_ROOK_MID;
				end_game_mat[side]	+= TWO_MINOR_VS_ROOK_END;
			}
		}


		mid_game	= mid_game_mat[WHITE] - mid_game_mat[BLACK];
		end_game	= end_game_mat[WHITE] - end_game_mat[BLACK];

		// Guardando en hash de material la evaluacion
		StoreMatScore(mid_game, end_game, (U32)(HashMat));
	}

	// Asignando valores de las tablas hash
	MidGame[WHITE]	+= mid_game;
	EndGame[WHITE]	+= end_game;
}


// Inicializa algunos datos de ataque necesarios en la evaluacion
void	IniEvAttack()			{
	memset(Weight,		0, sizeof(Weight));

	// Inicializando tabla de ataques
	PawnAttBB[BLACK]	= ((BLACK_PAWN_BB & (U64)(~FILE_A_BB)) >> 9) | ((BLACK_PAWN_BB & (~FILE_H_BB)) >> 7);
	PawnAttBB[WHITE]	= ((WHITE_PAWN_BB & (U64)(~FILE_A_BB)) << 7) | ((WHITE_PAWN_BB & (~FILE_H_BB)) << 9);

	PieceAttBB[BLACK]	= PieceAttBB[WHITE] = 0;

	KingAttBB[BLACK]	= KING_MOVES_BB(KING_SQ(BLACK));
	KingAttBB[WHITE]	= KING_MOVES_BB(KING_SQ(WHITE));

	KING_ZONE[BLACK]	= WHITE_QUEEN_BB ? KingZoneBB[BLACK][KING_SQ(BLACK)] : 0;
	KING_ZONE[WHITE]	= BLACK_QUEEN_BB ? KingZoneBB[WHITE][KING_SQ(WHITE)] : 0;
}


// Evaluación completa de la posicion
int		FullEval()				{
	TURN	side,
			cturn		= FLIP_TURN(Turn);

	IniEvAttack();

	// Evaluando posicion
	KnightEval();
	BishopEval();
	RookEval();
	QueenEval();
	KingEval();
	PawnEval();
	MaterialEval();

	// Recolectando evaluacion de seguridad del rey de cada bando
	TURN_LOOP(side)
		MidEndGame[side]	+= (PROTECTED_PIECES * PopCountBB((PieceBB[side] ^ PAWN_BB(side) ^ KING_BB(side)) & EV_ATTACKED_BB(side))) / 4
							- (KingSafety[side] * PieceMat[FLIP_TURN(side)]) / (4 * INITIAL_MATERIAL);


	// Recolectando evaluacion para cada fase del juego y adiciona el Tempo
	*MidGame		= MidGame[Turn]		- MidGame[cturn] + TEMPO_MID;
	*EndGame		= EndGame[Turn]		- EndGame[cturn] + TEMPO_END;
	*MidEndGame		= MidEndGame[Turn]	- MidEndGame[cturn];

	// Interpolando la evaluación entre el mediojuego y final. No es necesario hacerlo si se esta en un final simple
	return *MidEndGame + ((*MidGame) * GAME_PHASE + (*EndGame) * (256 - GAME_PHASE)) / 256;
}


// Evaluación de la posición con salida rápidas: lazy eval, tablas hash, final tablas
int		Eval(int alpha, int beta)				{
	// Comprueba si la posición esta evaluada
	int score	= ProbeScore(Hash);
	if (score != -MATE)
		return score;

	int		lazy_ev_margen	= 850 - (175 * GAME_PHASE) / 64,	// Entre 150 en mediojuego y 850 en final
			mat_diff		= PieceMat[Turn] + PawnMat[Turn] - PieceMat[FLIP_TURN(Turn)] - PawnMat[FLIP_TURN(Turn)];


	/*
	 * Si se tiene mucha ventaja(desventaja) no hace falta seguir evaluando, cuanto menor es el valor de
	 * 'lazy_ev_margen' más rápido sera el programa, con riesgo de dejar algún movimiento súper bueno
	 */
	if (mat_diff - lazy_ev_margen >= beta)
		return beta;

	if (mat_diff + lazy_ev_margen <= alpha)
		return alpha;


	// Evaluacion y coreciones si el final soportado por el motor
	score		= FullEval();
	int draw	= SupportedEndGames();
	if (draw)
		score	= (16 * score) / draw;


	// Guardando y devolviendo evaluacion
	StoreScore(score, Hash);
	return score;
}


// Detecta tablas por insuficiencia material
int		MatInsuf()				{
	BITBOARD	bb;

	switch ((U32)HashMat)		{
		case KK: case KNK: case KKN: case KNNK: case KKNN: case KKB: case KBK:	return 1;

		case KBBK: case KKBB: case KBKB: case KBBKB: case KBKBB: case KBBKBB:
			bb	= BLACK_BISHOP_BB | WHITE_BISHOP_BB;
			return !((bb & BLACK_SQUARES_BB) && (bb & WHITE_SQUARES_BB));

		default: return 0;
	}
}


// Detecta cuando un bando tiene insuficiencia material
int		MatInsufSide(TURN side)	{
	if (PAWNS(side) || ROOKS(side) || QUEENS(side))
		return 0;

	if (PieceMat[side] < ROOK_VALUE)
		return 1;

	if ((KNIGHTS(side) && BISHOPS(side)) || KNIGHTS(side) > 2)
		return 0;

	if (KNIGHTS(side))
		return 1;

	BITBOARD	bb	= BLACK_BISHOP_BB | WHITE_BISHOP_BB;
	return !((bb & BLACK_SQUARES_BB) && (bb & WHITE_SQUARES_BB));
}


/*
 * Inicializa las tablas del tropismo.
 * Ver (Evaluation Function Draft) de <www.chessprogramming.org/Home/Evaluation/Evaluation Function Draft>
 */
/// no me gusta para nada esta forma del tropismo del alfil
void	TropismIni()			{
	SQUARE	sq1, sq2;
	int	diag_NW[64]			= {0,1,2,3,4,5,6,7,1,2,3,4,5,6,7,8,2,3,4,5,6,7,8,9,3,4,5,6,7,8,9,10,4,5,6,7,8,9,10,11,5,6,7,8,9,10,11,12,6,7,8,9,10,11,12,13,7,8,9,10,11,12,13,14,},
		diag_NE[64]			= {7,6,5,4,3,2,1,0,8,7,6,5,4,3,2,1,9,8,7,6,5,4,3,2,10,9,8,7,6,5,4,3,11,10,9,8,7,6,5,4,12,11,10,9,8,7,6,5,13,12,11,10,9,8,7,6,14,13,12,11,10,9,8,7,},
		bonus_distance[15]	= {5, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	SQ_LOOP(sq1)
		SQ_LOOP(sq2)
			BishopTropism[sq1][sq2]	= bonus_distance[abs(diag_NE[sq1] - diag_NE[sq2])] + bonus_distance[abs(diag_NW[sq1] - diag_NW[sq2])];
}


// Precalcula datos necesarios en la evaluacion
void	EvalIni()				{
	TropismIni();
}


// Algunos finales soportados. El retorno por debajo de 16	es para posiciones tablas
/// demasiado codigo 
int		SupportedEndGames()		{
	SQUARE		P, r, R, k, K, prom_sq;
	int			dist, all_pawns_in_file,
				mat_sum			= PieceMat[BLACK] + PieceMat[WHITE] + PawnMat[WHITE] + PawnMat[BLACK],
				mat_diff		= abs(PieceMat[BLACK] + PawnMat[BLACK] - PieceMat[WHITE] - PawnMat[WHITE]),
				pawn_diff		= abs(BLACK_PAWNS - WHITE_PAWNS);
	U32			MatHashPiece	= (U32)(HashMat & 0xFFFFFF00);
	BITBOARD	bb				= BLACK_PAWN_BB		| WHITE_PAWN_BB,
				bb2				= BLACK_BISHOP_BB	| WHITE_BISHOP_BB;
	TURN		side			= 2,
				cside;

	// Salida Rapida, no se tiene suficiente material
	if (GAME_PHASE > 102)
		return 0;


	// Se pone el punto de vista del tablero del bando que tenga peones, sino al que tenga alfil
	if (BLACK_PAWNS && !WHITE_PAWNS)
		side	= BLACK;

	else if (WHITE_PAWNS && !BLACK_PAWNS)
		side	= WHITE;

	else if (BLACK_BISHOP_BB)
		side	= BLACK;

	else if (WHITE_BISHOP_BB)
		side	= WHITE;

	// Calcula cual es el Peon mas avanzado del bando con Peones
	for (P = LSB_BB(PAWN_BB(side)); ForwardBB[side][P] & PAWN_BB(side); )
		P = LSB_BB(ForwardBB[side][P] & PAWN_BB(side));

	cside				= FLIP_TURN(side);
	K					= KING_SQ(side);
	k					= KING_SQ(cside);
	prom_sq				= LSB_BB(ForwardBB[side][P] & (RANK_1_BB | RANK_8_BB));
	all_pawns_in_file	= (~FileBB[P] & bb) == 0;


	switch ((U32)HashMat)		{
		case KNNKP: case KNNKPP: case KNNKPPP: case KPKNN: case KPPKNN: case KPPPKNN:
		case KNKP: case KBKP: case KPPKN: case KPPKB: case KPKN: case KPKB: case KNKPP: case KBKPP:
		case KRKNP: case KRKBP: case KNPKR: case KBPKR:
		case KNNKNP: case KNNKBP: case KBNKNP: case KBNKBP: case KBBKNP: case KBBKBP: case KNPKNN: case KBPKNN: case KNPKBN: case KBPKBN: case KNPKBB: case KBPKBB:
		case KRPKNN: case KRPKBN: case KRPKBB: case KNNKRP: case KBNKRP: case KBBKRP:
		case KRPKRN: case KRPKRB: case KRNKRP: case KRBKRP:
		case KQNKQP: case KQBKQP: case KQPKQN: case KQPKQB:
		case KRNKNNP: case KRNKBNP: case KRNKBBP: case KRBKBBP: case KNNPKRN: case KBNPKRN: case KBBPKRN: case KBBPKRB:
		case KNNKNPP: case KBNKNPP: case KBNKBPP: case KBBKBPP: case KNPPKNN: case KNPPKBN: case KBPPKBN: case KBPPKBB:
		case KBNNKRP: case KRPKBNN:
		case KRNKRPP: case KRBKRPP: case KRPPKRN: case KRPPKRB:
		case KQNKQPP: case KQBKQPP: case KQPPKQN: case KQPPKQB:
		case KRPPKBNN: case KBNNKRPP:
			return DRAW_SCORE(mat_sum);

		// Lo importante para la defensa es no llevar el Rey a la esquina del mismo color que el Alfil
		case KRBKR: case KRKRB:
			if (((bb2 & BLACK_SQUARES_BB) && (KING_BB(cside) & BLACK_CORNER_BB))
			||  ((bb2 & WHITE_SQUARES_BB) && (KING_BB(cside) & WHITE_CORNER_BB)))
				return 0;
			return DRAW_SCORE(mat_sum);

		// Soporte para KPK o KKP
		case KPK: case KKP:
			// El peon corona por si solo
			if (DistanceLineal[prom_sq][P] + (Turn == cside) < DistanceLineal[prom_sq][k] + (Turn == side))
				return 0;

			// Rey enemigo captura al peón
			if (DistanceLineal[K][P] + (Turn == cside) > DistanceLineal[P][k] + (Turn == side))
				return DRAW_SCORE(mat_sum);

			// peon en columna A o H
			if ((bb & (FILE_A_BB | FILE_H_BB)) && (KING_BB(cside) & KingZoneBB[cside][prom_sq ^ 1]))
				return DRAW_SCORE(mat_sum);

			// Rey enemigo frente al Peon
			if (DistanceLineal[K][P + Front[side]] + (Turn == cside) > DistanceLineal[P + Front[side]][k] + (Turn == side))
				return DRAW_SCORE(mat_sum);

			if (K + 2 * Front[side] == k)		{
				// Rey del bando fuerte al lado del Peon y el otro al frente del rey
				if (P + 1 == K || P - 1 == K)
					return DRAW_SCORE(mat_sum);

				// Rey del bando fuerte al frente del Peon y el otro al frente del rey
				if ((P + Front[side] == K) && (Turn == side))
					return DRAW_SCORE(mat_sum);
			}
			break;

		// Soporte para KPK, KKP con todos los peones en columna A o H
		case KPPK: case KKPP: case KPPPK: case KKPPP: case KPPPPK: case KKPPPP:
			if (all_pawns_in_file && (bb & (FILE_A_BB | FILE_H_BB)) && (KING_BB(cside) & KingZoneBB[cside][prom_sq ^ 1]))
				return DRAW_SCORE(mat_sum);
			break;

		// Soporte para KBPK, KKBP con todos los peones en columna A o H
		case KBPK: case KBPPK: case KBPPPK: case KBPKP: case KBPPKPP: case KBPKPP: case KBPPKP: case KBPPPKPP: case KBPPPKP: case KBPPKPPP: case KBPKPPP: 
		case KKBP: case KKBPP: case KKBPPP: case KPKBP: case KPPKBPP: case KPPKBP: case KPKBPP: case KPPKBPPP: case KPKBPPP: case KPPPKBPP: case KPPPKBP:
			if (all_pawns_in_file && (bb & (FILE_A_BB | FILE_H_BB)) && !(bb2 & SquareColorBB[prom_sq]) && (KING_BB(cside) & KingZoneBB[cside][prom_sq ^ 1]))
				return DRAW_SCORE(mat_sum);
			break;

		// Soporte para KQKP, KPKQ, con todos los peones en columna A, C, F o H
		case KQKP: case KPKQ: case KQKPP: case KPPKQ: case KQKPPP: case KPPPKQ:
			if (all_pawns_in_file && MATCH(PAWN | side, prom_sq - Front[side]))	{
				if(((FILE_A_BB & bb) && (KING_BB(cside) & KQKP_DB1[cside]))
				|| ((FILE_H_BB & bb) && (KING_BB(cside) & KQKP_DB2[cside]))
				|| ((FILE_C_BB & bb) && (KING_BB(cside) & KQKP_DB3[cside]))
				|| ((FILE_F_BB & bb) && (KING_BB(cside) & KQKP_DB4[cside])))
					return DRAW_SCORE(mat_sum);
			}
			return 0;

		// Lo importante es no llevar rey débil a esquina de color contrario al alfil, para poder defenderse
		case KRKB: case KBKR:
			if(((bb2 & BLACK_SQUARES_BB) && (KING_BB(side) & ARRIM_WHITE_CORNER_BB))
			|| ((bb2 & WHITE_SQUARES_BB) && (KING_BB(side) & ARRIM_BLACK_CORNER_BB)))
				return DRAW_SCORE(mat_sum);
			break;

		/*
		 * 1) Controlar casilla por delante del peón de color distinto al alfil con pieza(NO REY)
		 * 2) Ocupar casilla por delante del peón de color distinto al alfil
		 */
		case KNKBP: case KBKBP: case KBPKN: case KBPKB:
			bb	= ForwardBB[side][P] & ((bb2 & BLACK_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB);
			if (bb & (PieceBB[cside] | EV_ATTACKED_WITHOUT_KING_BB(cside)))
				return DRAW_SCORE(mat_sum);
			break;

		case KRKP: case KPKR: case KRKPP: case KPPKR:
			if (all_pawns_in_file)				{
				r		= LSB_BB(ROOK_BB(cside));

				// Rey enemigo en frente del peon o el Rey no esta cerca para apoyar el peón
				if (IN_FRONT_BB(P, side, KING_BB(cside)) || (DistanceLineal[P][prom_sq] > 1 && DistanceLineal[K][P] > 2))
					return 0;

				// Peon en 7ma apoyado por su Rey
				if (DistanceLineal[P][prom_sq] < 2 && DistanceLineal[K][P] < 2)
					return DRAW_SCORE(mat_sum);

				// Torre cortando al rey enemigo en 5ta
				if (abs(RANK(prom_sq) - RANK(r)) > 3 && abs(RANK(prom_sq) - RANK(r)) <= abs(RANK(K) - RANK(r)))
					return 0;

				// Equivalente al soporte de [Fruit] al final KRKP
				if ((Turn == cside) && DistanceLineal[K][P] == 2 && abs(RANK(K) - RANK(P)) < 2 && FILE(r) != (FILE(K) + FILE(P)) / 2)
					return 0;

				dist	= DistanceLineal[K][prom_sq] + DistanceLineal[P][prom_sq] + (K == prom_sq)
						- (FILE(r) != FILE(P) && RANK(r) != RANK(prom_sq));

				if (P + Front[side] == K)		{
					if (FileBB[P] & (FILE_A_BB | FILE_H_BB))
						break;

					dist++;
				}

				if (DistanceLineal[k][prom_sq] + (Turn == side) >= dist + (Turn == cside))
					return DRAW_SCORE(mat_sum);
			}
			return 0;

		case KRKRP:	case KRPKR:
			R		= LSB_BB(ROOK_BB(side));
			r		= LSB_BB(ROOK_BB(cside));

			// Defensa de ultima fila
			if (IN_FRONT_BB(P, side, KING_BB(cside)) && (DistanceLineal[P][prom_sq] < 3) && 
			(PAWN_BB(side) & ROOK_KNIGHT_FILE_BB) && (RankBB[prom_sq] & RankBB[k] & RankBB[r]))
				return DRAW_SCORE(mat_sum);

			// Peon en columna torre
			if (FileBB[P] & (FILE_A_BB | FILE_H_BB))		{
				if (DistanceLineal[prom_sq][P] == 1)		{
					// Rey enemigo frente al Peon
					if (IN_FRONT_BB(P, side, KING_BB(cside)) && abs(FILE(P) - FILE(r)) == 1 && abs(FILE(P) - FILE(k) < 5))
						return DRAW_SCORE(mat_sum);

					// Torre del mismo bando frente al Peon
					if (R == prom_sq && FILE(P) == FILE(r) && (DistanceLineal[k][R] < 3 || (k + Front[side] == prom_sq && DistanceLineal[k][P] > 5)))
						return DRAW_SCORE(mat_sum);
				}

				// Vancura posicion
				else if (RANK(r) == RANK(P) && MIN(DistanceLineal[k][prom_sq], DistanceLineal[k][prom_sq ^ 7]) < 2 && 
				(ForwardBB[side][P] & ROOK_BB(side)))
					return DRAW_SCORE(mat_sum);
			}

			// Defensa del lado corto
			else if (abs(FILE(k) - FILE(r)) > abs(FILE(P) - FILE(r)) && DistanceLineal[K][P] == 1 && 
			abs(FILE(P) - FILE(r)) > 3 && abs(FILE(P) - FILE(k)) < 3 && DistanceLineal[k][r] > 5)
				return DRAW_SCORE(mat_sum);

			// Philidor position
			if (PawnPassedBB[side][P] & KING_BB(cside))		{
				if (DistanceLineal[P][prom_sq] > 2)			{
					if (abs(RANK(prom_sq) - RANK(K)) > 2 && abs(RANK(prom_sq) - RANK(r)) == 2)
						return DRAW_SCORE(mat_sum);
				}
				else if (abs(RANK(prom_sq) - RANK(r)) > 5)
					return DRAW_SCORE(mat_sum);
			}
			break;
		default:;
	}

	/*
	 * Regla general(no perfecta) para identificar posiciones tablas:
	 * Si ambos bandos no tienen peones se necesita el equivalente a casi 4 peones para poder ganar
	 */
	if (!ALL_PAWNS && mat_diff < 7 * PAWN_VALUE / 2)			{
		if (PieceMat[BLACK] == PieceMat[WHITE])
			return DRAW_SCORE(mat_sum);

		if(PieceMat[BLACK] < PieceMat[WHITE] && !(KING_BB(BLACK) & ARRIM_BB))
			return DRAW_SCORE(mat_sum);

		if(PieceMat[BLACK] > PieceMat[WHITE] && !(KING_BB(WHITE) & ARRIM_BB))
			return DRAW_SCORE(mat_sum);
	}

	// Dificil ganar cuando no existen peones y ambos bandos no tienen material suficiente para ganar
	if (!ALL_PAWNS && PieceMat[BLACK] < ROOK_VALUE && PieceMat[WHITE] < ROOK_VALUE
	&& (!(ARRIM_BB & WHITE_KING_BB) || !PieceMat[BLACK]) && (!(ARRIM_BB & BLACK_KING_BB) || !PieceMat[WHITE]))
		return DRAW_SCORE(mat_sum);

	// Dificil ganar cuando el Rey en desventaja esta al frente del unico peon y se tiene material igual
	if (PieceMat[BLACK] == PieceMat[WHITE] && ALL_PAWNS == 1 && IN_FRONT_BB(P, side, KING_BB(cside)))
		return 128;

	// Penalizando a bando que posee material insuficiente para ganar
	if (MatInsufSide(BLACK))
		EndGame[BLACK]	/= 2;

	if (MatInsufSide(WHITE))
		EndGame[WHITE]	/= 2;

	// Asegurar no haber muchos peones pasados
	int draw_end = pawn_diff < 2 && ALL_PAWNS;
	switch (MatHashPiece)		{
		// Prefiere ir final de peones o de caballos
		case KK:					return KK_END;
		case KNKN: case KNNKNN:		return KNKN_END;

		case KBKB:
			// Alfiles de distinto color
			if ((bb2 & BLACK_SQUARES_BB) && (bb2 & WHITE_SQUARES_BB))
				return (pawn_diff < 4 && ALL_PAWNS) ? (KBKB_END / ALL_PAWNS) : 0;

			// Alfiles de igual color
			return (draw_end && ALL_PAWNS) ? (KBKB2_END / ALL_PAWNS) : 0;

		// Estos finales posiblemente progresen a un final tablas
		case KRKR:		return draw_end ? KRKR_END		: 0;
		case KRRKRR:	return draw_end ? KRRKRR_END	: 0;
		case KQKQ:		return draw_end ? KQKQ_END		: 0;
		case KRBKRB:	return ((bb2 & BLACK_SQUARES_BB) && (bb2 & WHITE_SQUARES_BB)) ? KRBKRB_END : KRBKRB2_END;
		case KQBKQB:	return ((bb2 & BLACK_SQUARES_BB) && (bb2 & WHITE_SQUARES_BB)) ? KQBKQB_END : KQBKQB2_END;
		case KBBKBB:	return (draw_end && ALL_PAWNS) ? KBBKBB_END / ALL_PAWNS : 0;
		default:		return 0;
	}
}


//	1050	1100	1150
