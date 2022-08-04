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
int			Weight[16],
			*MidGame			= Weight + 0,
			*EndGame			= Weight + 2,
			*MidEndGame			= Weight + 4,
			*AttackWeightMid	= Weight + 6,
			*AttackWeightEnd	= Weight + 8,
			*AttackCount		= Weight + 10,
			*KingSafety			= Weight + 12,
			*RookTrapped		= Weight + 14;

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
		default:				return KING_NO_FIANCHETTO_MID;
	}
}


// Evaluacion de otros paramatros del escudo del REY.
int		ExtraShield(int file, TURN side)		{
	// Enroque corto
	if (file > FILE_F)
		return FianchettoEval(Board[REL_SQ(G7, side)] ^ side) + ((RookTrapped[side] & (BK | WK)) ? 0 : KING_CASTTLE_DONE_MID);

	// Enroque largo
	if (file < FILE_D)
		return FianchettoEval(Board[REL_SQ(B7, side)] ^ side) + ((RookTrapped[side] & (BQ | WQ)) ? 0 : KING_CASTTLE_DONE_MID);

	// REY en el centro
	return	(QUEENS(FLIP_TURN(side)) && !((Casttle >> side) & (BQ | BK))) ? KING_IN_CENTER_MID : 0;
}


// Evalua a uno de los PEONES del escudo del REY
int		KingPawnShield(SQUARE sq, TURN side)	{
	int		score	= 0;
	TURN	cside	= FLIP_TURN(side);
	sq				= REL_SQ(sq, side);

	// Penalizacion, PEON movido del escudo del REY
	if (!MATCH(PAWN | side, sq)){
		if (OPEN_FILE(sq, side))
			score	+= KING_NO_PAWN_IN_FILE;					// No hay en la columna

		else if (MATCH(PAWN | side, sq + Front[side]))
			score	+= KING_PAWN_IN_3;							// Movido 1 casilla

		else score	+= KING_PAWN_IN_4;							// Movido 2 o mas casillas
	}

	// Penalizacion, avalancha hacia el REY de PEONES enemigos
	if (OPEN_FILE(sq, cside))
		score		+= KING_NO_PAWN_ENEMY_IN_FILE;				// No hay en la columna

	else if (MATCH(PAWN | cside, sq + Front[side]))
		score		+= KING_PAWN_ENEMY_IN_3;					// En 3ra fila

	else if (MATCH(PAWN | cside, sq + 2 * Front[side]))
		score		+= KING_PAWN_ENEMY_IN_4;					// En 4ta fila

	return score;
}


// Evalua a uno de los PEONES del escudo del REY
int		KingShield(TURN side)	{
	BITBOARD	bb		= BLACK_PAWN_BB | WHITE_PAWN_BB;
	int			file	= FILE(KING_SQ(side));

	// Enroque corto
	if (file > FILE_F)
		return KingPawnShield(H7, side) + KingPawnShield(G7, side) + KingPawnShield(F7, side) / 2;

	// Enroque largo
	if (file < FILE_D)
		return KingPawnShield(A7, side) + KingPawnShield(B7, side) + KingPawnShield(C7, side) / 2;

	// REY en el centro
	return KING_IN_OPEN_FILE * (!(FileBB[file] & bb) + !(FileBB[file - 1] & bb) + !(FileBB[file + 1] & bb));
}


// PEON retrasado. Solo tiene en cuenta a los PEONES y sus ataques para detectarlo
int		PawnBackward(SQUARE sq)	{
	SQUARE		to;
	TURN		side	= GET_COLOR(Board[sq]),
				cside	= FLIP_TURN(side);

	// Si no existe PEON vecino mas atrasado y existen PEONES enemigos al frente
	if (IS_OUTPOST(cside, sq) && IN_FRONT_BB(sq, side, PawnAttBB[side] | PAWN_BB(cside)))
		for (to	= sq + Front[side];; to += Front[side])			{
			// Es retrasado, PEON enemigo lo bloquea
			if (MATCH(PAWN | cside, to))
				return 1;

			// Es retrasado, mayoria de PEONES enemigos atacando, de lo contrario no lo es
			if (PAWN_ATTACK_BB(to, side))
				return COUNT_PAWN_ATTACK(to, cside) > COUNT_PAWN_ATTACK(to, side);

			// Es retrasado, mayoria de PEONES enemigos atacando
			if (PAWN_ATTACK_BB(to, cside))
				return 1;
		}

	return 0;
}


// Determina si un PEON puede defender a una casilla de una columna, si no es aislado.
int		SupportedFile(SQUARE sq, TURN side)		{
	SQUARE		to;
	int			front	= Front[side];
	TURN		cside	= FLIP_TURN(side);
	BITBOARD	bb		= ForwardBB[cside][sq];

	// no existen PEONES del mismo bando en la columna, no se puede defender
	if (!(bb & PAWN_BB(side)))
		return 0;

	// no existen PEONES enemigos en la columna o atacando la columna, puede defenderse
	if (!(bb & (PAWN_BB(cside) | PawnAttBB[cside])))
		return 1;

	// buscando en la columna si es posible defenderlo
	for (to = sq - front;; to -= front)	{
		// PEON encontrado, puede defenderse
		if (SQ_BB(to) & PAWN_BB(side))
			return 1;

		// PEONES enemigos controlando la casilla, no se puede defender
		if ((SQ_BB(to) & PAWN_BB(cside)) || PopCountBB(PAWN_ATTACK_BB(to, side)) < PopCountBB(PAWN_ATTACK_BB(to, cside)))
			return 0;
	}

	return 0;
}


// Similar a PEON retrasado, son avanzados que no pueden ser defendidos por peones
int		PawnAdvanced(SQUARE sq)	{
	TURN		side	= GET_COLOR(Board[sq]),
				cside	= FLIP_TURN(side);

	// No es avanzado. Existen PEONES vecinos por delante o no existen PEONES enemigos estorbando la defensa
	if ((OutpostBB[side][sq] & PAWN_BB(side)) || !(OutpostBB[cside][sq] & PAWN_BB(side)) || (SQ_BB(sq) & PawnAttBB[side]))
		return 0;

	// Comprobando si puede ser defendido por la izquierda
	if (FILE(sq) != FILE_A && SupportedFile(sq - 1, side))
		return 0;

	// Comprobando si puede ser defendido por la derecha
	return !(FILE(sq) != FILE_H && SupportedFile(sq + 1, side));
}


