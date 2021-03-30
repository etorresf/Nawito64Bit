/*
 * < Nawito is a chess engine winboard derived of Danasah507>
 * Copyright (C) <2021> <Ernesto Torres>
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



int			GamePhase,
			Tropism[64][64],
			BishopTropism[64][64];

BITBOARD	PawnAttBB[2],
			PieceAttBB[2],
			KingAttBB[2],
			KING_ZONE[2];

int			Weight[16],

			*AttackWeight		= Weight + 0,
			*AttackCount		= Weight + 2,

			*PassedPawns		= Weight + 4,
			*IsolatedPawns		= Weight + 6,
			*MidGame			= Weight + 8,
			*EndGame			= Weight + 10;

int			EvWeight[64],

			*MaterialMidGame		= EvWeight + 0,
			*MaterialEndGame		= EvWeight + 2,

			*KingTropism			= EvWeight + 4,
			*KingShield				= EvWeight + 6,
			*KingAttack				= EvWeight + 8,
			*KingSafetyMidGame		= EvWeight + 10,
			*KingSafetyEndGame		= EvWeight + 12,

			*WeakPawnMidGame		= EvWeight + 14,
			*WeakPawnEndGame		= EvWeight + 16,
			*PassedPawnMidGame		= EvWeight + 18,
			*PassedPawnEndGame		= EvWeight + 20,

			*MobilityMidGame		= EvWeight + 22,
			*MobilityEndGame		= EvWeight + 24,

			*ActMidGame				= EvWeight + 26,
			*ActEndGame				= EvWeight + 28,

			*TrappedPiece			= EvWeight + 30;


// Evaluación de la posición con salida rápidas: lazy eval, tablas hash, final tablas
int		Eval(int alpha, int beta)				{
	int		score, mat_diff, draw;


	// Comprueba si la posición esta evaluada
	score = ProbeScore(Hash);
	if (score != -MATE)
		return score;


	/*
	 * Si se tiene mucha ventaja o desventaja no hace falta seguir evaluando, cuanto menor es el valor de
	 * LazyScore más rápido sera el programa, con riesgo de dejar algún movimiento súper bueno
	 */
	mat_diff = PieceMat[Turn] + PawnMat[Turn] - PieceMat[FLIP_TURN(Turn)] - PawnMat[FLIP_TURN(Turn)];
	if (mat_diff - LazyScore[MatSum >> 6] >= beta)
		return beta;

	if (mat_diff + LazyScore[MatSum >> 6] <= alpha)
		return alpha;


	score		= FullEval();

	// Ajusta evaluacion si final soportado por el motor
	draw		= SupportedEndGames();
	if (draw)
		score	= (16 * score) / draw;


	StoreScore(score, Hash);
	return score;
}


// Recolecta los datos de evaluacion
void	CollectDataEv(void)		{
	int		tmp;
	TURN	side,
			cturn	= FLIP_TURN(Turn);


	// Recolectando evaluacion de cada bando
	TURN_LOOP(side)				{
		tmp				= ((KingTropism[side] + KingShield[side]) * PieceMat[FLIP_TURN(side)]) / INITIAL_MATERIAL + TrappedPiece[side];

		ActMidGame[side]+= 2 * PopCountBB((AllPieceBB[side] ^ PAWN_BB(side) ^ KING_BB(side)) & EV_ATTACKED_BB(side));

		MidGame[side]	+= MaterialMidGame[side] - KingAttack[side]     + KingSafetyMidGame[side] + PassedPawnMidGame[side] - WeakPawnMidGame[side] + MobilityMidGame[side] + ActMidGame[side] + TempoMid - tmp;
		EndGame[side]	+= MaterialEndGame[side] - KingAttack[side] / 2 + KingSafetyEndGame[side] + PassedPawnEndGame[side] - WeakPawnEndGame[side] + MobilityEndGame[side] + ActEndGame[side] + TempoEnd - tmp;
	}


	// Recolectando evaluacion para cada fase del juego
	*MidGame	= MidGame[Turn] - MidGame[cturn];
	*EndGame	= EndGame[Turn] - EndGame[cturn];
}

// Evaluación completa de la posicion
int		FullEval(void)			{
	GamePhase			= MIN(2 * INITIAL_MATERIAL, PieceMat[BLACK] + PieceMat[WHITE]) / 24;
	int		simple_end	= GamePhase < 64;


	// Inicializando datos de la evaluacion
	memset(Weight,		0, sizeof(Weight));
	memset(EvWeight,	0, sizeof(EvWeight));

	PawnAttBB[BLACK]	= ((BLACK_PAWN_BB & (~FILE_A_BB)) >> 9) | ((BLACK_PAWN_BB & (~FILE_H_BB)) >> 7);
	PawnAttBB[WHITE]	= ((WHITE_PAWN_BB & (~FILE_A_BB)) << 7) | ((WHITE_PAWN_BB & (~FILE_H_BB)) << 9);

	PieceAttBB[BLACK]	= PieceAttBB[WHITE] = 0;

	KingAttBB[BLACK]	= KING_MOVES_BB(KING_SQ(BLACK));
	KingAttBB[WHITE]	= KING_MOVES_BB(KING_SQ(WHITE));

	KING_ZONE[BLACK]	= (!simple_end && WHITE_QUEENS) ? KingZoneBB[BLACK][KING_SQ(BLACK)] : 0;
	KING_ZONE[WHITE]	= (!simple_end && BLACK_QUEENS) ? KingZoneBB[WHITE][KING_SQ(WHITE)] : 0;


	// Evaluando posicion
	MaterialEval();

	KnightEval(simple_end);
	BishopEval(simple_end);
	RookEval(simple_end);
	QueenEval(simple_end);
	PawnEval(simple_end);
	KingEval(simple_end);

	// Recolecta los datos de los distintos parametros de evaluacion
	CollectDataEv();


	// Interpolando la evaluación entre el mediojuego y final. No es necesario hacerlo si se esta en un final simple
	if (simple_end)
		return *EndGame;

	return ((*MidGame) * GamePhase + (*EndGame) * (256 - GamePhase)) / 256;
}


/*
 * Evalúa Peones por:
 *
 * 1) Material-PST
 * 2) Aislados
 * 4) Retrasados
 * 5) Doblados
 * 6) Pasado
 * 7) Semi-pasado
 */