// PEON semi-pasado. Solo tiene en cuenta a los PEONES y sus ataques para detectarlo
int		PawnCandidate(SQUARE sq, TURN side)		{
	TURN	cside		= FLIP_TURN(side);

	// Si no hay mayoria de PEONES enemigos al frente no es considerado como candidato
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


// Actualiza ataques al REY enemigo y la tabla de ataques
void	PutAttack(TURN side, BITBOARD bb, int weight_mid, int weight_end, int count)			{
	// Actualizando tabla de ataques
	PieceAttBB[side]	|= bb;

	// Actualizando ataque al REY enemigo
	if ((bb &= KING_ZONE[FLIP_TURN(side)]))		{
		AttackWeightMid[side]	+= weight_mid * PopCountBB(bb);
		AttackWeightEnd[side]	+= weight_end * PopCountBB(bb);
		AttackCount[side]		+= count;						// K = 0, Q = 4, R = 3, B = 2, N = 2, P = 1
	}
}


// PEON pasado bloqueado
void	IsPawnPassedBloqued(TURN side, SQUARE bloqued_sq, SQUARE sq)			{
	int			out_post	= BISHOPS(side) && !KNIGHTS(side) && !(SquareColorBB[bloqued_sq] & BISHOP_BB(side)),	// Puntuacion por bloquear a PEON pasado, bonifica si el bloqueador no puede ser desalojado
				score		= ((CHEBYSHEV_DIST(sq, bloqued_sq) - 4) * (2 + out_post)) / 2,
				dist_prom	= MAX(1, CHEBYSHEV_DIST(sq, PROM_SQ(sq, side)));

	// PIEZA bloquea a PEON pasado
	switch (Board[bloqued_sq] ^ side)			{
		// TORRE inmobilizada por propios PEONES pasados
		case ROOK:
			MidEndGame[side]	+= OWN_ROOK_BLOCK_PASSED_PAWN;
			break;

		// TORRE y DAMA atrapadas por PEON pasado
		case WHITE_ROOK:
			MidGame[side]		+= ROOK_COMPELLING_MID / dist_prom;
			EndGame[side]		+= ROOK_COMPELLING_END / dist_prom;
			break;
		case WHITE_QUEEN:
			MidGame[side]		+= QUEEN_COMPELLING_MID / dist_prom;
			EndGame[side]		+= QUEEN_COMPELLING_END / dist_prom;
			break;

		// Buenos bloqueadores, ptos. extra si no se pueden desalojar
		case WHITE_KNIGHT:
			MidGame[side]		+= PAWN_PASSED_BLOCKED_BY_KNIGHT_MID * score;
			EndGame[side]		+= PAWN_PASSED_BLOCKED_BY_KNIGHT_END * score;
			break;
		case WHITE_BISHOP:
			MidGame[side]		+= PAWN_PASSED_BLOCKED_BY_BISHOP_MID * score;
			EndGame[side]		+= PAWN_PASSED_BLOCKED_BY_BISHOP_END * score;
			break;
		case WHITE_KING:
			MidEndGame[side]	+= PAWN_PASSED_BLOCKED_BY_KING * score;
			break;
		default:;
	}
}


// Evalua PEONES pasados
void	PawnPassedEval(BITBOARD pawns_passed)	{
	BITBOARD	bb, tmp_bb;
	TURN		side, cside;
	SQUARE		sq, sq_rel, to;
	int			view, last_rank_dist, protected, count, front, support, support_attack, tmp, center_file_dist;

	TURN_LOOP(side)				{
		view	= side ? 56 : 0;
		cside	= FLIP_TURN(side);
		front	= Front[side];

		for (bb = pawns_passed & PAWN_BB(side); bb; bb ^= SQ_BB(sq))			{
			tmp				= 0;
			sq				= LSB_BB(bb);
			sq_rel			= sq ^ view;
			last_rank_dist	= MAX(1, LAST_RANK_DIST(sq_rel));
			center_file_dist= MANHATTAN_DIST_CENTER_FILE(sq);

			// PEON pasado apoyado o atacado por torre por detras
			tmp_bb 			= ForwardBB[cside][sq] & (BLACK_ROOK_BB | WHITE_ROOK_BB) & ROOK_MOVES_BB(sq, ALL_PIECES_BB);
			support			= (tmp_bb & ROOK_BB(side))  != 0;
			support_attack	= (tmp_bb & ROOK_BB(cside)) != 0;

			if (support)
				MidEndGame[side]	+= ROOK_SUPP_PASSED;

			if (support_attack)
				MidGame[side]	+= ROOK_ATTACK_PASSED;

			// PEON pasado acercandose a casilla de promocion, es mas peligroso
			MidGame[side]	+= PAWN_PASSED_MID + PAWN_PASSED_DIST_LAST_RANK_MID * last_rank_dist + PAWN_PASSED_CENTER_FILE_MID * center_file_dist;
			EndGame[side]	+= PAWN_PASSED_END + PAWN_PASSED_DIST_LAST_RANK_END * last_rank_dist + PAWN_PASSED_CENTER_FILE_END * center_file_dist;

			// PEON pasado protegido, es mas peligroso
			protected		= PAWN_ATTACK_BB(sq, side) ? 4 : 0;
			if (PAWN_ATTACK_BB(sq + front, side))
				protected	= 8;

			if (protected)		{
				MidGame[side]	+= (PAWN_PASSED_PROTECTED_MID * protected) / 8;
				EndGame[side]	+= (PAWN_PASSED_PROTECTED_END * protected) / 8;
			}

			// Tropismo de REYES a PEON pasado
			EndGame[side]	+= PAWN_PASSED_ENEMY_KING_TROPISM_END	* CHEBYSHEV_DIST(sq + front, KING_SQ(cside))
							+  PAWN_PASSED_MY_KING_TROPISM_END		* CHEBYSHEV_DIST(sq + front, KING_SQ(side));

			// PEON pasado indetenible, el REY enemigo no puede alcanzarlo
			if (!PieceMat[cside]){
				to = PROM_SQ(sq, side);

				if (CHEBYSHEV_DIST(sq, to) + (Turn == cside) < CHEBYSHEV_DIST(KING_SQ(cside), to) + (Turn == side))
					EndGame[side]	+= PAWN_PASSED_UNSTOPPABLE_END / last_rank_dist;
			}

			// Determina que tan peligroso es un peón pasado, si puede avansar libremente o esta bloqueado
			for (to = sq + front, count = 3; count; to += front, count--)		{
				if (A1 <= to && to <= H8)		{
					if (Board[to])				{
						IsPawnPassedBloqued(side, to, sq);
						break;
					}

					if ((support_attack || EV_ATTACKED(to, cside)) && !(EV_ATTACKED(to, side) || support))
						break;
				}

				tmp		+= count;
			}

			EndGame[side]	+= (PAWN_PASSED_DANGEROUS_END * tmp) / last_rank_dist;
		}
	}
}


// Evalua PEONES bloqueados en C2, D2, E2
void	PawnBlocked()			{
	TURN		side;
	int			view;

	TURN_LOOP(side)				{
		view	= side ? 56 : 0;

		if (MATCH(PAWN | side, C7 ^ view) && MATCH(KNIGHT | side, C6 ^ view) && MATCH(PAWN | side, D5 ^ view) && !MATCH(PAWN | side, E5 ^ view))
			MidGame[side]	+= PAWN_C2_BLOCKED_MID;

		if (MATCH(PAWN | side, E7 ^ view) && MATCH(BISHOP | side, F8 ^ view) && NO_EMPTY(E6 ^ view))
			MidGame[side]	+= PAWN_CENTRAL_BLOCKED_MID;

		if (MATCH(PAWN | side, D7 ^ view) && MATCH(BISHOP | side, C8 ^ view) && NO_EMPTY(D6 ^ view))
			MidGame[side]	+= PAWN_CENTRAL_BLOCKED_MID;
	}
}


// Evalua la estructura de PEONES
void	PawnEval()				{
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, tmp_bb,
				pawns_passed_BB		= 0;
	int			mid_game			= 0,
				end_game			= 0,
				black_casttle		= 0,
				white_casttle		= 0,
				mid_game_pawn[2]	= {0, 0},
				end_game_pawn[2]	= {0, 0},
				view, center_file_dist, last_rank_dist, tmp1, tmp2,
				doubled, isolated, opened, backward, passed, candidate, advanced;

	// Evaluando caracteristicas de PEONES, si no estan en la hash de los PEONES
	if ((Engine & LEARNING) || ProbePawnScore(&mid_game, &end_game, &black_casttle, &white_casttle, PawnHash, &pawns_passed_BB) == -MATE)	{
		TURN_LOOP(side)			{
			view	= side ? 56 : 0;
			cside	= FLIP_TURN(side);

			for (bb = PAWN_BB(side); bb; bb ^= SQ_BB(sq))		{
				sq				= LSB_BB(bb);
				sq_rel			= sq ^ view;
				center_file_dist= MANHATTAN_DIST_CENTER_FILE(sq);
				last_rank_dist	= LAST_RANK_DIST(sq_rel);
				doubled			= (ForwardBB[side][sq] & PAWN_BB(side)) != 0;
				isolated		= !(FileBB[sq] & PawnAttBB[side]);
				opened			= !IN_FRONT_BB(sq, side, PAWN_BB(cside));
				passed			= !PAWNS_IN_FRONT_BB(sq, cside);
				backward		= !(passed || isolated) && PawnBackward(sq);
				advanced		= !(passed || isolated || backward) && PawnAdvanced(sq);
				candidate		= !(backward || isolated || passed || advanced) && opened && PawnCandidate(sq, side);

				// Material y PST
				mid_game_pawn[side]	+= PAWN_DIST_LAST_RANK_MID * last_rank_dist + PAWN_DIST_CENTER_FILE_MID * center_file_dist;
				end_game_pawn[side]	+= PAWN_DIST_LAST_RANK_END * last_rank_dist + PAWN_DIST_CENTER_FILE_END * center_file_dist;

				// Penalizando a PEONES centrales en sus casilla inicial
				if (sq_rel == D7 || sq_rel == E7)
					mid_game_pawn[side]	+= PAWN_CENTRAL_DEFAULT_POS_MID;

				// Bonificando a cadenas de PEONES
				if (FILE(sq) != FILE_A && (FileBB[sq - 1] & PAWN_BB(side)))		{
					mid_game_pawn[side]	+= PAWN_UNITED_MID;
					end_game_pawn[side]	+= PAWN_UNITED_END;
				}

				// Penalizando a PEONES doblados, incluye menor cantidad de PEONES enemigos deteniendolo
				if (doubled)	{
					mid_game_pawn[side]	+= PAWN_DOUBLED_MID + PAWN_DOUBLED_CENTER_FILE_MID * center_file_dist + PAWN_DOUBLED_DIST_LAST_RANK_MID * last_rank_dist;
					end_game_pawn[side]	+= PAWN_DOUBLED_END + PAWN_DOUBLED_CENTER_FILE_END * center_file_dist + PAWN_DOUBLED_DIST_LAST_RANK_END * last_rank_dist;

					// PERMANENCIA
					tmp1	= (FILE(sq) == FILE_A) ? 1 : (OPEN_FILE(sq - 1, side) == OPEN_FILE(sq - 1, cside));
					tmp2	= (FILE(sq) == FILE_H) ? 1 : (OPEN_FILE(sq + 1, side) == OPEN_FILE(sq + 1, cside));

					if ((tmp1 && tmp2) || isolated || backward)	{
						mid_game_pawn[side]	+= PAWN_DOUBLED_PERMANENCY_MID;
						end_game_pawn[side]	+= PAWN_DOUBLED_PERMANENCY_END;

						// ECONOMIA DE RECURSOS
						tmp_bb	= FileBB[sq] | ((FILE(sq) != FILE_A) ? FileBB[sq - 1] : 0) | ((FILE(sq) != FILE_H) ? FileBB[sq + 1] : 0);
						if (PopCountBB(tmp_bb & PAWN_BB(side)) > PopCountBB(tmp_bb & PAWN_BB(cside)))	{
							mid_game_pawn[side]	+= PAWN_DOUBLED_RESOURCE_ECONOMY_MID;
							end_game_pawn[side]	+= PAWN_DOUBLED_RESOURCE_ECONOMY_END;
						}
					}
				}

				// Penalizando a peones PEONES
				if (isolated)	{
					mid_game_pawn[side]	+= (PAWN_ISOLATED_MID + PAWN_ISOLATED_DIST_CENTER_FILE_MID * center_file_dist + PAWN_ISOLATED_DIST_LAST_RANK_MID * last_rank_dist) * (opened + 1);
					end_game_pawn[side]	+=  PAWN_ISOLATED_END + PAWN_ISOLATED_DIST_CENTER_FILE_END * center_file_dist + PAWN_ISOLATED_DIST_LAST_RANK_END * last_rank_dist;
				}

				// Penalizando a PEONES retrasados
				if (backward)	{
					tmp_bb	= RANK_2_BB | RANK_3_BB | RANK_7_BB | RANK_6_BB;
					mid_game_pawn[side]	+= (PAWN_BACKWARD_MID + PAWN_BACKWARD_DIST_CENTER_FILE_MID * center_file_dist + PAWN_BACKWARD_DIST_LAST_RANK_MID * last_rank_dist) * (opened + 1);
					end_game_pawn[side]	+=  PAWN_BACKWARD_END + PAWN_BACKWARD_DIST_CENTER_FILE_END * center_file_dist + PAWN_BACKWARD_DIST_LAST_RANK_END * last_rank_dist;

					// PERMANENCIA
					if ((SQ_BB(sq) & tmp_bb))	{
						tmp_bb	= PawnPassedBB[side][sq - Front[side]];

						mid_game_pawn[side]	+= PAWN_BACKWARD_PERMANENCY_MID;
						end_game_pawn[side]	+= PAWN_BACKWARD_PERMANENCY_END;

						// ECONOMIA DE RECURSOS
						if (!(ForwardBB[side][sq] & PAWN_BB(cside)) && PopCountBB(tmp_bb & PAWN_BB(side)) > PopCountBB(tmp_bb & PAWN_BB(cside)))	{
							mid_game_pawn[side]	+= PAWN_BACKWARD_RESOURCE_ECONOMY_MID;
							end_game_pawn[side]	+= PAWN_BACKWARD_RESOURCE_ECONOMY_END;
						}
					}
				}

				// Penalizando a PEONES avanzados
				if (advanced)	{
					mid_game_pawn[side]	+= (PAWN_ADVANCED_MID + PAWN_ADVANCED_DIST_CENTER_FILE_MID * center_file_dist + PAWN_ADVANCED_DIST_LAST_RANK_MID * last_rank_dist) * (opened + 1);
					end_game_pawn[side]	+=  PAWN_ADVANCED_END + PAWN_ADVANCED_DIST_CENTER_FILE_END * center_file_dist + PAWN_ADVANCED_DIST_LAST_RANK_END * last_rank_dist;
				}

				// Bonificando PEONES semi-pasados
				if (candidate)	{
					mid_game_pawn[side]	+= PAWN_CANDIDATE_MID + PAWN_CANDIDATE_DIST_LAST_RANK_MID * last_rank_dist + PAWN_CANDIDATE_CENTER_FILE_MID * center_file_dist;
					end_game_pawn[side]	+= PAWN_CANDIDATE_END + PAWN_CANDIDATE_DIST_LAST_RANK_END * last_rank_dist + PAWN_CANDIDATE_CENTER_FILE_END * center_file_dist;
				}

				// Guardando posicion de PEON pasado
				if (passed)
					pawns_passed_BB	|= SQ_BB(sq);
			}
		}

		mid_game		= mid_game_pawn[WHITE] - mid_game_pawn[BLACK];
		end_game		= end_game_pawn[WHITE] - end_game_pawn[BLACK];

		black_casttle	= KingShield(BLACK);
		white_casttle	= KingShield(WHITE);

		// Guardando en hash de PEONES la evaluacion
		if (!(Engine & LEARNING))
			StorePawnScore(mid_game, end_game, black_casttle, white_casttle, PawnHash, pawns_passed_BB);
	}

	// Asignando valores de las tablas hash
	MidGame[WHITE]		+= mid_game;
	EndGame[WHITE]		+= end_game;

	KingSafety[BLACK]	+= black_casttle;
	KingSafety[WHITE]	+= white_casttle;

	// Evaluando caracteristicas de PEONES no almacenadas en hash
	PawnBlocked();
	PawnPassedEval(pawns_passed_BB);
}


// Evalúa los CABALLOS
void	KnightEval()			{
	int			center_dist, last_rank_dist, view, pawn_attack, mob1, mob2, tmp;
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, mob_bb, bb2, tmp_BB;

	TURN_LOOP(side)
		if ((bb = KNIGHT_BB(side)))				{
			view	= side ? 56 : 0;
			cside	= FLIP_TURN(side);
			bb2		= MOBILITY_TARGET(cside);

			for (; bb; bb ^= SQ_BB(sq))			{
				sq				= LSB_BB(bb);
				sq_rel			= sq ^ view;
				last_rank_dist	= LAST_RANK_DIST(sq_rel);
				center_dist		= MANHATTAN_DIST_CENTER(sq);
				pawn_attack		= PopCountBB(PAWN_ATTACK_BB(sq, side));
				mob_bb			= KNIGHT_MOVES_BB(sq);
				mob1			= PopCountBB(mob_bb & bb2 & IMP4);
				mob2			= PopCountBB(mob_bb & bb2 & IMP2);

				// Agregando ataques de CABALLO a la tabla de ataques
				PutAttack(side, mob_bb, KNIGHT_ATTACK_KING_MID, KNIGHT_ATTACK_KING_END, 2);

				// Material y PST
				MidGame[side]	+= KNIGHT_MOB1_MID * mob1 + KNIGHT_MOB2_MID * mob2 + KNIGHT_DIST_CENTER_MID * center_dist + KNIGHT_DIST_LAST_RANK_MID * last_rank_dist;
				EndGame[side]	+= KNIGHT_MOB1_END * mob1 + KNIGHT_MOB2_END * mob2 + KNIGHT_DIST_CENTER_END * center_dist + KNIGHT_DIST_LAST_RANK_END * last_rank_dist;

				// Bonus, acercandose al REY enemigo
				tmp				= MANHATTAN_DIST(sq, KING_SQ(cside));
				MidGame[side]	+= KNIGHT_KING_TROPISM_MID * tmp;
				EndGame[side]	+= KNIGHT_KING_TROPISM_END * tmp;

				// atacando con CABALLO a PEONES y PIEZAS debiles
				tmp_BB			= mob_bb & (~PawnAttBB[cside]) & PieceBB[cside];
				if (tmp_BB)		{
					tmp			= PopCountBB(tmp_BB);
					if (tmp_BB & PAWN_BB(cside))	{
						MidGame[side]	+= KNIGHT_ATTACK_WEAK_PAWN_MID * tmp;
						EndGame[side]	+= KNIGHT_ATTACK_WEAK_PAWN_END * tmp;
					}
					else 	{
						MidGame[side]	+= KNIGHT_ATTACK_WEAK_PIECE_MID * tmp;
						EndGame[side]	+= KNIGHT_ATTACK_WEAK_PIECE_END * tmp;
					}
				}

				// Desarrollo lento
				if (sq_rel == B8 || sq_rel == G8)
					MidGame[side]		+= KNIGHT_SLOW_DEVELOPMENT_MID;

				// Defendiendo a CABALLO con PEONES
				if (pawn_attack){
					// Apostado, no es posible atacarlo con PEONES, está defendido por PEONES y en casillas centrales
					if ((KNIGHT_OUTPOST_BB & SQ_BB(sq_rel)) && IS_OUTPOST(side, sq))
						switch (sq_rel)			{
							case D5: case D4: case E5: case E4:						MidGame[side]	+= KNIGHT_OUTPOST1_MID * pawn_attack;	break;
							case C5: case C4: case F5: case F4: case D3: case E3:	MidGame[side]	+= KNIGHT_OUTPOST2_MID * pawn_attack;	break;
							case C3: case F3:										MidGame[side]	+= KNIGHT_OUTPOST3_MID * pawn_attack;	break;
							case B5: case B4: case G5: case G4:						MidGame[side]	+= KNIGHT_OUTPOST4_MID * pawn_attack;	break;
							default:;
						}

					else 	{
						MidGame[side]	+= KNIGHT_PROTECTED_BY_PAWN_MID;
						EndGame[side]	+= KNIGHT_PROTECTED_BY_PAWN_END;
					}
				}

				// CABALLO atrapado
				if (KNIGHT_TRAPPED(mob_bb & bb2))
					switch (sq_rel)				{
						case A1: case H1:	MidEndGame[side]	+= KNIGHT_TRAPPED_A1;	break;
						case A2: case H2:	MidEndGame[side]	+= KNIGHT_TRAPPED_A2;	break;
						default:;
					}
			}
		}
}


// Evalúa los ALFILES
void	BishopEval()			{
	int			center_dist, last_rank_dist, view, two_bishop, tmp,
				mob1, mob2, mob3, bad_bishop1, bad_bishop2, bad_bishop3, bad_bishop4;
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, mob_bb, bb2, tmp_BB;

	TURN_LOOP(side)
		if ((bb = BISHOP_BB(side)))				{
			cside			= FLIP_TURN(side);
			view			= side ? 56 : 0;
			two_bishop		= (BISHOP_BB(side) & WHITE_SQUARES_BB) && (BISHOP_BB(side) & BLACK_SQUARES_BB);
			bb2				= MOBILITY_TARGET(cside);

			for (; bb; bb ^= SQ_BB(sq))			{
				sq				= LSB_BB(bb);
				sq_rel			= sq ^ view;
				last_rank_dist	= LAST_RANK_DIST(sq_rel);
				center_dist		= MANHATTAN_DIST_CENTER(sq);
				mob_bb			= BISHOP_MOVES_BB(sq, ALL_PIECES_BB);
				mob1			= PopCountBB(mob_bb & bb2 & IMP4);
				mob2			= PopCountBB(mob_bb & bb2 & IMP2);
				mob3			= PopCountBB(mob_bb & bb2 & IMP1);

				// Agregando ataques de ALFILES a la tabla de ataques
				PutAttack(side, mob_bb, BISHOP_ATTACK_KING_MID, BISHOP_ATTACK_KING_END, 2);

				// Material y PST
				MidGame[side]	+= BISHOP_MOB1_MID * mob1 + BISHOP_MOB2_MID * mob2 + BISHOP_MOB3_MID * mob3 + BISHOP_DIST_CENTER_MID * center_dist + BISHOP_DIST_LAST_RANK_MID * last_rank_dist;
				EndGame[side]	+= BISHOP_MOB1_END * mob1 + BISHOP_MOB2_END * mob2 + BISHOP_MOB3_END * mob3 + BISHOP_DIST_CENTER_END * center_dist + BISHOP_DIST_LAST_RANK_END * last_rank_dist;

				// Bonus, acercandose al REY enemigo
				tmp				= MANHATTAN_DIST_BISHOP(sq, KING_SQ(cside));
				MidGame[side]	+= BISHOP_KING_TROPISM_MID * tmp;
				EndGame[side]	+= BISHOP_KING_TROPISM_END * tmp;

				// atacando con ALFIL a PEONES y PIEZAS debiles
				tmp_BB			= mob_bb & (~PawnAttBB[cside]) & PieceBB[cside];
				if (tmp_BB)		{
					tmp			= PopCountBB(tmp_BB);
					if (tmp_BB & PAWN_BB(cside))	{
						MidGame[side]	+= BISHOP_ATTACK_WEAK_PAWN_MID * tmp;
						EndGame[side]	+= BISHOP_ATTACK_WEAK_PAWN_END * tmp;
					}
					else 	{
						MidGame[side]	+= BISHOP_ATTACK_WEAK_PIECE_MID * tmp;
						EndGame[side]	+= BISHOP_ATTACK_WEAK_PIECE_END * tmp;
					}
				}

				// Desarrollo lento
				if (sq_rel == C8 || sq_rel == F8)
					MidGame[side]	+= BISHOP_SLOW_DEVELOPMENT_MID;

				// Defendiendo a ALFILES con PEONES
				if (PAWN_ATTACK_BB(sq, side))
					MidGame[side]	+= BISHOP_PROTECTED_BY_PAWN_MID;

				// ALFIL atrapado
				if (BISHOP_TRAPPED(mob_bb & bb2))
					switch (sq_rel)				{
						case A2: case H2: case B1:case G1:										MidEndGame[side]	+= BISHOP_TRAPPED_A2;	break;
						case A3: case H3: case B2:case G2: case A1:case H1: case C1:case F1:	MidEndGame[side]	+= BISHOP_TRAPPED_A3;	break;
						default:;
					}

				// ALFIL malo
				if (!two_bishop && BAD_BISHOP(mob_bb & bb2))	{
					mob_bb			= PAWN_BB(side) & SquareColorBB[sq];
					bad_bishop1		= PopCountBB(mob_bb & BAD_B5);
					bad_bishop2		= PopCountBB(mob_bb & BAD_B3);
					bad_bishop3		= PopCountBB(mob_bb & BAD_B2);
					bad_bishop4		= PopCountBB(mob_bb & BAD_B1);

					MidGame[side]	+= (BAD_BISHOP1_MID * bad_bishop1 + BAD_BISHOP2_MID * bad_bishop2 + BAD_BISHOP3_MID * bad_bishop3 + BAD_BISHOP4_MID * bad_bishop4) / 4;
					EndGame[side]	+= (BAD_BISHOP1_END * bad_bishop1 + BAD_BISHOP2_END * bad_bishop2 + BAD_BISHOP3_END * bad_bishop3 + BAD_BISHOP4_END * bad_bishop4) / 4;
				}
			}
		}
}


// Evalúa las TORRES
void	RookEval()				{
	int			center_file_dist, last_rank_dist, view, tmp,
				mob1, mob2;
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, bb2, mob_bb, tmp_BB;

	TURN_LOOP(side)
		if ((bb = ROOK_BB(side)))				{
			cside		= FLIP_TURN(side);
			view		= side ? 56 : 0;
			bb2			= MOBILITY_TARGET(cside);

			for (; bb; bb ^= SQ_BB(sq))			{
				sq				= LSB_BB(bb);
				sq_rel			= sq ^ view;
				center_file_dist= MANHATTAN_DIST_CENTER_FILE(sq);
				last_rank_dist	= LAST_RANK_DIST(sq_rel);
				mob_bb			= ROOK_MOVES_BB(sq, ALL_PIECES_BB);
				mob1			= PopCountBB(mob_bb & FileBB[sq] & bb2);
				mob2			= PopCountBB(mob_bb & RankBB[sq] & bb2);

				// Agregando ataques de TORRES a la tabla de ataques
				PutAttack(side, mob_bb, ROOK_ATTACK_KING_MID, ROOK_ATTACK_KING_END, 3);

				// Material y PST
				MidGame[side]	+= ROOK_MOB1_MID * mob1 + ROOK_MOB2_MID * mob2 + ROOK_DIST_CENTER_FILE_MID * center_file_dist + ROOK_DIST_LAST_RANK_MID * last_rank_dist;
				EndGame[side]	+= ROOK_MOB1_END * mob1 + ROOK_MOB2_END * mob2 + ROOK_DIST_CENTER_FILE_END * center_file_dist + ROOK_DIST_LAST_RANK_END * last_rank_dist;

				// Bonus, acercandose al REY enemigo
				tmp				= MANHATTAN_DIST(sq, KING_SQ(cside));
				MidGame[side]	+= ROOK_KING_TROPISM_MID * tmp;
				EndGame[side]	+= ROOK_KING_TROPISM_END * tmp;

				// atacando con TORRE a PEONES y PIEZAS debiles
				tmp_BB			= mob_bb & (~PawnAttBB[cside]) & PieceBB[cside];
				if (tmp_BB)		{
					tmp			= PopCountBB(tmp_BB);
					if (tmp_BB & PAWN_BB(cside))	{
						MidGame[side]	+= ROOK_ATTACK_WEAK_PAWN_MID * tmp;
						EndGame[side]	+= ROOK_ATTACK_WEAK_PAWN_END * tmp;
					}
					else 	{
						MidGame[side]	+= ROOK_ATTACK_WEAK_PIECE_MID * tmp;
						EndGame[side]	+= ROOK_ATTACK_WEAK_PIECE_END * tmp;
					}
				}

				switch (GET_FILE_STATE(sq, side))				{
					case FILE_HALF_OPEN:
						// TORRE en columna semi-abierta
						MidGame[side]		+= ROOK_IN_HALF_OPEN_FILE_MID;
						EndGame[side]		+= ROOK_IN_HALF_OPEN_FILE_END;

						// Atacando al REY enemigo
						if (abs(FILE(KING_SQ(cside)) - FILE(sq)) < 2)	{
							AttackWeightMid[side]	+= ROOK_ATTACK_KING_HALF_OPEN_FILE_MID;
							AttackWeightEnd[side]	+= ROOK_ATTACK_KING_HALF_OPEN_FILE_END;
						}
						break;
					case FILE_OPEN:
						// TORRE en columna abierta
						MidGame[side]		+= ROOK_IN_OPEN_FILE_MID;

						// Atacando al REY enemigo
						if (abs(FILE(KING_SQ(cside)) - FILE(sq)) < 2)	{
							AttackWeightMid[side]	+= ROOK_ATTACK_KING_OPEN_FILE_MID;
							AttackWeightEnd[side]	+= ROOK_ATTACK_KING_OPEN_FILE_END;
						}

						// TORRES y DAMAS dobladas en columna abierta
						if (mob_bb & FileBB[sq] & ROOK_BB(side))		{
							MidGame[side]		+= ROOK_ROOK_SUPPORT_MID;
							EndGame[side]		+= ROOK_ROOK_SUPPORT_END;
						}

						if (mob_bb & FileBB[sq] & QUEEN_BB(side))		{
							MidGame[side]		+= ROOK_QUEEN_SUPPORT_MID;
							EndGame[side]		+= ROOK_QUEEN_SUPPORT_END;
						}
						break;
					default:;
				}

				// Atrapada, en esquina del tablero y PEONES amigos bloqueando las salidas. Idea de [CPW]
				if (ROOK_TRAPPED(mob_bb & bb2, sq))				{
					switch (KING_SQ(side) ^ view)				{
						case F8: case G8:	tmp		= (CHEBYSHEV_DIST(sq_rel, H8) < 2)    * (BK | WK);	break;
						case C8: case B8:	tmp		= (CHEBYSHEV_DIST(sq_rel, A8) < 2)    * (BQ | WQ);	break;
						case H8: case G7:	tmp		= (sq == KING_SQ(side) + Front[side]) * (BK | WK);	break;
						case A8: case B7:	tmp		= (sq == KING_SQ(side) + Front[side]) * (BQ | WQ);	break;
						default: tmp = 0;
					}

					if (tmp)	{
						MidGame[side]		+= ROOK_TRAPPED_MID;
						EndGame[side]		+= ROOK_TRAPPED_END;
						RookTrapped[side]	|= tmp;
					}
				}

				// TORRE en 7ma fila si PEONES enemigos en 7ma o REY enemigo en 8va
				if (IN_7th(sq_rel, side))		{
					MidGame[side]	+= ROOK_7_MID;
					EndGame[side]	+= ROOK_7_END;
				}
			}
		}
}


// Evalúa las DAMAS
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
				last_rank_dist	= LAST_RANK_DIST(sq_rel);
				center_dist		= MANHATTAN_DIST_CENTER(sq);
				mob_bb			= QUEEN_MOVES_BB(sq, ALL_PIECES_BB);
				mob				= PopCountBB(mob_bb & ~ALL_PIECES_BB);

				// Agregando ataques de DAMAS a la tabla de ataques
				PutAttack(side, mob_bb, QUEEN_ATTACK_KING_MID, QUEEN_ATTACK_KING_END, 4);

				// Material y PST
				MidGame[side]	+= QUEEN_MOB_MID * mob + QUEEN_DIST_CENTER_MID * center_dist + QUEEN_DIST_LAST_RANK_MID * last_rank_dist;
				EndGame[side]	+= QUEEN_MOB_END * mob + QUEEN_DIST_CENTER_END * center_dist + QUEEN_DIST_LAST_RANK_END * last_rank_dist;

				// Bonus, acercandose al REY enemigo
				MidGame[side]	+= QUEEN_KING_TROPISM_MID * MANHATTAN_DIST(sq, KING_SQ(cside));
				EndGame[side]	+= QUEEN_KING_TROPISM_END * MANHATTAN_DIST(sq, KING_SQ(cside));

				// DAMA en 7ma fila si PEONES enemigos en 7ma o REY enemigo en 8va
				if (IN_7th(sq_rel, side))		{
					MidGame[side]	+= QUEEN_7_MID;
					EndGame[side]	+= QUEEN_7_END;
				}
			}
		}
}


// Evalúa los REYES
void	KingEval()				{
	SQUARE		sq;
	TURN		side, cside;
	BITBOARD	mob_bb, bb, bb2, king_front_bb;
	int			last_rank_dist, center_dist, view, king_attack_mid, king_attack_end;

	TURN_LOOP(side)				{
		cside			= FLIP_TURN(side);
		view			= side ? 56 : 0;
		sq				= KING_SQ(side);
		last_rank_dist	= LAST_RANK_DIST(sq ^ view);
		center_dist		= MANHATTAN_DIST_CENTER(sq);
		mob_bb			= KING_MOVES_BB(sq);

		// PST
		MidGame[side]	+= KING_DIST_CASTTLE_MID * MANHATTAN_DIST_CASTTLE(sq ^ view) +  KING_DIST_LAST_RANK_MID * last_rank_dist + ExtraShield(FILE(sq), side);
		EndGame[side]	+= KING_DIST_CENTER_END * center_dist + KING_TROPISM_WEAK_PAWN_END * PopCountBB(mob_bb & ~PawnAttBB[cside] & PAWN_BB(cside)) + KING_DIST_LAST_RANK_END * last_rank_dist;

		/*
		 * Ataque al REY
		 * Mezcla de la seguridad del REY de [Rebel], [Stockfish] y otras ideas.
		 * Tiene en cuenta la cantidad de ataques de PIEZAS y su tipo de piezas a casillas cercanas al REY [Stockfish].
		 * Tambien analiza si esas casillas estan defendidas o ocupadas por PIEZAS del mismo bando o del
		 * enemigo o si no hay PEONES en esas casillas [Rebel].
		 */
		if (AttackCount[cside] > 4 && PieceMat[cside] > QUEEN_VALUE + BISHOP_VALUE)			{
			bb2			= EV_ATTACKED_BB(cside);				// casillas atacadas por el enemigo
			king_front_bb= KingFrontBB[side][sq];				// zona de enfrente del REY
			bb			= king_front_bb & bb2 & ~PAWN_BB(side);	// zona de enfrente del REY sin PEONES y atacada por el enemigo

			king_attack_mid	= AttackWeightMid[cside]															// Penaliza, Zona de REY Atacada(segun atacante) o Rey en centro de tablero
						+ (PAWN_ATTACK_KING_MID * PopCountBB(KING_ZONE[side] & PawnAttBB[cside])) / 2			// Penaliza, ataques de peones al REY
						+ KING_ONLY_DEFEND_MID * PopCountBB(mob_bb & bb2 & ~EV_ATTACKED_WITHOUT_KING_BB(side))	// Penaliza, REY unico defensor de casillas atacadas
						+ KING_WITHOUT_FRIEND_MID * PopCountBB(king_front_bb & ~bb2 & ~PieceBB[side])			// Penaliza, casillas frente al REY no atacadas, pero vacia o PIEZA enemiga ocupandola
						+ KING_WITH_ENEMY_MID  * PopCountBB(bb & ~PieceBB[side]) 
						+ KING_WITH_ENEMY2_MID * PopCountBB(bb & PieceBB[side]);								// Penaliza, casillas frente a REY atacadas no ocupadas por PEON del mismo bando, duplicar si no ocupada por el mismo bando
			king_attack_end	= AttackWeightEnd[cside]															// Penaliza, Zona de REY Atacada(segun atacante) o Rey en centro de tablero
						+ (PAWN_ATTACK_KING_END * PopCountBB(KING_ZONE[side] & PawnAttBB[cside])) / 2;			// Penaliza, ataques de peones al REY

			MidGame[side]	-= SafetyTable[MAX(0, MIN(99, king_attack_mid))];
			EndGame[side]	-= SafetyTable[MAX(0, MIN(99, king_attack_end))];
		}
	}
}