void	PawnEval(U8 simple_end)	{
	int			opened, view, backward, isolated, passed, passed_weight, candidate,
				count, doubled, advanced, support, support_attack, tmp, front;
	TURN		side, cside;
	SQUARE		sq, sq_rel, to;
	BITBOARD	bb, tmp_bb;


	TURN_LOOP(side)				{
		cside			= FLIP_TURN(side);
		view			= side ? 56 : 0;
		front			= Front[side];


		// Peones bloqueados en C2, D2, E2
		if (!simple_end)				{
			if (MATCH(PAWN | side, C7 ^ view) && MATCH(KNIGHT | side, C6 ^ view) && MATCH(PAWN | side, D5 ^ view) && NO_MATCH(PAWN | side, E5 ^ view))
				ActMidGame[side]	-= BlockedPawnC2;

			if (MATCH(PAWN | side, E7 ^ view) && MATCH(BISHOP | side, F8 ^ view) && NO_EMPTY(E6 ^ view))
				ActMidGame[side]	-= BlockedCentralPawn;

			if (MATCH(PAWN | side, D7 ^ view) && MATCH(BISHOP | side, C8 ^ view) && NO_EMPTY(D6 ^ view))
				ActMidGame[side]	-= BlockedCentralPawn;
		}


		for (bb = PAWN_BB(side); bb; bb ^= SquareBB[sq])		{
			sq			= LSB_BB(bb);
			sq_rel		= sq ^ view;
			opened		= (ForwardBB[side][sq] & PAWN_BB(cside)) == 0;
			doubled		= 2 * ((ForwardBB[cside][sq] & PAWN_BB(side)) != 0);
			isolated	= !(FileBB[sq] & PawnAttBB[side]);
			passed		= !(PawnPassedBB[side][sq] & PAWN_BB(cside));
			backward	= !(passed || isolated) && IsPawnBackward(sq);
			advanced	= !(passed || isolated || backward) && IsPawnAdvanced(sq);
			candidate	= !(backward || isolated || passed || advanced) && opened && IsPawnCandidate(sq);
			support		= support_attack = passed_weight = tmp = 0;

			if (candidate || passed)			{
				tmp_bb 			= ForwardBB[cside][sq] & (BLACK_ROOK_BB|WHITE_ROOK_BB) & ROOK_MOVES_BB(sq, ALL_PIECES_BB);
				support			= (tmp_bb & ROOK_BB(side))  != 0;
				support_attack	= (tmp_bb & ROOK_BB(cside)) != 0;
			}


			ActMidGame[side]	+= PawnMidGamePST[sq_rel];
			ActEndGame[side]	+= PawnEndGamePST[sq_rel];

			// Peón doblado
			if (doubled)		{
				if (isolated && (PawnPassedBB[side][sq] & PAWN_BB(cside)) && (PopCountBB(PawnPassedBB[side][sq] & PAWN_BB(cside)) <= PopCountBB(ForwardBB[cside][sq] & PAWN_BB(side))))
					doubled++;

				WeakPawnMidGame[side]	+= (PawnDoubled[sq_rel] * doubled) / 2;
				WeakPawnEndGame[side]	+= PawnDoubled[sq_rel]  * doubled;
			}

			// Peón Aislado
			if (isolated)		{
				IsolatedPawns[side]++;
				WeakPawnMidGame[side]	+= PawnIsolated[sq_rel] * (1 + opened);
				WeakPawnEndGame[side]	+= PawnIsolated[sq_rel] * 2;
			}

			// Peón retrasado. No pasado, aislado
			if (backward)		{
				WeakPawnMidGame[side]	+= PawnBackward[sq_rel]  * (1 + opened);
				WeakPawnEndGame[side]	+= (PawnBackward[sq_rel] * 3) / 2;
			}

			if (advanced)		{
				WeakPawnMidGame[side]	+= PawnBackward[sq]  * (1 + opened);
				WeakPawnEndGame[side]	+= (PawnBackward[sq] * 3) / 2;
			}

			// Peón Pasado
			if (passed)			{
				PassedPawns[side]++;
				to	= sq + front;

				tmp	+= (4 * Distance[to][KING_SQ(cside)] - Distance[to][KING_SQ(side)]) / 2;

				// Los peones pasados protegidos son mas peligrosos
				if (PawnAttBB[side] & SquareBB[sq + front])
					passed_weight	+= 2;

				else if (PawnAttBB[side] & SquareBB[sq])
					passed_weight++;

				// El rey enemigo no puede alcanzarlo
				if (!PieceMat[cside])			{
					to = LSB_BB(ForwardBB[side][sq] & (RANK_1_BB | RANK_8_BB));

					if (Distance[sq][to] < Distance[KING_SQ(cside)][to] + (Turn == side))
						tmp += 80;
				}

				// Determina que tan peligroso es un peón pasado
				for (to = sq + front, count = 3; count; to += front, count--)	{
					if (A1 <= to && to <= H8)	{
						if (Board[to])			{
							PassedPawnEndGame[side] += IsPawnPassedBloqued(to, sq);
							break;
						}

						if ((support_attack || EV_ATTACKED(to, cside)) && !(EV_ATTACKED(to, side) || support))
							break;
					}

					tmp += count;								// Peón pasado peligroso, puede avanzar libremente
				}

				PassedPawnEndGame[side] += PassedPawnPotential[sq_rel] * tmp;
			}

			passed_weight += (2 + 2 * support - support_attack) * (candidate + 2 * passed);
			if (passed_weight)	{
				PassedPawnMidGame[side]	+= (passed_weight * PawnPassed[sq_rel]) / 4;
				PassedPawnEndGame[side]	+= (passed_weight * PawnPassed[sq_rel]) / 2;
			}
		}
	}
}


/*
 * Evalúa Caballos por:
 *
 * 1) Material
 *	->	Material
 *	->	PST
 * 2) Movilidad
 *	->	Movilidad
 *	->	Soportado por peones
 *	->	Atacando a peones y piezas débiles
 *	->	Apostado
 *	->	Atrapado
 */
void	KnightEval(U8 simple_end)				{
	int			pawn_attack, mob, view;
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, mob_bb, bb2;


	TURN_LOOP(side)
		if (KNIGHTS(side))		{
			cside			= FLIP_TURN(side);
			view			= side ? 56 : 0;
			bb2				= ~(PawnAttBB[cside] | AllPieceBB[side]);


			for (bb = KNIGHT_BB(side); bb; bb ^= SquareBB[sq])	{
				sq			= LSB_BB(bb);
				sq_rel		= sq ^ view;
				mob_bb		= KNIGHT_MOVES_BB(sq);
				mob			= 4 * PopCountBB(mob_bb & bb2 & IMP4) + 2 * PopCountBB(mob_bb & bb2 & IMP2);

				pawn_attack	= PopCountBB(PAWN_ATTACK_BB(sq, side));
				if (pawn_attack)
					mob += 2;


				ActEndGame[side]		+= KnightEndGamePST[sq_rel];
				MobilityEndGame[side]	+= KnightMob[mob];

				// Atrapado, en esquina del tablero y casillas de salidas atacadas por peones. Idea del motor CPW
				if (mob < 4)
					switch (sq_rel)				{
						case A1: case H1:	TrappedPiece[side]	+= KnightTrappedA8;	break;
						case A2: case H2:	TrappedPiece[side]	+= KnightTrappedA7;	break;
						default:;
					}

				PutAttack(side, mob_bb, 2);
				if (simple_end)
					continue;


				ActMidGame[side]		+= KnightMidGamePST[sq_rel];
				MobilityMidGame[side]	+= KnightMob[mob];
				KingTropism[cside]		+= KNIGHT_TROPISM(sq, KING_SQ(cside));

				// Apostado, no es posible atacarlo con peones, está defendido por peones y en casillas centrales
				if (pawn_attack && KnightOutpost[sq_rel] && !(OutpostBB[side][sq] & PAWN_BB(cside)))
					ActMidGame[side]	+= pawn_attack * KnightOutpost[sq_rel];
			}
		}
}