// Algunos valores para el desvalance material segun Larry Kaufman
void	MaterialEval()			{
	TURN	side;
	int		mid_game, end_game, mat_diff,
			mid_game_mat[2]	= {0, 0},
			end_game_mat[2]	= {0, 0};

	if ((Engine & LEARNING) || ProbeMatScore(&mid_game, &end_game, (U32)HashMat) == -MATE)		{
		TURN_LOOP(side)			{
			// Incrementa valor de los CABALLOS en posiciones cerradas y decrementa en abiertas, lo contrario con las TORRES
			// Penaliza la existencia de pocos PEONES, mayor posibilidad de tablas
			mid_game_mat[side]	+= PAWNS(side) * (KNIGHT_ADJUST_MID * KNIGHTS(side) + ROOK_ADJUST_MID * ROOKS(side)) + PAWN_ADJUST_MID / (PAWNS(side) + 1);
			end_game_mat[side]	+= PAWNS(side) * (KNIGHT_ADJUST_END * KNIGHTS(side) + ROOK_ADJUST_END * ROOKS(side)) + PAWN_ADJUST_END / (PAWNS(side) + 1);

			// Penalización, redundancia de PIEZAS cuando se tiene mas de 1 CABALLO
			if (KNIGHTS(side) > 1)	{
				mid_game_mat[side]	+= KNIGHT_PAIR_MID;
				end_game_mat[side]	+= KNIGHT_PAIR_END;
			}

			// Penalización, redundancia de PIEZAS cuando se tiene mas de 1 TORRE
			if (ROOKS(side) > 1)	{
				mid_game_mat[side]	+= ROOK_PAIR_MID;
				end_game_mat[side]	+= ROOK_PAIR_END;
			}

			// Bonificando pareja de ALFILES
			if ((BISHOP_BB(side) & WHITE_SQUARES_BB) && (BISHOP_BB(side) & BLACK_SQUARES_BB))	{
				mid_game_mat[side]	+= BISHOP_PAIR_MID;
				end_game_mat[side]	+= BISHOP_PAIR_END;
			}

			// Bonificando el material de PIEZAS
			mid_game_mat[side]	+= PAWN_MAT_MID * PAWNS(side) + KNIGHT_MAT_MID * KNIGHTS(side) + BISHOP_MAT_MID * BISHOPS(side) + ROOK_MAT_MID * ROOKS(side) + QUEEN_MAT_MID * QUEENS(side);
			end_game_mat[side]	+= PAWN_MAT_END * PAWNS(side) + KNIGHT_MAT_END * KNIGHTS(side) + BISHOP_MAT_END * BISHOPS(side) + ROOK_MAT_END * ROOKS(side) + QUEEN_MAT_END * QUEENS(side);
		}

		mat_diff	= PieceMat[WHITE] - PieceMat[BLACK];
		side		= WHITE;

		if (mat_diff < 0)		{
			mat_diff	= -mat_diff;
			side		= BLACK;
		}

		// Mayor contra PEONES
		if (mat_diff >= ROOK_VALUE)				{
			mid_game_mat[side]	+= ROOK_VS_PAWN_MID;
			end_game_mat[side]	+= ROOK_VS_PAWN_END;
		}

		// Menor contra PEONES
		else if (mat_diff >= KNIGHT_VALUE)		{
			mid_game_mat[side]	+= MINOR_VS_PAWN_MID;
			end_game_mat[side]	+= MINOR_VS_PAWN_END;
		}

		// Menor contra TORRE
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

			// Dos menores contra TORRE
			if (mat_diff > 1)	{
				mid_game_mat[side]	+= TWO_MINOR_VS_ROOK_MID;
				end_game_mat[side]	+= TWO_MINOR_VS_ROOK_END;
			}
		}

		mid_game	= mid_game_mat[WHITE] - mid_game_mat[BLACK];
		end_game	= end_game_mat[WHITE] - end_game_mat[BLACK];

		// Guardando en hash de material la evaluacion
		if (!(Engine & LEARNING))
			StoreMatScore(mid_game, end_game, (U32)(HashMat));
	}

	// Asignando valores de las tablas hash
	MidGame[WHITE]	+= mid_game;
	EndGame[WHITE]	+= end_game;
}


/*
 * Evalua posicion sin peones y un bando sin material, ejecuta mates elementales simples.
 * Ver (Mop-up Evaluation) de <www.chessprogramming.org/Home/Evaluation/Game Phases/Endgame/Mop-up Evaluation>
 */
int		MopUpEv(TURN side)		{
	SQUARE		K			= KING_SQ(side),
				k			= KING_SQ(side ^ 1);
	int			CMD			= 14.0,
				MD			= 14.0 - MANHATTAN_DIST(k, K);
	BITBOARD	bb			= BLACK_BISHOP_BB | WHITE_BISHOP_BB,
				corner_bb	= 0;

	// Detectando esquina equivocada en caso de tener un solo alfil
	if (bb & BLACK_SQUARES_BB)
		corner_bb |= BLACK_SQUARES_BB;

	if (bb & WHITE_SQUARES_BB)
		corner_bb |= WHITE_SQUARES_BB;

	// Calculo de distancias: a ultimas filas(columnas) o esquinas(si un solo alfil en el tablero)
	if(corner_bb == BLACK_SQUARES_BB)
		CMD		-= MIN(MANHATTAN_DIST(k, A1), MANHATTAN_DIST(k, H8));

	else if (corner_bb == WHITE_SQUARES_BB)
		CMD		-= MIN(MANHATTAN_DIST(k, A8), MANHATTAN_DIST(k, H1));

	else CMD	-= MANHATTAN_DIST_CENTER(k);

	return 54 * CMD + 18 * MD;
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
	int		p;
	TURN	side,
			cturn	= FLIP_TURN(Turn);

	IniEvAttack();

	// Evaluando posicion
	KnightEval();
	BishopEval();
	RookEval();
	QueenEval();
	KingEval();
	PawnEval();
	MaterialEval();

	// Recolectando evaluacion de seguridad del REY de cada bando
	TURN_LOOP(side)	{
		p		= PopCountBB((PieceBB[side] ^ PAWN_BB(side) ^ KING_BB(side)) & EV_ATTACKED_BB(side));

		MidGame[side]		+= PROTECTED_PIECES_MID * p;
		EndGame[side]		+= PROTECTED_PIECES_END * p;
		MidEndGame[side]	+= (KingSafety[side] * PieceMat[FLIP_TURN(side)]) / INITIAL_MATERIAL;
	}

	// Recolectando evaluacion para cada fase del juego y adiciona el Tempo
	*MidGame		= MidGame[Turn]		- MidGame[cturn] + TEMPO_MID;
	*EndGame		= EndGame[Turn]		- EndGame[cturn];
	*MidEndGame		= MidEndGame[Turn]	- MidEndGame[cturn];

	// Interpolando la evaluación entre el mediojuego y final. No es necesario hacerlo si se esta en un final simple
	return *MidEndGame + TAPERED_SCORE(*MidGame, *EndGame);
}