/*
 * Evalúa Alfiles por:
 *
 * 1) Material
 *	->	Material
 *	->	PST
 * 2) Movilidad
 *	->	Movilidad
 *	->	Soportado por peones
 *	->	Atacando a peones y piezas débiles
 *	->	Atrapado
 */
void	BishopEval(U8 simple_end)				{
	int			two_bishop, mob, view, pawn_color;
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, mob_bb, bb2,
				all_bb		= ALL_PIECES_BB;


	TURN_LOOP(side)
		if (BISHOPS(side))		{
			cside			= FLIP_TURN(side);
			view			= side ? 56 : 0;
			two_bishop		= (BISHOP_BB(side) & WHITE_SQUARES_BB) && (BISHOP_BB(side) & BLACK_SQUARES_BB);
			bb2				= ~all_bb | (~PawnAttBB[cside] & AllPieceBB[cside]);


			for (bb = BISHOP_BB(side); bb; bb ^= SquareBB[sq])	{
				sq			= LSB_BB(bb);
				sq_rel		= sq ^ view;
				mob_bb		= BISHOP_MOVES_BB(sq, all_bb);
				mob			= 4 * PopCountBB(mob_bb & bb2 & IMP4) + 2 * PopCountBB(mob_bb & bb2 & IMP2) + PopCountBB(mob_bb & bb2 & IMP1);

				if (PawnAttBB[side] & SquareBB[sq])
					mob++;


				ActEndGame[side]		+= BishopEndGamePST[sq_rel];
				MobilityEndGame[side]	+= BishopMob[mob];

				/*
				 * Alfil malo, depende de:
				 *
				 * 1) El numero de peones del bando del alfil
				 * 2) Si tiene suficiente movilidad no es considerado como malo
				 */
				if (!two_bishop && mob < 9)		{
					pawn_color			= BadBishop[PopCountBB(PAWN_BB(side) & SquareColorBB[sq])];
					ActMidGame[side]	-= pawn_color / 4;
					ActEndGame[side]	-= pawn_color;
				}

				// Atrapado, en esquina del tablero y peones enemigos bloqueando las salidas. Idea del motor CPW
				if (mob < 4)
					switch (sq_rel)				{
						case A2: case H2:	TrappedPiece[side]	+= BishopTrappedA7;	break;
						case A3: case H3:	TrappedPiece[side]	+= BishopTrappedA6;	break;
						case B1: case G1:	TrappedPiece[side]	+= BishopTrappedB8;	break;
						default:;
					}

				PutAttack(side, mob_bb, 2);
				if (simple_end)
					continue;


				ActMidGame[side]		+= BishopMidGamePST[sq_rel];
				KingTropism[cside]		+= BISHOP_TROPISM(sq, KING_SQ(cside));
				MobilityMidGame[side]	+= BishopMob[mob];
			}
		}
}


/*
 * Evalúa Torres por:
 *
 * 1) Material
 *	->	Material
 *	->	PST
 * 2) Movilidad
 *	->	Movilidad
 *	->	Atacando a peones y piezas débiles
 *	->	Torre en columna abierta
 *	->	Torre doblada en columna abierta
 *	->	Torre en columna semi-abierta
 *	->	Torre en 7ma fila
 *	->	Atrapada
 */
void	RookEval(U8 simple_end)	{
	int			mob, view, front;
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, mob_bb, bb2,
				all_bb		= ALL_PIECES_BB;


	TURN_LOOP(side)
		if (ROOKS(side))		{
			cside			= FLIP_TURN(side);
			view			= side ? 56 : 0;
			bb2				= ~all_bb | (~PawnAttBB[cside] & AllPieceBB[cside]);
			front			= Front[side];


			for (bb = ROOK_BB(side); bb; bb ^= SquareBB[sq])	{
				sq			= LSB_BB(bb);
				sq_rel		= sq ^ view;
				mob_bb		= ROOK_MOVES_BB(sq, all_bb);
				mob			= 2 * PopCountBB(mob_bb & FileBB[sq] & bb2) + PopCountBB(mob_bb & RankBB[sq] & bb2);


				ActEndGame[side]		+= RookEndGamePST[sq_rel];
				MobilityEndGame[side]	+= RookMob[mob] * 2;

				// Torre en 7ma fila si peones enemigos en 7ma o Rey enemigo en 8va
				if (RANK(sq_rel) == RANK_2 && ((RankBB[sq] & PAWN_BB(cside)) || (RankBB[sq + front] & KING_BB(cside))))			{
					ActMidGame[side]	+= RookIn7th;
					ActEndGame[side]	+= RookIn7th * 2;
				}


				PutAttack(side, mob_bb, 3);
				if (simple_end)
					continue;

				ActMidGame[side]		+= RookMidGamePST[sq_rel];
				MobilityMidGame[side]	+= RookMob[mob];
				KingTropism[cside]		+= ROOK_TROPISM(sq, KING_SQ(cside));

				if (!(FileBB[sq] & PAWN_BB(side)))				{
					if (FileBB[sq] & PAWN_BB(cside))			{
						// Torre en columna semi-abierta
						ActMidGame[side]	+= RookInHalfOpenFile;
						ActEndGame[side]	+= RookInHalfOpenFile;

						// Atacando al Rey enemigo
						if (abs(FILE(KING_SQ(cside)) - FILE(sq)) < 2)
							AttackWeight[side] += 2;
					}

					else		{
						// Torre en columna abierta
						ActMidGame[side]	+= RookInOpenFile;
						ActEndGame[side]	+= RookInOpenFile;

						// Atacando al Rey enemigo
						if (abs(FILE(KING_SQ(cside)) - FILE(sq)) < 2)
							AttackWeight[side]++;

						// Torres y Damas dobladas en columna abierta
						if (mob_bb & FileBB[sq] & ROOK_BB(side))
							ActMidGame[side]	+= RookSuppRook;

						if (mob_bb & FileBB[sq] & QUEEN_BB(side))
							ActMidGame[side]	+= QueenSuppRook;
					}
				}

				// Atrapada, en esquina del tablero y peones amigos bloqueando las salidas. Idea del motor CPW
				if (mob < 6)
					switch (KING_SQ(side) ^ view)				{
						case F8: case G8:
							if (sq_rel == H8 || sq_rel == H7 || sq_rel == G8)	{
								TrappedPiece[side]		+= RookTrapped;
								KingSafetyMidGame[side]	-= CasttleDone;
							}
							break;

						case C8: case B8:
							if (sq_rel == A8 || sq_rel == A7 || sq_rel == B8)	{
								TrappedPiece[side]		+= RookTrapped;
								KingSafetyMidGame[side]	-= CasttleDone;
							}
							break;

						case H8: case A8: case G7: case B7:
							if (sq == KING_SQ(side) + front)	{
								TrappedPiece[side]		+= RookTrapped;
								KingSafetyMidGame[side]	-= CasttleDone;
							}
							break;
						default:;
					}
			}
		}
}


/*
 * Evalúa Damas por:
 *
 * 1) Material
 *	->	Material
 *	->	PST
 * 2) Movilidad
 *	->	Movilidad
 *	->	Dama prematura
 */
void	QueenEval(U8 simple_end){
	int			view, mob;
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, mob_bb,
				all_bb		= ALL_PIECES_BB;


	TURN_LOOP(side)
		if (QUEENS(side))		{
			cside	= FLIP_TURN(side);
			view	= side ? 56 : 0;


			for (bb = QUEEN_BB(side); bb; bb ^= SquareBB[sq])	{
				sq			= LSB_BB(bb);
				sq_rel		= sq ^ view;
				mob_bb		= QUEEN_MOVES_BB(sq, all_bb);
				mob			= PopCountBB(mob_bb & ~all_bb);


				ActEndGame[side]		+= QueenEndGamePST[sq_rel];
				MobilityEndGame[side]	+= QueenMob[mob] * 2;


				PutAttack(side, mob_bb, 5);
				if (simple_end)
					continue;

				KingTropism[cside]		+= QUEEN_TROPISM(sq, KING_SQ(cside));
				ActMidGame[side]		+= QueenMidGamePST[sq_rel];
				MobilityMidGame[side]	+= QueenMob[mob];

				// Dama prematura
				if (sq_rel != D8 && (MATCH(KNIGHT | side, B8 ^ view) || MATCH(BISHOP | side, C8 ^ view) || MATCH(KNIGHT | side, G8 ^ view) || MATCH(BISHOP | side, F8 ^ view)))
					ActMidGame[side] -= QueenPremature;
			}
		}
}


// Evalua la prescencia o falta de fiancheto en el enroque
int		FianchettoEval(PIECE piece)				{
	switch (piece)				{
		case PAWN: case KING:	return 0;
		case BISHOP:			return Fianchetto;
		default:				return -Fianchetto;
	}
}


int		KingPawnShield(SQUARE sq, TURN side)	{
	int		score	= 0,
			front	= Front[side];
	TURN	cside	= FLIP_TURN(side);
	sq				= REL_SQ(sq, side);


	// Penalizacion, peon movido del escudo del Rey
	if (!(FileBB[sq] & PAWN_BB(side)))
		score		+= NoPawnInFile;							// No hay peon en la columna

	else if (NO_MATCH(PAWN | side, sq))			{
		if (MATCH(PAWN | side, sq + front))
			score	+= Pawn3th;									// Peon movido 1 casilla

		else score	+= Pawn4th;									// Peon movido 2 o mas casillas
	}

	// Penalizacion, avalancha hacia el Rey de peones enemigos
	if (!(FileBB[sq] & PAWN_BB(cside)))
		score		+= NoEnemyPawnInFile;						// No hay peones enemigos en la columna

	else if (MATCH(PAWN | cside, sq + front))
		score		+= EnemyPawn3th;							// Peon enemigo en 3ra fila

	else if (MATCH(PAWN | cside, sq + 2 * front))
		score		+= EnemyPawn4th;							// Peon enemigo en 4ta fila


	return score;
}

// ESCUDO DEL REY, avalancha de peones, peones movidos del enroque, fiancheto, etc.
void	Shield(int file, TURN side)				{
	BITBOARD bb;


	switch (file)				{
		// Enroque corto
		case FILE_G: case FILE_H:
			KingShield[side]		+= KingPawnShield(H7, side) + KingPawnShield(G7, side) + KingPawnShield(F7, side) / 2;
			KingSafetyMidGame[side]	+= FianchettoEval(Board[REL_SQ(G7, side)] ^ side);
			break;

		// Enroque largo
		case FILE_A: case FILE_B: case FILE_C:
			KingShield[side]		+= KingPawnShield(A7, side) + KingPawnShield(B7, side) + KingPawnShield(C7, side) / 2;
			KingSafetyMidGame[side]	+= FianchettoEval(Board[REL_SQ(B7, side)] ^ side);
			break;

		// Rey en el centro
		default:
			bb						= BLACK_PAWN_BB | WHITE_PAWN_BB;
			KingShield[side]		+= KingInOpenFile * (!(FileBB[file] & bb) + !(FileBB[file - 1] & bb) + !(FileBB[file + 1] & bb));

			// Pérdida del enroque
			if (QUEENS(FLIP_TURN(side)) && !((KingSideCasttle[side] | QueenSideCasttle[side]) & Casttle))
				KingSafetyMidGame[side]	-= CasttleDone;
	}
}


/*
 * Evalúa Reyes por:
 *
 * 1) Material
 *	->	PST
 * 2) Movilidad
 *	->	Movilidad
 * 3) Seguridad del Rey
 *	->	Escudo del Rey
 */
void	KingEval(U8 simple_end)	{
	TURN		side, cside;
	SQUARE		sq, sq_rel;
	BITBOARD	bb, bb2, mob_bb, king_front_bb;
	int			view, score;


	TURN_LOOP(side)				{
		cside		= FLIP_TURN(side);
		view		= side ? 56 : 0;
		sq			= KING_SQ(side);
		sq_rel		= sq ^ view;
		mob_bb		= KING_MOVES_BB(sq);


		ActEndGame[side]	+= KingEndGamePST[sq_rel] + PopCountBB(mob_bb & ~PawnAttBB[cside] & PAWN_BB(cside)) * KingMob;

		if (simple_end)
			continue;

		ActMidGame[side]	+= KingMidGamePST[sq_rel];

		Shield(FILE(sq), side);

		/*
		 * ATAQUE AL REY
		 * Mezcla de la seguridad del rey de los motores Rebel y Stockfish.
		 * Tiene en cuenta la cantidad de ataques de piezas y su tipo de piezas a casillas cercanas al Rey(Stockfish).
		 * Tambien analiza si esas casillas estan defendidas o ucupadas por piezas del mismo bando o del
		 * enemigo o si no hay peones en esas casillas(Rebel).
		 */
		if (AttackCount[cside] > 4 && PieceMat[cside] > QUEEN_VALUE + BISHOP_VALUE)				{
			bb2			= EV_ATTACKED_BB(cside);
			king_front_bb= KingFrontBB[side][sq];
			bb			= king_front_bb & bb2 & ~PAWN_BB(side);

			score		= PopCountBB(KING_ZONE[side] & PawnAttBB[cside])
						+ AttackWeight[cside] + SRKingCount[sq_rel]										// Penalizar, Zona de Rey Atacada(segun atacante) o Rey en centro de tablero
						+ PopCountBB(mob_bb & bb2 & ~EV_ATTACKED_WITHOUT_KING_BB(side))					// Penalizar, Rey unico defensor de casillas atacadas
						+ PopCountBB(king_front_bb & ~bb2 & ~AllPieceBB[side])							// Penalizar, casillas frente al Rey no atacadas, pero vacia o pieza enemiga ocupandola
						+ 2 * PopCountBB(bb & ~AllPieceBB[side]) + PopCountBB(bb & AllPieceBB[side]);	// Penalizar, casillas frente a Rey atacadas no ocupadas por peon del mismo bando, duplicar si no ocupada por el mismo bando

			KingAttack[side]	= SafetyTable[score];
		}
	}
}