// Evaluación de la posición con salida rápidas: lazy eval, tablas hash, final tablas
int		Eval(int alpha, int beta)				{
	int		score, lazy_margen, mat_diff, mat_sum;
	TURN	cturn;

	// Motor aprendiendo, ignoramos tablas hash, lazy eval y otros datos.
	if (Engine & LEARNING)
		return (16 * FullEval()) / SupportedEndGames();

	// La posición esta evaluada por la hash de evaluacion
	score	= ProbeScore(Hash);
	if (score != -MATE)
		return score;

	/*
	 * Si se tiene mucha ventaja(desventaja) no hace falta seguir evaluando, cuanto menor es 
	 * 'lazy_margen' más rápido sera el programa, con riesgo de dejar algún movimiento súper bueno
	 */
	cturn		= FLIP_TURN(Turn);
	lazy_margen	= TAPERED_SCORE(200, 800);
	mat_diff	= PieceMat[Turn] + PawnMat[Turn] - PieceMat[cturn] - PawnMat[cturn];
	if (mat_diff - lazy_margen >= beta)
		return beta;

	if (mat_diff + lazy_margen <= alpha)
		return alpha;

	// Evaluacion para mates elementales simples.
	mat_sum		= PieceMat[WHITE] + PawnMat[WHITE] + PieceMat[BLACK] + PawnMat[BLACK];
	if ((GAME_PHASE < 5) && !ALL_PAWNS && mat_sum >= ROOK_VALUE && abs(mat_diff) == mat_sum)
		score	= PieceMat[Turn] ? MopUpEv(Turn) : -MopUpEv(cturn);

	// Evaluacion y coreciones si el final esta soportado por el motor
	else score	= (16 * FullEval()) / SupportedEndGames();

	// Reducir evaluacion si se esta acerca de la regla de los 50 movimientos
	if (68 < Rule50 && Rule50 < 100)
		score	= (score * (100 - Rule50)) / 32;

	// Guardando y devolviendo evaluacion
	StoreScore(score, Hash);
	return score;
}


// Detecta tablas por insuficiencia material
int		MatInsuf()				{
	BITBOARD	bb	= BLACK_BISHOP_BB | WHITE_BISHOP_BB;

	switch ((U32)HashMat)		{
		case KK: case KNK: case KKN: case KNNK: case KKNN: case KKB: case KBK:	return 1;
		case KBBK: case KKBB: case KBKB: case KBBKB: case KBKBB: case KBBKBB:	return !((bb & BLACK_SQUARES_BB) && (bb & WHITE_SQUARES_BB));
		default: return 0;
	}
}


// Algunos finales soportados. El retorno por debajo de 16	es para posiciones tablas
int		SupportedEndGames()		{
	SQUARE		P, r, R, k, K, prom_sq, p;
	int			dist, all_pawns_in_file, P_file, K_file, k_file, r_file,
				mat_sum			= PieceMat[BLACK] + PieceMat[WHITE] + PawnMat[WHITE] + PawnMat[BLACK],
				mat_diff		= abs(PieceMat[BLACK] - PieceMat[WHITE]),
				pawn_diff		= abs(BLACK_PAWNS - WHITE_PAWNS);
	U32			MatHashPiece	= (U32)(HashMat & 0xFFFFFF00);
	BITBOARD	bb				= BLACK_PAWN_BB		| WHITE_PAWN_BB,
				bb2				= BLACK_BISHOP_BB	| WHITE_BISHOP_BB;
	TURN		side			= 2,
				cside, t;

	// Salida Rapida, no se tiene suficiente material
	if (GAME_PHASE > 102)
		return 16;

	// Se pone el punto de vista del tablero del bando que tenga PEONES, sino al que tenga ALFIL
	if (BLACK_PAWNS && !WHITE_PAWNS)
		side	= BLACK;

	else if (WHITE_PAWNS && !BLACK_PAWNS)
		side	= WHITE;

	else if (BLACK_BISHOP_BB)
		side	= BLACK;

	else if (WHITE_BISHOP_BB)
		side	= WHITE;

	// Calcula cual es el PEON mas avanzado del bando con PEONES
	for (P = LSB_BB(PAWN_BB(side)); ForwardBB[side][P] & PAWN_BB(side); )
		P = LSB_BB(ForwardBB[side][P] & PAWN_BB(side));

	cside				= FLIP_TURN(side);
	K					= KING_SQ(side);
	k					= KING_SQ(cside);
	prom_sq				= PROM_SQ(P, side);
	all_pawns_in_file	= (~FileBB[P] & bb) == 0;
	P_file				= FILE(P);
	K_file				= FILE(K);
	k_file				= FILE(k);


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

		// Lo importante para la defensa es no llevar el REY a la esquina del mismo color que el ALFIL
		case KRBKR: case KRKRB:
			if (((bb2 & BLACK_SQUARES_BB) && (KING_BB(cside) & BLACK_CORNER_BB)) || ((bb2 & WHITE_SQUARES_BB) && (KING_BB(cside) & WHITE_CORNER_BB)))
				return 16;
			return DRAW_SCORE(mat_sum);

		// Soporte para KPK o KKP
		case KPK: case KKP:
			// El PEON corona por si solo
			if (CHEBYSHEV_DIST(prom_sq, P) + (Turn == cside) < CHEBYSHEV_DIST(prom_sq, k) + (Turn == side))
				return 16;

			// REY enemigo captura al PEON
			if (CHEBYSHEV_DIST(K, P) + (Turn == cside) > CHEBYSHEV_DIST(P, k) + (Turn == side))
				return DRAW_SCORE(mat_sum);

			// PEON en columna A o H
			if ((bb & (FILE_A_BB | FILE_H_BB)) && (KING_BB(cside) & KingZoneBB[cside][prom_sq ^ 1]))
				return DRAW_SCORE(mat_sum);

			// REY enemigo frente al PEON
			if (CHEBYSHEV_DIST(K, P + Front[side]) + (Turn == cside) > CHEBYSHEV_DIST(P + Front[side], k) + (Turn == side))
				return DRAW_SCORE(mat_sum);

			if (K + 2 * Front[side] == k)		{
				// REY del bando fuerte al lado del PEON y el otro al frente del REY
				if (P + 1 == K || P - 1 == K)
					return DRAW_SCORE(mat_sum);

				// REY del bando fuerte al frente del PEON y el otro al frente del REY
				if ((P + Front[side] == K) && (Turn == side))
					return DRAW_SCORE(mat_sum);
			}
			break;

		// Soporte para KPK, KKP con todos los PEONES en columna A o H
		case KPPK: case KKPP: case KPPPK: case KKPPP: case KPPPPK: case KKPPPP:
			if (all_pawns_in_file && (bb & (FILE_A_BB | FILE_H_BB)) && (KING_BB(cside) & KingZoneBB[cside][prom_sq ^ 1]))
				return DRAW_SCORE(mat_sum);
			break;

		// Soporte para KBPK, KKBP con todos los PEONES en columna A o H
		case KBPK: case KBPPK: case KBPPPK: case KBPKP: case KBPPKPP: case KBPKPP: case KBPPKP: case KBPPPKPP: case KBPPPKP: case KBPPKPPP: case KBPKPPP: 
		case KKBP: case KKBPP: case KKBPPP: case KPKBP: case KPPKBPP: case KPPKBP: case KPKBPP: case KPPKBPPP: case KPKBPPP: case KPPPKBPP: case KPPPKBP:
			if (all_pawns_in_file && (bb & (FILE_A_BB | FILE_H_BB)) && !(bb2 & SquareColorBB[prom_sq]) && (KING_BB(cside) & KingZoneBB[cside][prom_sq ^ 1]))
				return DRAW_SCORE(mat_sum);
			break;

		// Soporte para KQKP, KPKQ, con todos los PEONES en columna A, C, F o H
		case KQKP: case KPKQ: case KQKPP: case KPPKQ: case KQKPPP: case KPPPKQ:
			if (all_pawns_in_file && MATCH(PAWN | side, prom_sq - Front[side]))	{
				if(((FILE_A_BB & bb) && (KING_BB(cside) & KQKP_DB1[cside])) || ((FILE_H_BB & bb) && (KING_BB(cside) & KQKP_DB2[cside]))
				|| ((FILE_C_BB & bb) && (KING_BB(cside) & KQKP_DB3[cside])) || ((FILE_F_BB & bb) && (KING_BB(cside) & KQKP_DB4[cside])))
					return DRAW_SCORE(mat_sum);
			}
			return 16;

		// Lo importante es no llevar REY débil a esquina de color contrario al ALFIL, para poder defenderse
		case KRKB: case KBKR:
			if(((bb2 & BLACK_SQUARES_BB) && (KING_BB(side) & ARRIM_WHITE_CORNER_BB)) || ((bb2 & WHITE_SQUARES_BB) && (KING_BB(side) & ARRIM_BLACK_CORNER_BB)))
				return DRAW_SCORE(mat_sum);
			break;

		/*
		 * 1) Controlar casilla por delante del peón de color distinto al ALFIL con PIEZA(no REY)
		 * 2) Ocupar casilla por delante del peón de color distinto al ALFIL
		 */
		case KNKBP: case KBKBP: case KBPKN: case KBPKB:
			bb	= ForwardBB[side][P] & ((bb2 & BLACK_SQUARES_BB) ? WHITE_SQUARES_BB : BLACK_SQUARES_BB);
			if (bb & (PieceBB[cside] | EV_ATTACKED_WITHOUT_KING_BB(cside)))
				return DRAW_SCORE(mat_sum);
			break;

		case KRKP: case KPKR: case KRKPP: case KPPKR:
			if (all_pawns_in_file)				{
				r		= LSB_BB(ROOK_BB(cside));
				r_file	= FILE(r);

				// REY enemigo en frente del PEON o el REY no esta cerca para apoyar el PEON
				if (IN_FRONT_BB(P, side, KING_BB(cside)) || (CHEBYSHEV_DIST(P, prom_sq) > 1 && CHEBYSHEV_DIST(K, P) > 2))
					return 16;

				// PEON en 7ma apoyado por su REY
				if (CHEBYSHEV_DIST(P, prom_sq) < 2 && CHEBYSHEV_DIST(K, P) < 2)
					return DRAW_SCORE(mat_sum);

				// TORRE cortando al REY enemigo en 5ta
				if (abs(RANK(prom_sq) - RANK(r)) > 3 && abs(RANK(prom_sq) - RANK(r)) <= abs(RANK(K) - RANK(r)))
					return 16;

				// Equivalente al soporte de [Fruit] al final KRKP
				if ((Turn == cside) && CHEBYSHEV_DIST(K, P) == 2 && abs(RANK(K) - RANK(P)) < 2 && r_file != (K_file + P_file) / 2)
					return 16;

				dist	= CHEBYSHEV_DIST(K, prom_sq) + CHEBYSHEV_DIST(P, prom_sq) + (K == prom_sq) - (r_file != P_file && RANK(r) != RANK(prom_sq));
				if (P + Front[side] == K)		{
					if (FileBB[P] & (FILE_A_BB | FILE_H_BB))
						break;

					dist++;
				}

				if (CHEBYSHEV_DIST(k, prom_sq) + (Turn == side) >= dist + (Turn == cside))
					return DRAW_SCORE(mat_sum);
			}
			return 16;

		case KRKRP:	case KRPKR:
			R		= LSB_BB(ROOK_BB(side));
			r		= LSB_BB(ROOK_BB(cside));
			r_file	= FILE(r);

			// Defensa de ultima fila
			if (IN_FRONT_BB(P, side, KING_BB(cside)) && (CHEBYSHEV_DIST(P, prom_sq) < 3) && (PAWN_BB(side) & ROOK_KNIGHT_FILE_BB) && (RankBB[prom_sq] & RankBB[k] & RankBB[r]))
				return DRAW_SCORE(mat_sum);

			// PEON en columna TORRE
			if (FileBB[P] & (FILE_A_BB | FILE_H_BB))		{
				// PEON en 7ma
				if (P + Front[side] == prom_sq)	{
					// REY enemigo frente al PEON
					if (IN_FRONT_BB(P, side, KING_BB(cside)) && abs(P_file - r_file) == 1 && abs(P_file - k_file < 5))
						return DRAW_SCORE(mat_sum);

					// TORRE del mismo bando frente al PEON
					if (R == prom_sq && P_file == r_file && (CHEBYSHEV_DIST(k, R) < 3 || ((RankBB[k] & PAWN_BB(side)) && CHEBYSHEV_DIST(k, P) > 5)))
						return DRAW_SCORE(mat_sum);

					// REY enemigo frente al PEON
					if (IN_FRONT_BB(P, side, KING_BB(cside)))
						return DRAW_SCORE(mat_sum);
				}

				// Vancura posicion
				else if (RANK(r) == RANK(P) && MIN(CHEBYSHEV_DIST(k, prom_sq), CHEBYSHEV_DIST(k, prom_sq ^ 7)) < 2 && (ForwardBB[side][P] & ROOK_BB(side)))
					return DRAW_SCORE(mat_sum);
			}

			// Defensa del lado corto
			else if (abs(k_file - r_file) > abs(P_file - r_file) && CHEBYSHEV_DIST(K, P) == 1 && abs(P_file - r_file) > 3 && abs(P_file - k_file) < 3 && CHEBYSHEV_DIST(k, r) > 5)
				return DRAW_SCORE(mat_sum);

			// Philidor position
			if (PawnPassedBB[side][P] & KING_BB(cside))		{
				if (CHEBYSHEV_DIST(P, prom_sq) > 2)			{
					if (abs(RANK(prom_sq) - RANK(K)) > 2 && abs(RANK(prom_sq) - RANK(r)) == 2)
						return DRAW_SCORE(mat_sum);
				}
				else if (abs(RANK(prom_sq) - RANK(r)) > 5)
					return DRAW_SCORE(mat_sum);
			}
			break;
		default:;
	}

	// Regla general(no perfecta). Si ambos bandos no PEONES peones se necesita el equivalente a casi 4 PEONES para poder ganar.
	t	= PieceMat[BLACK] < PieceMat[WHITE] ? BLACK : WHITE;
	if (!ALL_PAWNS && mat_diff < 7 * PAWN_VALUE / 2 && !(KING_BB(t) & ARRIM_BB))
		return DRAW_SCORE(mat_sum);

	// Dificil ganar cuando se tiene material igual y pocos PEONES
	t	= (BLACK_PAWNS < WHITE_PAWNS) ? BLACK : WHITE;
	int draw_end = pawn_diff < 2 && ALL_PAWNS;
	if (!mat_diff && pawn_diff < 2)
		switch (ALL_PAWNS)		{
			// REY en desventaja frente del unico PEON
			case 1:
				if (IN_FRONT_BB(P, side, KING_BB(cside)))
					return DRAW_SCORE(mat_sum);

				// Los finales de DAMAS y TORRES son analizados aparte
				if (KNIGHTS(t) || BISHOPS(t))
					return DRAW_SCORE(mat_sum);
				break;

			// REY en desventaja cerca a su PEON y su enemigo sin peones pasados
			case 3:
				p	= LSB_BB(PAWN_BB(t));
				if (PopCountBB(PAWNS_IN_FRONT_BB(p, t ^ 1)) == 2 && CHEBYSHEV_DIST(KING_SQ(t), p) < 3)
					return DRAW_SCORE(mat_sum);
				break;
			case 5:
				for (bb = PAWN_BB(t); bb; bb ^= SQ_BB(p))		{
					p	= LSB_BB(bb);
					if (!PAWNS_IN_FRONT_BB(p, t ^ 1))
						break;
				}

				if (!bb && CHEBYSHEV_DIST(KING_SQ(t), p) < 3)
					return DRAW_SCORE(mat_sum);
				break;
			default:;
		}

	// Asegurar no haber muchos PEONES pasados
	switch (MatHashPiece)		{
		// Prefiere ir final de PEONES o de CABALLOS
		case KK:		return KK_END;
		case KNNKNN:	return KNNKNN_END;
		case KNKN:		return KNKN_END;

		case KBKB:
			// ALFILES de distinto color
			if ((bb2 & BLACK_SQUARES_BB) && (bb2 & WHITE_SQUARES_BB))
				return (pawn_diff < 4 && ALL_PAWNS) ? (KBKB_END / ALL_PAWNS) : 16;

			// ALFILES de igual color
			return (draw_end && ALL_PAWNS) ? (KBKB2_END / ALL_PAWNS) : 16;

		// Estos finales posiblemente progresen a un final tablas
		case KRKR:		return draw_end ? KRKR_END		: 16;
		case KQKQ:		return draw_end ? KQKQ_END		: 16;
		case KRBKRB:	return ((bb2 & BLACK_SQUARES_BB) && (bb2 & WHITE_SQUARES_BB)) ? KRBKRB_END : KRBKRB2_END;
		case KBNKBN:	return ((bb2 & BLACK_SQUARES_BB) && (bb2 & WHITE_SQUARES_BB)) ? KBNKBN_END : KBNKBN2_END;
		case KBBKBB:	return (draw_end && ALL_PAWNS) ? KBBKBB_END / ALL_PAWNS : 16;
		default:		return 16;
	}
}


//	1200	1250	1300	1350