// Evaluacion de las constelaciones del material
void	MaterialEval(void)		{
	int		tmp, mat_diff, 
			pawns_side, pawns_cside, mid_score, end_score;
	TURN	side, cside;


	if (ProbeMatScore(&mid_score, &end_score, (U32)HashMat))	{
		MaterialMidGame[WHITE]	+= mid_score;
		MaterialEndGame[WHITE]	+= end_score;
		return;
	}


	// Algunos valores para el desvalance material segun Larry Kaufman
	TURN_LOOP (side)			{
		cside		= FLIP_TURN(side);
		pawns_side	= MIN(8, PAWNS(side));
		pawns_cside	= MIN(8, PAWNS(cside));
		tmp			= KNIGHTS(side)	* KnightAdjust[pawns_side]- KnightPair* (KNIGHTS(side)> 1)
					+ ROOKS(side)	* RookAdjust[pawns_side]	- RookPair	* (ROOKS(side)	> 1)
					- PawnAdjust[pawns_side];

		/*
		 * Pareja de alfiles, depende de:
		 *
		 * 1) El numero total de peones y donde están
		 * 2) Si el rival tiene piezas menores
		 */
		if ((BISHOP_BB(side) & WHITE_SQUARES_BB) && (BISHOP_BB(side) & BLACK_SQUARES_BB))		{
			MaterialMidGame[side] += BishopPairMid[pawns_side + pawns_cside];
			MaterialEndGame[side] += BishopPairEnd[pawns_side + pawns_cside];

			if (!BISHOPS(cside)){
				if (!KNIGHTS(cside))
					tmp		+= (3 * WithOutBishop) >> 1;

				else tmp	+= WithOutBishop;
			}
		}

		MaterialMidGame[side]	+= tmp + PAWNS(side) * PawnMidGameMat + KNIGHTS(side) * KnightMidGameMat + BISHOPS(side) * BishopMidGameMat + ROOKS(side) * RookMidGameMat + QUEENS(side) * QueenMidGameMat;
		MaterialEndGame[side]	+= tmp + PAWNS(side) * PawnEndGameMat + KNIGHTS(side) * KnightEndGameMat + BISHOPS(side) * BishopEndGameMat + ROOKS(side) * RookEndGameMat + QUEENS(side) * QueenEndGameMat;
	}


	mat_diff	= PieceMat[WHITE] - PieceMat[BLACK];
	side		= WHITE;

	if (mat_diff < 0)			{
		mat_diff	= -mat_diff;
		side		= BLACK;
	}


	// Mayor contra peones
	if (mat_diff >= ROOK_VALUE)	{
		MaterialMidGame[side]	+= RookVSpawnsMid;
		MaterialEndGame[side]	+= RookVSpawnsEnd;
	}

	// Menor contra peones
	else if (mat_diff >= KNIGHT_VALUE)			{
		MaterialMidGame[side]	+= KnightVSpawnsMid;
		MaterialEndGame[side]	+= KnightVSpawnsEnd;
	}

	// Menor contra torre
	else if (mat_diff >= ROOK_VALUE - BISHOP_VALUE)				{
		MaterialMidGame[side]	+= RookVSknightMid;
		MaterialEndGame[side]	+= RookVSknightEnd;
	}

	else	{
		mat_diff	= KNIGHTS(WHITE) + BISHOPS(WHITE) - KNIGHTS(BLACK) - BISHOPS(BLACK);
		side		= WHITE;

		if (mat_diff < 0)		{
			mat_diff	= -mat_diff;
			side		= BLACK;
		}

		// Dos menores contra torre
		if (mat_diff > 1)		{
			MaterialMidGame[side]	+= KnightBishopVSrookMid;
			MaterialEndGame[side]	+= KnightBishopVSrookEnd;
		}
	}


	mid_score	= MaterialMidGame[WHITE] - MaterialMidGame[BLACK];
	end_score	= MaterialEndGame[WHITE] - MaterialEndGame[BLACK];

	StoreMatScore(mid_score, end_score, (U32)(HashMat));
}


// Peón semi-pasado. Solo tiene en cuenta a los peones y sus ataques para detectarlo
U8		IsPawnCandidate(SQUARE sq)				{
	TURN		side		= GET_COLOR(Board[sq]),
				cside		= FLIP_TURN(side);
	int			front		= Front[side];

	return PopCountBB(OutpostBB[cside][sq + front] & PAWN_BB(side)) >= PopCountBB(OutpostBB[side][sq + front] & PAWN_BB(cside));
}

// Peón retrasado. Solo tiene en cuenta a los peones y sus ataques para detectarlo
U8		IsPawnBackward(SQUARE sq)				{
	SQUARE		to;
	BITBOARD	bb;
	TURN		side	= GET_COLOR(Board[sq]),
				cside	= FLIP_TURN(side);
	int			front	= Front[side];


	if ((OutpostBB[cside][sq] & PAWN_BB(side)) || !(ForwardBB[side][sq] & (PawnAttBB[side] | PAWN_BB(cside))))
		return 0;												// No es retrasado. Existe peon vecino mas atrasado o los peones enemigos no impiden su avance

	for (to	= sq + front;; to += front)			{
		bb	= SquareBB[to];

		if (bb & PAWN_BB(cside))
			return 1;

		if (bb & PawnAttBB[side])
			return PopCountBB(PAWN_ATTACK_BB(to, cside)) > PopCountBB(PAWN_ATTACK_BB(to, side));

		if (bb & PawnAttBB[cside])
			return 1;
	}

	return 0;
}

// falla en detectar algunos casos
U8		IsPawnAdvanced(SQUARE sq)				{
	SQUARE		to;
	BITBOARD	bb;
	TURN		side		= GET_COLOR(Board[sq]),
				cside		= FLIP_TURN(side);
	int			front		= Front[side];


	if ((OutpostBB[side][sq] & PAWN_BB(side)) || !(OutpostBB[cside][sq] & PAWN_BB(side)) || (SquareBB[sq] & PawnAttBB[side]))
		return 0;												// No es avanzado.	Existen peones vecinos por delante o no existen peones enemigos estorbando la defensa

	// Comprobando si puede ser defendido por la izquierda
	bb = ForwardBB[cside][sq - 1];
	if (FILE(sq) != FILE_A && (bb & PAWN_BB(side)))				{
		if (bb & (PAWN_BB(cside) | PawnAttBB[cside]))
			for (to = sq - front - 1;; to -= front)				{
				if (MATCH(PAWN | side, to))
					return 0;

				if (MATCH(PAWN | cside, to) || PopCountBB(PAWN_ATTACK_BB(to, side)) < PopCountBB(PAWN_ATTACK_BB(to, cside)))
					break;
			}

		else return 0;
	}

	// Comprobando si puede ser defendido por la derecha
	bb = ForwardBB[cside][sq + 1];
	if (FILE(sq) != FILE_H && (bb & PAWN_BB(side)))				{
		if (bb & (PAWN_BB(cside) | PawnAttBB[cside]))
			for (to = sq - front + 1;; to -= front)				{
				if (MATCH(PAWN | side, to))
					return 0;

				if (MATCH(PAWN | cside, to) || PopCountBB(PAWN_ATTACK_BB(to, side)) < PopCountBB(PAWN_ATTACK_BB(to, cside)))
					break;
			}

		else return 0;
	}

	return 1;
}

// Bloqueando a peones enemigos
int		IsPawnPassedBloqued(SQUARE bloqued_sq, SQUARE sq)		{
	TURN		side		= GET_COLOR(Board[sq]);
	PIECE		piece_block	= Board[bloqued_sq] ^ side;
	int			view		= side ? 56 : 0,
				out_post	= BISHOPS(side) && !KNIGHTS(side) && !(SquareColorBB[bloqued_sq] & BISHOP_BB(side)),	// Puntuacion por bloquear a peon pasado, bonifica si el bloqueador no puede ser desalojado
				score		= ((Distance[sq][bloqued_sq] - 4) * (2 + out_post)) / 2;


	switch (piece_block)		{
		case ROOK:			return -RookBlockPassedPawn;		// Torre amiga bloqueando el peón pasado

		// Malos bloqueadores, se penaliza
		case WHITE_ROOK:	return PawnPassed[sq ^ view];
		case WHITE_QUEEN:	return (PawnPassed[sq ^ view] * 3) / 2;

		// Buenos bloqueadores, ptos. extra si no se pueden desalojar
		case WHITE_KNIGHT:	return KnightBloqPawn	* score;
		case WHITE_BISHOP:	return BishopBloqPawn	* score;
		case WHITE_KING:	return KingBloqPawn		* score;

		default:			return 0;
	}
}


// Detecta tablas por insuficiencia material
U8		MatInsuf(void)			{
	BITBOARD	bb	= BISHOP_BB(BLACK) | BISHOP_BB(WHITE);


	if (HashMat & 0xFFF000FF)
		return 0;												// Salida rápida, existen peones, damas o torres

	switch ((U32)HashMat)		{
		case KK: case KNK: case KKN: case KKB: case KBK: case KNNK: case KKNN:	return 1;

		case KBBK: case KKBB: case KBKB: case KBBKB: case KBKBB: case KBBKBB:	return !((bb & BLACK_SQUARES_BB) && (bb & WHITE_SQUARES_BB));
		default: return 0;
	}
}

// Algunos finales soportados. El retorno por debajo de 16	es para posiciones tablas
int		SupportedEndGames(void)	{
	SQUARE		P, r, R, k, K, prom_sq;
	int			dist, P_File, r_File, r_Rank, tmp, k_File, K_File, view, all_pawns_in_file, k_in_front_pawn,
				mat_diff		= abs(PieceMat[BLACK] + PawnMat[BLACK] - PieceMat[WHITE] - PawnMat[WHITE]),
				pawn_diff		= abs(BLACK_PAWNS - WHITE_PAWNS);
	U32			MatHashPiece	= (U32)(HashMat & 0xFFFFFF00);
	BITBOARD	bb				= BLACK_PAWN_BB		| WHITE_PAWN_BB,
				bb2				= BLACK_BISHOP_BB	| WHITE_BISHOP_BB;
	TURN		side			= 2,
				cside;


	if (GamePhase > 107)
		return 0;												// Salida Rapida, no se tiene suficiente material

	// Se pone el punto de vista del tablero del bando que tenga peones, sino al que tenga alfil
	if (BLACK_PAWNS && !WHITE_PAWNS)
		side	= BLACK;

	else if (WHITE_PAWNS && !BLACK_PAWNS)
		side	= WHITE;

	else if (BLACK_BISHOP_BB)
		side	= BLACK;

	else if (WHITE_BISHOP_BB)
		side	= WHITE;

	for (P = LSB_BB(PAWN_BB(side)); ForwardBB[side][P] & PAWN_BB(side); )
		P = LSB_BB(ForwardBB[side][P] & PAWN_BB(side));


	cside				= FLIP_TURN(side);
	K					= KING_SQ(side);
	k					= KING_SQ(cside);
	prom_sq				= LSB_BB(ForwardBB[side][P] & (RANK_1_BB | RANK_8_BB));
	k_File				= FILE(k);
	K_File				= FILE(K);
	view				= side ? 0 : 56;
	all_pawns_in_file	= (~FileBB[P] & bb) == 0;
	k_in_front_pawn		= (ForwardBB[side][P] & KING_BB(cside)) != 0;


	switch ((U32)HashMat)		{
		// Insuficiencia material									0 -> 0
		case KK: case KNK: case KKN: case KBK: case KKB:		return DRAW_SCORE(0, 0);

		// Otras Tablas
		case KBKP: case KPKB: case KNKP: case KPKN:

		case KNNKP: case KPKNN:
		case KNPKR: case KRKNP: case KBPKR: case KRKBP:
		case KPPKN: case KNKPP: case KPPKB: case KBKPP:

		case KNNKPP: case KPPKNN:
		case KNPKNN: case KNNKNP: case KBPKNN: case KNNKBP: case KNPKBN: case KBNKNP:
		case KBPKBN: case KBNKBP: case KNPKBB: case KBBKNP: case KBPKBB: case KBBKBP:
		case KRPKNN: case KNNKRP: case KRPKBN: case KBNKRP: case KRPKBB: case KBBKRP:
		case KRPKRN: case KRNKRP: case KRPKRB: case KRBKRP:
		case KQNKQP: case KQPKQN: case KQBKQP: case KQPKQB:

		case KRNKNNP: case KNNPKRN: case KRNKBNP: case KBNPKRN: case KRNKBBP: case KBBPKRN: case KRBKBBP: case KBBPKRB:
		case KNPPKNN: case KNNKNPP: case KNPPKBN: case KBNKNPP: case KBPPKBN: case KBNKBPP: case KBPPKBB: case KBBKBPP:
		case KNNKPPP: case KPPPKNN:
		case KBNNKRP: case KRPKBNN:
		case KRPPKRN: case KRNKRPP: case KRPPKRB: case KRBKRPP:
		case KNNKNNP: case KNNPKNN: case KBNKNNP: case KNNPKBN: case KBBKNNP: case KNNPKBB: case KNNKBNP: case KBNPKNN: case KBNKBNP:
		case KBNPKBN: case KBBKBNP: case KBNPKBB: case KBBKBBP: case KBBPKBB: case KNNKBBP: case KBBPKNN: case KBNKBBP: case KBBPKBN:
		case KRNKRNP: case KRNPKRN: case KRBKRNP: case KRNPKRB: case KRNKRBP: case KRBPKRN: case KRBKRBP: case KRBPKRB:
		case KQNKQPP: case KQPPKQN: case KQBKQPP: case KQPPKQB:
		case KQNKQNP: case KQNPKQN: case KQBKQNP: case KQNPKQB: case KQNKQBP: case KQBPKQN: case KQBKQBP: case KQBPKQB:

		case KBNNKRPP: case KRPPKBNN:	return DRAW_SCORE(MatSum, mat_diff);

		case KRBKR: case KRKRB:
			if ((bb2 & BLACK_SQUARES_BB) && (KING_BB(cside) & 0xEC0800000010307))
				return 0;

			if ((bb2 & WHITE_SQUARES_BB) && (KING_BB(cside) & 0x70301000080C0E0))
				return 0;

			return DRAW_SCORE(MatSum, mat_diff);

		// Soporte para KPK o KKP
		case KPK: case KKP:
			if (Distance[prom_sq][P] + (Turn == cside) < Distance[prom_sq][k])
				return 2;

			if (Distance[K][P] + (Turn == cside) > Distance[P][k])
				return DRAW_DIST_SCORE(MatSum, mat_diff, Distance[P][k]);	// Rey enemigo captura al peón

			// peon en columna A o H
			if (bb & (FILE_A_BB | FILE_H_BB))
				return DRAW_DIST_SCORE(MatSum, mat_diff, Distance[k][prom_sq ^ 9]);

			tmp = P + 2 * Front[side];
			if (tmp < A1 || tmp > H8 || PopCountBB(KingAttBB[side] & SquareBB[tmp]))
				return 2;										// Rey controla las casillas por delante del peón

			return DRAW_SCORE(MatSum, mat_diff);					// Rey enemigo controla las casillas por delante del peón

		// Soporte para KPK, KKP con todos los peones en columna A o H
		case KPPK: case KKPP: case KPPPK: case KKPPP: case KPPPPK: case KKPPPP:
			if (all_pawns_in_file && (bb & (FILE_A_BB | FILE_H_BB)))
				return DRAW_DIST_SCORE(MatSum, mat_diff, Distance[k][prom_sq ^ 9]);
			break;

		// Soporte para KBPK, KKBP con todos los peones en columna A o H
		case KBPK: case KKBP: case KBPPK: case KKBPP: case KBPPPK: case KKBPPP:
			if (all_pawns_in_file && (bb & (FILE_A_BB | FILE_H_BB)) && !(bb2 & SquareColorBB[prom_sq]))
				return DRAW_DIST_SCORE(MatSum / 2, mat_diff / 2, Distance[k][prom_sq ^ 9]);
			break;

		// Soporte para KQKP, KPKQ, con todos los peones en columna A, C, F o H
		case KQKP: case KPKQ: case KQKPP: case KPPKQ: case KQKPPP: case KPPPKQ:
			if (all_pawns_in_file && MATCH(PAWN | side, prom_sq - Front[side]))	{
				if ((FILE_A_BB & bb) && (KING_BB(cside) & KQKP_DB1[cside]))
					return DRAW_SCORE(MatSum, mat_diff);

				if ((FILE_H_BB & bb) && (KING_BB(cside) & KQKP_DB2[cside]))
					return DRAW_SCORE(MatSum, mat_diff);

				if ((FILE_C_BB & bb) && (KING_BB(cside) & KQKP_DB3[cside]))
					return DRAW_SCORE(MatSum, mat_diff);

				if ((FILE_F_BB & bb) && (KING_BB(cside) & KQKP_DB4[cside]))
					return DRAW_SCORE(MatSum, mat_diff);
			}
			break;

		// Lo importante es no llevar rey débil a esquina de color contrario al alfil, para poder defenderse
		case KRKB: case KBKR:
			if ((bb2 & BLACK_SQUARES_BB) && (KING_BB(!BLACK_BISHOPS) & 0xE0E0E00000070707L))
				return DRAW_SCORE(MatSum, mat_diff);

			if ((bb2 & WHITE_SQUARES_BB) && (KING_BB(!BLACK_BISHOPS) & 0x707070000E0E0E0L))
				return DRAW_SCORE(MatSum, mat_diff);
			break;

		/*
		 * 1) Controlar casilla por delante del peón de color distinto al alfil con pieza(NO REY)
		 * 2) Ocupar casilla por delante del peón de color distinto al alfil
		 */
		case KNKBP: case KBKBP: case KBPKN: case KBPKB: case KNKBPP: case KBKBPP: case KBPPKN: case KBPPKB:
			if (all_pawns_in_file)				{
				if (bb2 & BLACK_SQUARES_BB)
					bb	= ForwardBB[side][P] & WHITE_SQUARES_BB;

				else bb	= ForwardBB[side][P] & BLACK_SQUARES_BB;


				if (AllPieceBB[cside] & bb)
					return DRAW_SCORE(MatSum, mat_diff);

				if (KNIGHT_BB(cside))				{
					if (KnightMoveBB[LSB_BB(KNIGHT_BB(cside))] & bb)
						return DRAW_SCORE(MatSum, mat_diff);
				}
				else if (BISHOP_BB(cside) && (BishopPseudoMoveBB[LSB_BB(BISHOP_BB(cside))] & bb))
					return DRAW_SCORE(MatSum, mat_diff);
			}
			break;

		/*
		 * Mate elemental con caballo y alfil
		 *
		 * 1) acercar caballo a rey débil
		 * 2) acercar rey a rey débil
		 * 3) controlar casillas de color distinto al alfil con caballo
		 * 4) llevar a esquina de color del alfil al rey débil
		 */
		case KKBN:	case KBNK:
			MaterialEndGame[cside] -= 150 / Distance[K][k] + 128 / Distance[k][LSB_BB(KNIGHT_BB(side))];

			if (BISHOP_BB(side) & BLACK_SQUARES_BB)				{
				if (KNIGHT_BB(side) & BLACK_SQUARES_BB)
					MaterialEndGame[cside] -= 32;

				return DRAW_DIST_SCORE(MatSum, mat_diff, MIN(Distance[k][H1], Distance[k][A8]));
			}

			if (KNIGHT_BB(side) & WHITE_SQUARES_BB)
				MaterialEndGame[cside] -= 32;

			return DRAW_DIST_SCORE(MatSum, mat_diff, MIN(Distance[k][A1], Distance[k][H8]));

		case KRKP: case KPKR: case KRKPP: case KPPKR:
			if (all_pawns_in_file)				{
				r		= LSB_BB(ROOK_BB(cside));
				r_Rank	= RANK(REL_SQ(r, cside));
				r_File	= FILE(r);
				P_File	= FILE(P);

				// Rey enemigo en frente del peon o el Rey no esta cerca para apoyar el peón
				if (k_in_front_pawn || (Distance[P][prom_sq] > 1 && Distance[K][P] > 2))
					break;

				// Peon en 7ma apoyado por su Rey
				if (prom_sq == P + Front[side] && Distance[K][P] < 2)
					return DRAW_SCORE(MatSum, mat_diff);

				// Torre cortando al rey enemigo en 5ta
				if (r_Rank <= RANK_4 && RANK(REL_SQ(K, side)) < r_Rank)
					break;

				// Equivalente al soporte de Fruit al final KRKP
				if ((Turn == cside) && Distance[K][P] == 2 && abs(RANK(K) - RANK(P)) < 2 && r_File != (K_File + P_File) / 2)
					break;

				dist	= Distance[K][prom_sq] + Distance[P][prom_sq] + (K == prom_sq)
						- ((r_File != P_File && RANK(r ^ view) != RANK_8) + (Turn == side));

				if (P + Front[side] == K)			{
					if (FileBB[P] & (FILE_A_BB | FILE_H_BB))
						break;

					dist++;
				}

				return DRAW_DIST_SCORE(MatSum, mat_diff, MAX(0, Distance[k][prom_sq] - dist));
			}
			break;

		case KRKRP:	case KRPKR:
			if (all_pawns_in_file)				{
				R		= LSB_BB(ROOK_BB(side));
				r		= LSB_BB(ROOK_BB(cside));
				r_File	= FILE(r);
				P_File	= FILE(P);

				if (k_in_front_pawn && (RANK(P ^ view) >= RANK_6) && (FILE_C > P_File || P_File > FILE_F) && RANK(r ^ view) == RANK_8 && RANK(k ^ view) == RANK_8)
					return DRAW_SCORE(MatSum, mat_diff);		// Back-rank defense

				// Pawn in rook file
				if (FileBB[P] & (FILE_A_BB | FILE_H_BB))		{
					if (RANK(P ^ view) == RANK_7)	{
						if (k_in_front_pawn && abs(P_File - r_File) == 1 && abs(P_File - k_File < 5))
							return DRAW_SCORE(MatSum, mat_diff);// King in front of pawn

						if (R == prom_sq && P_File == r_File && (Distance[k][R] < 3 || (RANK(k ^ view) == RANK_7 && Distance[k][P] > 5)))
							return DRAW_SCORE(MatSum, mat_diff);// Rook in front of pawn
					}
					else if (RANK(r) == RANK(P) && MIN(Distance[k ^ view][H8], Distance[k ^ view][A8]) < 2 && (ForwardBB[side][P] & ROOK_BB(side)))
						return DRAW_SCORE(MatSum, mat_diff);	// Vancura position

					if (k_in_front_pawn)
						return DRAW_SCORE(MatSum, mat_diff);	// Enemy king in front of pawn
				}

				// Philidor position
				if (PawnPassedBB[side][P] & KING_BB(cside))			{
					if (RANK(P ^ view) < RANK_6)	{
						if (RANK(K ^ view) < RANK_6 && RANK(r ^ view) == RANK_6)
							return DRAW_SCORE(MatSum, mat_diff) / 2;
					}
					else if (RANK(K ^ view) > RANK_2)
						return DRAW_SCORE(MatSum, mat_diff);

					return DRAW_SCORE(MatSum, mat_diff) / 4;
				}

				if ((P_File == FILE_B || P_File == FILE_G) && Distance[K][P] == 1 && abs(P_File - r_File) > 3 && abs(P_File - k_File) < 3 && Distance[k][r] > 5)
					return DRAW_SCORE(MatSum, mat_diff);		// Short side defense
			}
			break;
		default:;
	}


	// Ambos bandos tienen igual material(no se tienen en cuenta los peones)
	if (MatHashPiece && PieceMat[BLACK] == PieceMat[WHITE])		{
		if (k_in_front_pawn && PAWNS(side) == 1)
			return DRAW_SCORE(MatSum, mat_diff);				// Difícil ganar cuando rey enemigo esta frente al único peón

		// Difícil ganar cuando: pocos peones, no debilidades o fortalezas sustanciales
		if (pawn_diff < 2 && ALL_PAWNS < 7 && BLACK_PAWNS && WHITE_PAWNS)		{
			if (ALL_PAWNS < 3)	{
				if (!(PassedPawns[BLACK] || PassedPawns[WHITE]))
					return DRAW_SCORE(MatSum, mat_diff);		// 1 solo peon por bando sin peones pasados
			}

			else	{
				side	= BLACK_PAWNS > WHITE_PAWNS;
				cside	= FLIP_TURN(side);

				if (Distance[KING_SQ(side)][LSB_BB(PAWN_BB(side))] < 3 && !PassedPawns[cside] && (PAWNS(side) == 1 || !IsolatedPawns[side]))
					return DRAW_SCORE(MatSum, mat_diff);		// 2, 3 por bando sin debilidades o fortalezas en ambos bandos
			}
		}
	}


	tmp = (pawn_diff < 2) && (PassedPawns[BLACK] < 2) && (PassedPawns[WHITE] < 2);
	switch (MatHashPiece)		{
		case KK: case KNKN: case KNNKNN:	return 14;			// Prefiere ir final de peones o de caballos

		// Final de alfiles. Ojo con los peones pasados
		case KBKB:
			// Alfiles de distinto color
			if ((bb2 & BLACK_SQUARES_BB) && (bb2 & WHITE_SQUARES_BB))	{
				if (pawn_diff < 4)
					return DiferentBishop[MIN(16, ALL_PAWNS)];
			}
			// Alfiles de igual color
			else if (pawn_diff < 2)
				return DiferentBishop[MIN(16, ALL_PAWNS)] / 2;
			break;

		// Estos finales posiblemente progresen a un final tablas. Asegurar no haber muchos peones pasados
		case KRKR: case KQKQ: case KRRKRR: case KRBKRB: case KQBKQB:
			if (tmp)
				return DiffEndGame[MIN(16, ALL_PAWNS)];
			break;

		case KBBKBB:
			if (tmp)
				return DiferentBishop[MIN(16, ALL_PAWNS)] / 2;
			break;

		default:;
	}


	/*
	 * Regla general(no perfecta) para identificar posiciones tablas:
	 * Si ambos bandos no tienen peones se necesita el equivalente a casi 4 peones para poder ganar
	 */
	if (!ALL_PAWNS && mat_diff < 7 * PAWN_VALUE / 2)				{
		if (PieceMat[WHITE] > PieceMat[BLACK] && !(KING_BB(BLACK) & 0xFFC381818181C3FF))
			return DRAW_SCORE(MatSum, mat_diff);

		if (PieceMat[BLACK] > PieceMat[WHITE] && !(KING_BB(WHITE) & 0xFFC381818181C3FF))
			return DRAW_SCORE(MatSum, mat_diff);
	}

	return 0;
}


// Precalcula datos necesarios en la evaluacion
void	EvalIni(void)			{
	SQUARE	sq1, sq2;


	/*
	 * Inicializa las tablas del tropismo.
	 * Ver 'Evaluation Function Draft' de 'www.chessprogramming.org' en 'Home/Evaluation/Evaluation Function Draft'
	 */
	int	diag_NW[64]			= {0,1,2,3,4,5,6,7,1,2,3,4,5,6,7,8,2,3,4,5,6,7,8,9,3,4,5,6,7,8,9,10,4,5,6,7,8,9,10,11,5,6,7,8,9,10,11,12,6,7,8,9,10,11,12,13,7,8,9,10,11,12,13,14,},
		diag_NE[64]			= {7,6,5,4,3,2,1,0,8,7,6,5,4,3,2,1,9,8,7,6,5,4,3,2,10,9,8,7,6,5,4,3,11,10,9,8,7,6,5,4,12,11,10,9,8,7,6,5,13,12,11,10,9,8,7,6,14,13,12,11,10,9,8,7,},
		bonus_distance[15]	= {5, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


	SQ_LOOP(sq1)
		SQ_LOOP(sq2)			{
			Tropism[sq1][sq2]		= 14 - (abs(FILE(sq1) - FILE(sq2)) + abs(RANK(sq1) - RANK(sq2)));
			BishopTropism[sq1][sq2]	= bonus_distance[abs(diag_NE[sq1] - diag_NE[sq2])] + bonus_distance[abs(diag_NW[sq1] - diag_NW[sq2])];
		}
}


void	PutAttack(TURN side, BITBOARD bb, int king_att_weight)	{
	PieceAttBB[side]	|= bb;
	bb					&= KING_ZONE[FLIP_TURN(side)];

	if (king_att_weight && bb)	{
		AttackWeight[side]	+= king_att_weight * PopCountBB(bb);
		AttackCount[side]	+= MIN(4, king_att_weight);			// Q = 4, R = 3, B = 2, N = 2, P = 1
	}
}


// 1300
