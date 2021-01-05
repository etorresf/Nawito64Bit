/*
 * < Nawito is a chess engine winboard derived of Danasah507>
 * Copyright (C) <2020> <Ernesto Torres Feliciano>
 *
 * E-mail:		<ernesto2@nauta.cu>
 * Source:		https:*github.com/etorresf/Nawito
 * Web page:	*****************************
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
			WeakPieceBB[2],
			KING_ZONE[2];

int			Weight[32],
			*MidGameMat			= Weight + 0,
			*EndGameMat			= Weight + 2,
			*MidGameMob			= Weight + 4,
			*EndGameMob			= Weight + 6,
			*MidGamePawn		= Weight + 8,
			*EndGamePawn		= Weight + 10,
			*TrappedPiece		= Weight + 12,
			*KingSafety			= Weight + 14,
			
			*AttackWeight		= Weight + 24,
			*AttackCount		= Weight + 26,

			*PassedPawns		= Weight + 28,
			*IsolatedPawns		= Weight + 30;


// Evaluación de la posición con salida rápidas: lazy eval, tablas hash, final tablas
int		Eval(int Alpha, int Beta)				{
	int		Score, MatDiff, Draw;


	// Comprueba si la posición esta evaluada
	Score = ProbeScore(Hash);
	if (Score != -MATE)
		return Score;


	/*
	 * Si se tiene mucha ventaja o desventaja no hace falta seguir evaluando, cuanto menor es el valor de
	 * LazyScore más rápido sera el programa, con riesgo de dejar algún movimiento súper bueno
	 */
	MatDiff = PieceMat[Turn] + PawnMat[Turn] - PieceMat[FLIP_TURN(Turn)] - PawnMat[FLIP_TURN(Turn)];
	if (MatDiff - LazyScore[MatSum >> 6] >= Beta)
		return Beta;

	if (MatDiff + LazyScore[MatSum >> 6] <= Alpha)
		return Alpha;


	Score		= FullEval();

	// Ajusta evaluacion si final soportado por el motor
	Draw		= SupportedEndGames();
	if (Draw)
		Score	= (16 * Score) / Draw;


	StoreScore(Score, Hash);
	return Score;
}

// Evaluación completa de la posicion
int		FullEval(void)			{
	int		MidGame, EndGame;
	TURN	Cturn		= FLIP_TURN(Turn);


	GamePhase			= (256 * MIN(24, ALL_KNIGHTS + ALL_BISHOPS + 2 * ALL_ROOKS + 4 * ALL_QUEENS)) / 24;

	// Inicializando datos de la evaluacion
	memset(Weight,	0, sizeof(Weight));

	PawnAttBB[BLACK]	= ((BLACK_PAWN_BB & (~FILE_A_BB)) >> 9) | ((BLACK_PAWN_BB & (~FILE_H_BB)) >> 7);
	PawnAttBB[WHITE]	= ((WHITE_PAWN_BB & (~FILE_A_BB)) << 7) | ((WHITE_PAWN_BB & (~FILE_H_BB)) << 9);
	PieceAttBB[BLACK]	= PieceAttBB[WHITE] = 0;
	KingAttBB[BLACK]	= KING_MOVES_BB(KING_SQ(BLACK));
	KingAttBB[WHITE]	= KING_MOVES_BB(KING_SQ(WHITE));
	KING_ZONE[BLACK]	= WHITE_QUEENS ? KingZoneBB[BLACK][KING_SQ(BLACK)] : 0;
	KING_ZONE[WHITE]	= BLACK_QUEENS ? KingZoneBB[WHITE][KING_SQ(WHITE)] : 0;
	WeakPieceBB[BLACK]	= AllPieceBB[BLACK] & ~PawnAttBB[BLACK];
	WeakPieceBB[WHITE]	= AllPieceBB[WHITE] & ~PawnAttBB[WHITE];


	if (GamePhase < 64)			{
		MaterialEvalEnd();
		KnightEvalEnd();
		BishopEvalEnd();
		RookEvalEnd();
		QueenEvalEnd();
		PawnEvalEnd();
		KingEvalEnd();

		return	EndGameMat[Turn]	+ EndGameMob[Turn]	+ EndGamePawn[Turn]	+ TrappedPiece[Turn] 
				- EndGameMat[Cturn]	- EndGameMob[Cturn]	- EndGamePawn[Cturn]- TrappedPiece[Cturn]
				+ Tempo;
	}


	MaterialEval();
	KnightEval();
	BishopEval();
	RookEval();
	QueenEval();
	PawnEval();
	KingEval();


	// Recolectando valares de evaluacion
	MidGame	= MidGameMat[Turn]  + MidGameMob[Turn]  + MidGamePawn[Turn]
			- MidGameMat[Cturn] - MidGameMob[Cturn] - MidGamePawn[Cturn];

	EndGame = EndGameMat[Turn]  + EndGameMob[Turn]  + EndGamePawn[Turn]
			- EndGameMat[Cturn] - EndGameMob[Cturn] - EndGamePawn[Cturn];


	// Interpolando la evaluación entre el mediojuego y final
	return (MidGame * GamePhase + EndGame * (256 - GamePhase)) / 256 + Tempo
	+ TrappedPiece[Turn]  + KingSafety[Turn]
	- TrappedPiece[Cturn] - KingSafety[Cturn];
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
void	PawnEval(void)			{
	int			Opened, View, Backward, Isolated, Passed, PassedWeight, Candidate, Count, Doubled, Advanced, Support, AttSupport;
	TURN		Side, Cside;
	SQUARE		Square, RelSQ, To;
	BITBOARD	BB, PAWN_SIDE_BB, PAWN_CSIDE_BB;


	TURN_LOOP(Side)			{
		Cside			= FLIP_TURN(Side);
		View			= Side ? 56 : 0;
		PAWN_CSIDE_BB	= PAWN_BB(Cside);
		PAWN_SIDE_BB	= PAWN_BB(Side);


		// Peones bloqueados en C2, D2, E2
		if (MATCH(PAWN | Side, C7 ^ View) && MATCH(KNIGHT | Side, C6 ^ View) && MATCH(PAWN | Side, D5 ^ View) && NO_MATCH(PAWN | Side, E5 ^ View))
			MidGameMob[Side]	-= BlockedPawnC2;

		if (MATCH(PAWN | Side, E7 ^ View) && MATCH(BISHOP | Side, F8 ^ View) && NO_EMPTY(E6 ^ View))
			MidGameMob[Side]	-= BlockedCentralPawn;

		if (MATCH(PAWN | Side, D7 ^ View) && MATCH(BISHOP | Side, C8 ^ View) && NO_EMPTY(D6 ^ View))
			MidGameMob[Side]	-= BlockedCentralPawn;


		for (BB = PAWN_SIDE_BB; BB; BB ^= SquareBB[Square])		{
			Square		= LSB_BB(BB);
			RelSQ		= Square ^ View;
			Opened		= (ForwardBB[Side][Square] & PAWN_CSIDE_BB) == 0;
			Doubled		= 2 * ((ForwardBB[Cside][Square] & PAWN_SIDE_BB) != 0);
			Isolated	= !(FileBB[Square] & PawnAttBB[Side]);
			Passed		= !(PawnPassedBB[Side][Square] & PAWN_CSIDE_BB);
			Backward	= !(Passed || Isolated) && IsPawnBackward(Square);
			Advanced	= !(Passed || Isolated || Backward) && IsPawnAdvanced(Square);
			Candidate	= !(Backward || Isolated || Passed || Advanced) && Opened && IsPawnCandidate(Square);
			Support		= (ForwardBB[Cside][Square] & ROOK_BB(Side)) != 0;
			AttSupport	= (ForwardBB[Cside][Square] & ROOK_BB(Cside)) != 0;
			PassedWeight= (2 + 2 * Support - AttSupport) * (Candidate + 2 * Passed);


			// PST
			MidGameMat[Side]	+= PawnMidGamePST[RelSQ];
			EndGameMat[Side]	+= PawnEndGamePST[RelSQ];

			// Peón doblado
			if (Doubled)		{
				if (Isolated && (PawnPassedBB[Side][Square] & PAWN_CSIDE_BB) && (PopCountBB(PawnPassedBB[Side][Square] & PAWN_CSIDE_BB) <= PopCountBB(ForwardBB[Cside][Square] & PAWN_SIDE_BB)))
					Doubled++;

				MidGamePawn[Side]	-= (PawnDoubled[RelSQ] * Doubled) / 2;
				EndGamePawn[Side]	-= PawnDoubled[RelSQ]  * Doubled;
			}

			// Peón Aislado
			if (Isolated)		{
				IsolatedPawns[Side]++;
				MidGamePawn[Side]	-= PawnIsolated[RelSQ] * (1 + Opened);
				EndGamePawn[Side]	-= PawnIsolated[RelSQ] * 2;
			}

			// Peón retrasado. No pasado, aislado
			if (Backward)		{
				MidGamePawn[Side]	-= PawnBackward[RelSQ]  * (1 + Opened);
				EndGamePawn[Side]	-= (PawnBackward[RelSQ] * 3) / 2;
			}

			if (Advanced)		{
				MidGamePawn[Side]	-= PawnBackward[Square]  * (1 + Opened);
				EndGamePawn[Side]	-= (PawnBackward[Square] * 3) / 2;
			}

			// Peón Pasado
			if (Passed)			{
				PassedPawns[Side]++;
				To			= Square + Front[Side];

				EndGamePawn[Side]	+= PassedPawnPotential[RelSQ] * (4 * Distance[To][KING_SQ(Cside)] - Distance[To][KING_SQ(Side)]) / 2;

				// Los peones pasados protegidos son mas peligrosos
				if (PawnAttBB[Side] & (SquareBB[Square] | SquareBB[Square + Front[Side]]))
					PassedWeight++;

				// El rey enemigo no puede alcanzarlo
				if (!PieceMat[Cside])			{
					To = LSB_BB(ForwardBB[Side][Square] & (RANK_1_BB | RANK_8_BB));

					if (Distance[Square][To] < Distance[KING_SQ(Cside)][To] + (Turn == Side))
						EndGamePawn[Side] += PassedPawnPotential[RelSQ] * 80;
				}

				// Determina que tan peligroso es un peón pasado
				for (To = Square + Front[Side], Count = 3; Count; To += Front[Side], Count--)	{
					if (A1 <= To && To <= H8)	{
						if ((AttSupport || EV_ATTACKED(To, Cside)) && !(EV_ATTACKED(To, Side) || Support))
							break;

						if (Board[To])			{
							EndGamePawn[Side] += IsPawnPassedBloqued(To, Square);
							break;
						}
					}

					// Peón pasado peligroso, puede avanzar libremente
					EndGamePawn[Side] += PassedPawnPotential[RelSQ] * Count;
				}
			}

			if (PassedWeight)	{
				MidGamePawn[Side]	+= (PassedWeight * PawnPassed[RelSQ]) / 4;
				EndGamePawn[Side]	+= (PassedWeight * PawnPassed[RelSQ]) / 2;
			}
		}
	}
}

void	PawnEvalEnd(void)		{
	int			Opened, View, Backward, Isolated, Doubled, Passed, PassedWeight, Candidate, Advanced, Count, Support, AttSupport;
	TURN		Side, Cside;
	SQUARE		Square, RelSQ, To;
	BITBOARD	BB, PAWN_SIDE_BB, PAWN_CSIDE_BB;


	TURN_LOOP(Side)			{
		Cside			= FLIP_TURN(Side);
		View			= Side ? 56 : 0;
		PAWN_CSIDE_BB	= PAWN_BB(Cside);
		PAWN_SIDE_BB	= PAWN_BB(Side);


		for (BB = PAWN_SIDE_BB; BB; BB ^= SquareBB[Square])		{
			Square		= LSB_BB(BB);
			RelSQ		= Square ^ View;
			Opened		= (ForwardBB[Side][Square] & PAWN_CSIDE_BB) == 0;
			Doubled		= 2 * ((ForwardBB[Cside][Square] & PAWN_SIDE_BB) != 0);
			Isolated	= !(FileBB[Square] & PawnAttBB[Side]);
			Passed		= !(PawnPassedBB[Side][Square] & PAWN_CSIDE_BB);
			Backward	= !(Passed || Isolated) && IsPawnBackward(Square);
			Advanced	= !(Passed || Isolated || Backward) && IsPawnAdvanced(Square);
			Candidate	= !(Backward || Isolated || Passed || Advanced) && Opened && IsPawnCandidate(Square);
			Support		= (ForwardBB[Cside][Square] & ROOK_BB(Side)) != 0;
			AttSupport	= (ForwardBB[Cside][Square] & ROOK_BB(Cside)) != 0;
			PassedWeight= (2 + 2 * Support - AttSupport) * (Candidate + 2 * Passed);


			// PST
			EndGameMat[Side]	+= PawnEndGamePST[RelSQ];

			// Peón doblado, penaliza segun la cantidad de peones doblados y donde estan
			if (Doubled)		{
				if (Isolated && (PawnPassedBB[Side][Square] & PAWN_CSIDE_BB) && (PopCountBB(PawnPassedBB[Side][Square] & PAWN_CSIDE_BB) <= PopCountBB(ForwardBB[Cside][Square] & PAWN_SIDE_BB)))
					Doubled++;

				EndGamePawn[Side]	-= PawnDoubled[RelSQ] * Doubled;
			}

			// Peón Aislado
			if (Isolated)		{
				IsolatedPawns[Side]++;
				EndGamePawn[Side]	-= PawnIsolated[RelSQ] * 2;
			}

			// Peón retrasado. No pasado, aislado
			if (Backward)
				EndGamePawn[Side]	-= (PawnBackward[RelSQ] * 3) / 2;

			if (Advanced)
				EndGamePawn[Side]	-= (PawnBackward[Square] * 3) / 2;

			// Peón Pasado
			if (Passed)		{
				PassedPawns[Side]++;
				To			= Square + Front[Side];

				EndGamePawn[Side]	+= PassedPawnPotential[RelSQ] * (4 * Distance[To][KING_SQ(Cside)] - Distance[To][KING_SQ(Side)]) / 2;

				// Los peones pasados protegidos son mas peligrosos
				if (PawnAttBB[Side] & (SquareBB[Square] | SquareBB[Square + Front[Side]]))
					PassedWeight++;

				// Peón pasado peligroso, ni el rey o piezas enemigas pueden alcanzarlo
				if (!PieceMat[Cside])			{
					To = LSB_BB(ForwardBB[Side][Square] & (RANK_1_BB | RANK_8_BB));

					if (Distance[Square][To] < Distance[KING_SQ(Cside)][To] + (Turn == Side))
						EndGamePawn[Side] += PassedPawnPotential[RelSQ] * 80;
				}

				// Determina que tan peligroso es un peón pasado
				for (To = Square + Front[Side], Count = 3; Count; To += Front[Side], Count--)	{
					if (A1 <= To && To <= H8)	{
						if ((AttSupport || EV_ATTACKED(To, Cside)) && !(EV_ATTACKED(To, Side) || Support))
							break;

						if (Board[To])			{
							EndGamePawn[Side] += IsPawnPassedBloqued(To, Square);
							break;
						}
					}

					// Peón pasado peligroso, puede avanzar libremente
					EndGamePawn[Side] += PassedPawnPotential[RelSQ] * Count;
				}
			}

			if (PassedWeight)
				EndGamePawn[Side]	+= (PassedWeight * PawnPassed[RelSQ]) / 2;
		}
	}
}


/*
 * Evalúa Caballos por:
 *
 * 1) Material
 *	->	Material
 *	->	PST
 *	->	Ajuste del material según el numero de peones
 * 2) Movilidad
 *	->	Movilidad
 *	->	Soportado por peones
 *	->	Atacando a peones y piezas débiles
 *	->	Apostado
 *	->	Atrapado
 */
void	KnightEval(void)		{
	int			PawnAttack, Mob, View;
	TURN		Side, Cside;
	SQUARE		Square, RelSQ;
	BITBOARD	BB, MobBB, PAWN_CSIDE_BB;


	TURN_LOOP(Side)
		if (KNIGHTS(Side))		{
			Cside			= FLIP_TURN(Side);
			View			= Side ? 56 : 0;
			PAWN_CSIDE_BB	= PAWN_BB(Cside);


			for (BB = KNIGHT_BB(Side); BB; BB ^= SquareBB[Square])				{
				Square		= LSB_BB(BB);
				RelSQ		= Square ^ View;
				PawnAttack	= PopCountBB(PAWN_ATTACK_BB(Square, Side));


				MidGameMat[Side]	+= KnightMidGamePST[RelSQ];
				EndGameMat[Side]	+= KnightEndGamePST[RelSQ];


				/*
				 * 1) No se consideran los movimientos hacia las esquinas del tablero
				 * 2) Se da mas valor a movimiento hacia el centro del tablero(4 ptos), a los restantes(2 ptos)
				 * 3) No se consideran las casillas atacadas por peones enemigos
				 */
				MobBB	= KNIGHT_MOVES_BB(Square);
				PutAttack(Side, MobBB, 2);
				MobBB	&= ~(PawnAttBB[Cside] | AllPieceBB[Side]);
				Mob		= 4 * PopCountBB(MobBB & KNIGHT_IMP4) + 2 * PopCountBB(MobBB & KNIGHT_IMP2);

				MidGameMob[Side]	+= KnightMob[Mob] + 2 * (PawnAttack + PopCountBB(MobBB & WeakPieceBB[Cside]) + PopCountBB(MobBB & PAWN_CSIDE_BB));
				EndGameMob[Side]	+= KnightMob[Mob];

				// Apostado, no es posible atacarlo con peones, está defendido por peones y en casillas centrales
				if (PawnAttack && KnightOutpost[RelSQ] && !(OutpostBB[Side][Square] & PAWN_CSIDE_BB))
					MidGameMob[Side]+= PawnAttack * KnightOutpost[RelSQ];


				// Atrapado, en esquina del tablero y casillas de salidas atacadas por peones. Idea del motor CPW
				if (Mob < 4)
					switch (RelSQ)	{
						case A1: case H1:	TrappedPiece[Side]	-= KnightTrappedA8;	break;
						case A2: case H2:	TrappedPiece[Side]	-= KnightTrappedA7;	break;
						default:;
					}


				KingSafety[Cside]	-= KNIGHT_TROPISM(Square, KING_SQ(Cside));
			}
		}
}

void	KnightEvalEnd(void)		{
	int			Mob, View;
	TURN		Side, Cside;
	SQUARE		Square, RelSQ;
	BITBOARD	BB, MobBB;


	TURN_LOOP(Side)
		if (KNIGHTS(Side))		{
			Cside			= FLIP_TURN(Side);
			View			= Side ? 56 : 0;


			for (BB = KNIGHT_BB(Side); BB; BB ^= SquareBB[Square])				{
				Square		= LSB_BB(BB);
				RelSQ		= Square ^ View;


				EndGameMat[Side] += KnightEndGamePST[RelSQ];

				/*
				 * 1) No se consideran los movimientos hacia las esquinas del tablero
				 * 2) Se da mas valor a movimiento hacia el centro del tablero(4 ptos), a los restantes(2 ptos)
				 * 3) No se consideran las casillas atacadas por peones enemigos
				 */
				MobBB	= KNIGHT_MOVES_BB(Square);
				PutAttack(Side, MobBB, 0);
				MobBB	&= ~(PawnAttBB[Cside] | AllPieceBB[Side]);
				Mob		= 4 * PopCountBB(MobBB & KNIGHT_IMP4) + 2 * PopCountBB(MobBB & KNIGHT_IMP2);

				EndGameMob[Side] += KnightMob[Mob];

				// Atrapado, en esquina del tablero y casillas de salidas atacadas por peones. Idea del motor CPW
				if (Mob < 4)
					switch (RelSQ)				{
						case A1: case H1:	TrappedPiece[Side]	-= KnightTrappedA8;	break;
						case A2: case H2:	TrappedPiece[Side]	-= KnightTrappedA7;	break;
						default:;
					}
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
void	BishopEval(void)		{
	int			TwoBishop, Mob, View;
	TURN		Side, Cside;
	SQUARE		Square, RelSQ;
	BITBOARD	BB, MobBB, PAWN_SIDE_BB, PAWN_CSIDE_BB, BISHOP_SIDE_BB,
				AllBB		= ALL_PIECES_BB;


	TURN_LOOP(Side)
		if (BISHOPS(Side))		{
			Cside			= FLIP_TURN(Side);
			View			= Side ? 56 : 0;
			PAWN_SIDE_BB	= PAWN_BB(Side);
			PAWN_CSIDE_BB	= PAWN_BB(Cside);
			BISHOP_SIDE_BB	= BISHOP_BB(Side);
			TwoBishop		= (BISHOP_SIDE_BB & WHITE_SQUARES_BB) && (BISHOP_SIDE_BB & BLACK_SQUARES_BB);

			MidGameMob[Side]+= 2 * PopCountBB(PawnAttBB[Side] & BISHOPS(Side));


			for (BB = BISHOP_SIDE_BB; BB; BB ^= SquareBB[Square])				{
				Square		= LSB_BB(BB);
				RelSQ		= Square ^ View;


				MidGameMat[Side]	+= BishopMidGamePST[RelSQ];
				EndGameMat[Side]	+= BishopEndGamePST[RelSQ];


				/**/
				MobBB	= BISHOP_MOVES_BB(Square, AllBB);
				Mob		= PopCountBB(MobBB);
				PutAttack(Side, MobBB, 2);

				MidGameMob[Side]	+= BishopMob[Mob] + 2 * (PopCountBB(MobBB & WeakPieceBB[Cside]) + PopCountBB(MobBB & WeakPieceBB[Cside] & PAWN_CSIDE_BB));
				EndGameMob[Side]	+= BishopMob[Mob];


				/*
				 * Alfil malo, depende de:
				 *
				 * 1) El numero de peones del bando del alfil
				 * 2) Si tiene suficiente movilidad no es considerado como malo
				 */
				if (!TwoBishop && Mob < 8)		{
					if (BISHOP_SIDE_BB & WHITE_SQUARES_BB)
						EndGameMob[Side]	-= BadBishop[PopCountBB(PAWN_SIDE_BB & WHITE_SQUARES_BB)];

					else EndGameMob[Side]	-= BadBishop[PopCountBB(PAWN_SIDE_BB & BLACK_SQUARES_BB)];
				}

				// Atrapado, en esquina del tablero y peones enemigos bloqueando las salidas. Idea del motor CPW
				if (Mob < 4)
					switch (RelSQ)				{
						case A2: case H2:	TrappedPiece[Side]	-= BishopTrappedA7;	break;
						case A3: case H3:	TrappedPiece[Side]	-= BishopTrappedA6;	break;
						case B1: case G1:	TrappedPiece[Side]	-= BishopTrappedB8;	break;
						default:;
					}


				KingSafety[Cside]	-= BISHOP_TROPISM(Square, KING_SQ(Cside));
			}
		}
}

void	BishopEvalEnd(void)		{
	int			TwoBishop, Mob, View;
	TURN		Side;
	SQUARE		Square, RelSQ;
	BITBOARD	BB, PAWN_SIDE_BB, BISHOP_SIDE_BB, MobBB,
				AllBB		= ALL_PIECES_BB,
				EmptyBB		= ~AllBB;


	TURN_LOOP(Side)
		if (BISHOPS(Side))		{
			View			= Side ? 56 : 0;
			PAWN_SIDE_BB	= PAWN_BB(Side);
			BISHOP_SIDE_BB	= BISHOP_BB(Side);
			TwoBishop		= (BISHOP_SIDE_BB & WHITE_SQUARES_BB) && (BISHOP_SIDE_BB & BLACK_SQUARES_BB);


			for (BB = BISHOP_SIDE_BB; BB; BB ^= SquareBB[Square])				{
				Square		= LSB_BB(BB);
				RelSQ		= Square ^ View;


				EndGameMat[Side] += BishopEndGamePST[RelSQ];

				/**/
				MobBB	= BISHOP_MOVES_BB(Square, AllBB);
				Mob		= PopCountBB(MobBB & EmptyBB);
				PutAttack(Side, MobBB, 0);

				EndGameMob[Side] += BishopMob[Mob];

				/*
				 * Alfil malo, depende de:
				 *
				 * 1) El numero de peones del bando del alfil
				 * 2) Si tiene suficiente movilidad no es considerado como malo
				 */
				if (!TwoBishop && Mob < 8)		{
					if (BISHOP_SIDE_BB & WHITE_SQUARES_BB)
						EndGameMob[Side]	-= BadBishop[PopCountBB(PAWN_SIDE_BB & WHITE_SQUARES_BB)];

					else EndGameMob[Side]	-= BadBishop[PopCountBB(PAWN_SIDE_BB & BLACK_SQUARES_BB)];
				}

				// Atrapado, en esquina del tablero y peones enemigos bloqueando las salidas. Idea del motor CPW
				if (Mob < 4)
					switch (RelSQ)				{
						case A2: case H2:	TrappedPiece[Side]	-= BishopTrappedA7;	break;
						case A3: case H3:	TrappedPiece[Side]	-= BishopTrappedA6;	break;
						case B1: case G1:	TrappedPiece[Side]	-= BishopTrappedB8;	break;
						default:;
					}
				}
		}
}


/*
 * Evalúa Torres por:
 *
 * 1) Material
 *	->	Material
 *	->	PST
 *	->	Ajuste del material según el numero de peones
 * 2) Movilidad
 *	->	Movilidad
 *	->	Atacando a peones y piezas débiles
 *	->	Torre en columna abierta
 *	->	Torre doblada en columna abierta
 *	->	Torre en columna semi-abierta
 *	->	Torre en 7ma fila
 *	->	Atrapada
 */
void	RookEval(void)			{
	int			Mob, View;
	TURN		Side, Cside;
	SQUARE		Square, RelSQ;
	BITBOARD	BB, MobBB, PAWN_CSIDE_BB, PAWN_SIDE_BB,
				AllBB		= ALL_PIECES_BB,
				EmptyBB		= ~AllBB;


	TURN_LOOP(Side)
		if (ROOKS(Side))		{
			Cside			= FLIP_TURN(Side);
			View			= Side ? 56 : 0;
			PAWN_CSIDE_BB	= PAWN_BB(Cside);
			PAWN_SIDE_BB	= PAWN_BB(Side);


			for (BB = ROOK_BB(Side); BB; BB ^= SquareBB[Square])				{
				Square	= LSB_BB(BB);
				RelSQ	= Square ^ View;


				MidGameMat[Side]	+= RookMidGamePST[RelSQ];
				EndGameMat[Side]	+= RookEndGamePST[RelSQ];


				/*
				 * 1) Se da mas valor a movimiento en columnas (2 ptos), a los restantes(1 ptos)
				 */
				MobBB	= ROOK_MOVES_BB(Square, AllBB);
				PutAttack(Side, MobBB, 3);
				Mob		= 2 * PopCountBB(MobBB & FileBB[Square] & EmptyBB) + PopCountBB(MobBB & RankBB[Square] & EmptyBB);

				MidGameMob[Side]	+= RookMob[Mob] + 2 * (PopCountBB(MobBB & WeakPieceBB[Cside]) + PopCountBB(MobBB & WeakPieceBB[Cside] & PAWN_CSIDE_BB));
				EndGameMob[Side]	+= RookMob[Mob] * 2;

				if (!(FileBB[Square] & PAWN_SIDE_BB))			{
					if (FileBB[Square] & PAWN_CSIDE_BB)			{
						// Torre en columna semi-abierta
						MidGameMob[Side]	+= RookInHalfOpenFile;
						EndGameMob[Side]	+= RookInHalfOpenFile;

						// Atacando al Rey enemigo
						if (abs(FILE(KING_SQ(Cside)) - FILE(Square)) < 2)
							AttackWeight[Side] += 2;
					}

					else		{
						// Torre en columna abierta
						MidGameMob[Side]	+= RookInOpenFile;
						EndGameMob[Side]	+= RookInOpenFile;

						// Atacando al Rey enemigo
						if (abs(FILE(KING_SQ(Cside)) - FILE(Square)) < 2)
							AttackWeight[Side]++;

						// Torres y Damas dobladas en columna abierta
						if (MobBB & FileBB[Square] & ROOK_BB(Side))
							MidGameMob[Side]	+= RookSuppRook;

						if (MobBB & FileBB[Square] & QUEEN_BB(Side))
							MidGameMob[Side]	+= QueenSuppRook;
					}
				}

				// Torre en 7ma fila si peones enemigos en 7ma o Rey enemigo en 8va
				if (RANK(RelSQ) == RANK_2 && ((RankBB[Square] & PAWN_CSIDE_BB) || (RankBB[Square + Front[Side]] & KING_BB(Cside))))				{
					MidGameMob[Side]	+= RookIn7th;
					EndGameMob[Side]	+= RookIn7th * 2;
				}

				// Atrapada, en esquina del tablero y peones amigos bloqueando las salidas. Idea del motor CPW
				if (Mob < 6)
					switch (KING_SQ(Side) ^ View)				{
						case F8: case G8:
							if (RelSQ == H8 || RelSQ == H7 || RelSQ == G8)		{
								TrappedPiece[Side]	-= RookTrapped;
								MidGameMob[Side]	-= CasttleDone;
							}
							break;
						case C8: case B8:
							if (RelSQ == A8 || RelSQ == A7 || RelSQ == B8)		{
								TrappedPiece[Side]	-= RookTrapped;
								MidGameMob[Side]	-= CasttleDone;
							}
							break;
						case H8: case A8: case G7: case B7:
							if (Square == KING_SQ(Side) + Front[Side])			{
								TrappedPiece[Side]	-= RookTrapped;
								MidGameMob[Side]	-= CasttleDone;
							}
							break;
						default:;
					}


				KingSafety[Cside]	-= ROOK_TROPISM(Square, KING_SQ(Cside));
			}
		}
}

void	RookEvalEnd(void)		{
	int			Mob, View;
	TURN		Side, Cside;
	SQUARE		Square, RelSQ;
	BITBOARD	BB, MobBB, PAWN_CSIDE_BB,
				AllBB		= ALL_PIECES_BB,
				EmptyBB		= ~AllBB;


	TURN_LOOP(Side)
		if (ROOKS(Side))		{
			Cside			= FLIP_TURN(Side);
			View			= Side ? 56 : 0;
			PAWN_CSIDE_BB	= PAWN_BB(Cside);


			for (BB = ROOK_BB(Side); BB; BB ^= SquareBB[Square])				{
				Square	= LSB_BB(BB);
				RelSQ	= Square ^ View;


				EndGameMat[Side]	+= RookEndGamePST[RelSQ];

				/*
				 * 1) Se da mas valor a movimiento en columnas (2 ptos), a los restantes(1 ptos)
				 */
				MobBB	= ROOK_MOVES_BB(Square, AllBB);
				PutAttack(Side, MobBB, 0);
				Mob		= 2 * PopCountBB(MobBB & FileBB[Square] & EmptyBB) + PopCountBB(MobBB & RankBB[Square] & EmptyBB);

				EndGameMob[Side]	+= RookMob[Mob] * 2;

				// Torre en 7ma fila si peones enemigos en 7ma o Rey enemigo en 8va
				if (RANK(RelSQ) == RANK_2 && ((RankBB[Square] & PAWN_CSIDE_BB) || (RankBB[Square + Front[Side]] & KING_BB(Cside))))
					EndGameMob[Side]	+= RookIn7th * 2;
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
void	QueenEval(void)			{
	int			View, Mob;
	TURN		Side, Cside;
	SQUARE		Square, RelSQ;
	BITBOARD	BB, MobBB,
				AllBB		= ALL_PIECES_BB,
				EmptyBB		= ~AllBB;


	TURN_LOOP(Side)
		if (QUEENS(Side))		{
			Cside	= FLIP_TURN(Side);
			View	= Side ? 56 : 0;


			for (BB = QUEEN_BB(Side); BB; BB ^= SquareBB[Square])				{
				Square	= LSB_BB(BB);
				RelSQ	= Square ^ View;


				MidGameMat[Side]	+= QueenMidGamePST[RelSQ];
				EndGameMat[Side]	+= QueenEndGamePST[RelSQ];


				// Calculando la movilidad, solo se consideran las casillas vacias
				MobBB	= QUEEN_MOVES_BB(Square, AllBB);
				PutAttack(Side, MobBB, 5);
				Mob		= PopCountBB(MobBB & EmptyBB);

				MidGameMob[Side]	+= QueenMob[Mob];
				EndGameMob[Side]	+= QueenMob[Mob] * 2;

				// Dama prematura
				if (RelSQ != D8 && (MATCH(KNIGHT | Side, B8 ^ View) || MATCH(BISHOP | Side, C8 ^ View) || MATCH(KNIGHT | Side, G8 ^ View) || MATCH(BISHOP | Side, F8 ^ View)))
					MidGameMob[Side] -= QueenPremature;


				KingSafety[Cside]	-= QUEEN_TROPISM(Square, KING_SQ(Cside));
			}
		}
}

void	QueenEvalEnd(void)		{
	int			View, Mob;
	TURN		Side;
	SQUARE		Square, RelSQ;
	BITBOARD	BB, MobBB,
				AllBB		= ALL_PIECES_BB,
				EmptyBB		= ~AllBB;


	TURN_LOOP(Side)
		if (QUEENS(Side))		{
			View	= Side ? 56 : 0;


			for (BB = QUEEN_BB(Side); BB; BB ^= SquareBB[Square])				{
				Square	= LSB_BB(BB);
				RelSQ	= Square ^ View;


				EndGameMat[Side]	+= QueenEndGamePST[RelSQ];

				// Calculando la movilidad, solo se consideran las casillas vacias
				MobBB	= QUEEN_MOVES_BB(Square, AllBB);
				PutAttack(Side, MobBB, 0);
				Mob		= PopCountBB(MobBB & EmptyBB);

				EndGameMob[Side]	+= QueenMob[Mob] * 2;
			}
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
void	KingEval(void)			{
	int			Mob, View, File, AttackScore, Score, Piece;
	TURN		Side, Cside;
	SQUARE		Square, RelSQ;
	BITBOARD	BB, BB2, MobBB = 0, KingFront, PAWN_CSIDE_BB;


	TURN_LOOP(Side)				{
		Cside			= FLIP_TURN(Side);
		View			= Side ? 56 : 0;
		Square			= KING_SQ(Side);
		RelSQ			= Square ^ View;
		File			= FILE(Square);
		AttackScore		= 0;
		PAWN_CSIDE_BB	= PAWN_BB(Cside);


		// Material y PST
		Mob					= PopCountBB(KING_MOVES_BB(Square) & WeakPieceBB[Cside] & PAWN_CSIDE_BB);
		MidGameMob[Side]	+= KingMidGamePST[RelSQ];
		EndGameMob[Side]	+= KingEndGamePST[RelSQ] + Mob * KingEndGameMob;


		// Escudo del Rey
		switch (File)			{
			case FILE_C: case FILE_B: case FILE_A:				// Enroque largo
				Piece	= Board[B7] ^ Side;
				Score	= KingPawnShield(A7 ^ View, Side) + KingPawnShield(B7 ^ View, Side) + KingPawnShield(C7 ^ View, Side) / 2;
				break;

			case FILE_G: case FILE_H:							// Enroque corto
				Piece	= Board[G7] ^ Side;
				Score	= KingPawnShield(H7 ^ View, Side) + KingPawnShield(G7 ^ View, Side) + KingPawnShield(F7 ^ View, Side) / 2;
				break;

			default:											// Rey en centro del tablero
				Piece	= EMPTY_PIECE;
				BB		= BLACK_PAWN_BB | WHITE_PAWN_BB;
				Score	= KingInOpenFile * (!(FileBB[Square] & BB) + !(FileBB[Square - 1] & BB) + !(FileBB[Square + 1] & BB));
		}

		// Enroques opuestos
		if (abs(File - FILE(KING_SQ(Cside))) > 2)
			Score *= 2;

		KingSafety[Side] -= Score;


		switch (Piece)	{
			case EMPTY_PIECE:									// Pérdida del enroque
				if (QUEENS(Cside) && !((KingSideCasttle[Side] | QueenSideCasttle[Side]) & Casttle))
					MidGameMob[Side]		-= CasttleDone;
				break;

			// Fianchetto
			case PAWN: case KING:	break;
			case BISHOP:			MidGameMob[Side] += Fianchetto; break;
			default:				MidGameMob[Side] -= Fianchetto;
		}


		/*
		 * Ataque de piezas enemigas al rey.
		 * Es una mezcla de la seguridad del rey de los motores Rebel y Stockfish.
		 * Tiene en cuenta la cantidad de ataques de piezas y su tipo de piezas a casillas cercanas al Rey(Stockfish).
		 * Tambien analiza si esas casillas estan defendidas o ucupadas por piezas del mismo bando o del
		 * enemigo o si no hay peones en esas casillas(Rebel).
		 */

		if (AttackCount[Cside] > 2)				{
			// Penalizar, si Rey en jaque o en centro del tablero, casillas atacadas por peones enimigos(quizas ignorar si son peones bloquados por otros peones)
			AttackScore	= AttackWeight[Cside] + SRKingCount[RelSQ] + PopCountBB(KING_ZONE[Side] & PawnAttBB[Cside]);
			KingFront	= KingFrontBB[Side][Square];


			// Penalizar, piezas enemigas ocupando las casillas frente al rey o esta vacia.
			// Duplicar su valor si esta atacada
			BB	= KingFront & ~AllPieceBB[Side];
			BB2	= EV_ATTACKED_BB(Cside);
			AttackScore += 2 * PopCountBB(BB & BB2) + PopCountBB(BB & ~BB2);

			// Penalizar, Rey unico defensor de casillas atacadas alrededor del mismo
			AttackScore += PopCountBB(MobBB & BB2 & ~EV_ATTACKED_WITHOUT_KING_BB(Side));

			// Penalizar, casillas frente al rey sin peon, pero con pieza y atacada por el enemigo
			AttackScore += PopCountBB(KingFront & BB2 & (AllPieceBB[Side] & ~PAWN_BB(Side)));
		}

		KingSafety[Side] = (MIN(INITIAL_MATERIAL, PieceMat[Cside]) * KingSafety[Side]) / INITIAL_MATERIAL - SafetyTable[AttackScore];
	}
}

void	KingEvalEnd(void)		{
	int			View, Mob;
	TURN		Side, Cside;
	SQUARE		Square, RelSQ;

	TURN_LOOP(Side)				{
		Cside	= FLIP_TURN(Side);
		View	= Side ? 56 : 0;
		Square	= KING_SQ(Side);
		RelSQ	= Square ^ View;

		// Material y PST
		Mob					= PopCountBB(KING_MOVES_BB(Square) & WeakPieceBB[Cside] & PAWN_BB(Cside));
		EndGameMob[Side]	+= KingEndGamePST[RelSQ] + KingEndGameMob * Mob;
	}
}

// Evalua una de las columnas del escudo del Rey para un bando. Inspirado del motor TSCP, basicamente el mismo algoritmo
int		KingPawnShield(SQUARE Square, TURN Side){
	int		Score	= 0;
	TURN	Cside	= FLIP_TURN(Side);


	// Penalizacion, peon movido del escudo del Rey
	if (!(FileBB[Square] & PAWN_BB(Side)))
		// No hay peon en la columna
		Score		+= NoPawnInFile;

	else if (NO_MATCH(PAWN | Side, Square))		{
		// Peon movido 1 casilla
		if (MATCH(PAWN | Side, Square + Front[Side]))
			Score	+= Pawn3th;

		// Peon movido 2 o mas casillas
		else Score	+= Pawn4th;
	}


	// Penalizacion, avalancha hacia el Rey de peones enemigos
	if (!(FileBB[Square] & PAWN_BB(Cside)))
		// No hay peones enemigos en la columna
		Score	+= NoEnemyPawnInFile;

	// Peon enemigo en 3ra fila
	else if (MATCH(PAWN | Cside, Square + Front[Side]))
		Score	+= EnemyPawn3th;

	// Peon enemigo en 4ta fila
	else if (MATCH(PAWN | Cside, Square + 2 * Front[Side]))
		Score	+= EnemyPawn4th;


	return Score;
}


// Evaluacion de las constelaciones del material
void	MaterialEval(void)		{
	int		tmp, MatDiff, PawnSide, PawnCside;
	TURN	Side, Cside;

	// Salida rapida, ambos bandos tienen las mismas piezas y peones
	if (BLACK_PAWNS == WHITE_PAWNS && (HashMat & 0x1C71C700) == ((HashMat >> 3) & 0x1C71C700))
		return;


	// Algunos valores para el desvalance material segun Larry Kaufman
	TURN_LOOP (Side)			{
		Cside		= FLIP_TURN(Side);
		PawnSide	= MIN(8, PAWNS(Side));
		PawnCside	= MIN(8, PAWNS(Cside));
		tmp			= -PawnAdjust[PawnSide]
					+ KNIGHTS(Side)	* KnightAdjust[PawnSide]- KnightPair* (KNIGHTS(Side)> 1)
					+ ROOKS(Side)	* RookAdjust[PawnSide]	- RookPair	* (ROOKS(Side)	> 1);

		/*
		 * Pareja de alfiles, depende de:
		 *
		 * 1) El numero total de peones y donde están
		 * 2) Si el rival tiene piezas menores
		 */
		if ((BISHOP_BB(Side) & WHITE_SQUARES_BB) && (BISHOP_BB(Side) & BLACK_SQUARES_BB))		{
			tmp += BishopPair[PawnSide + PawnCside];

			if (!BISHOPS(Cside)){
				if (!KNIGHTS(Cside))
					tmp		+= 3 * (WithOutBishop >> 1);

				else tmp	+= WithOutBishop;
			}
		}

		MidGameMat[Side]	+= tmp;
		EndGameMat[Side]	+= tmp;
	}


	// Salida rapida, ambos bandos tienen las mismas piezas
	if ((HashMat & 0x1C71C700) == ((HashMat >> 3) & 0x1C71C700))
		return;

	MatDiff	= PieceMat[WHITE] - PieceMat[BLACK];
	Side	= WHITE;

	if (MatDiff < 0)			{
		MatDiff	= -MatDiff;
		Side	= BLACK;
	}


	// Mayor contra peones
	if (MatDiff >= ROOK_VALUE)	{
		MidGameMat[Side]	+= RookVSpawnsMid;
		EndGameMat[Side]	+= RookVSpawnsEnd;
	}

	// Menor contra peones
	else if (MatDiff >= KNIGHT_VALUE)			{
		MidGameMat[Side]	+= KnightVSpawnsMid;
		EndGameMat[Side]	+= KnightVSpawnsEnd;
	}

	// Menor contra torre
	else if (MatDiff >= ROOK_VALUE - KNIGHT_VALUE)				{
		MidGameMat[Side]	+= RookVSknightMid;
		EndGameMat[Side]	+= RookVSknightEnd;
	}

	else	{
		// Salida rapida, ambos bandos tienen la mismas cantidad de caballos y alfiles
		if ((HashMat & 0x1C700) == ((HashMat >> 3) & 0x1C700))
			return;

		MatDiff	= KNIGHTS(WHITE) + BISHOPS(WHITE) - KNIGHTS(BLACK) - BISHOPS(BLACK);
		Side	= WHITE;

		if (MatDiff < 0)		{
			MatDiff	= -MatDiff;
			Side	= BLACK;
		}

		// Dos menores contra torre
		if (MatDiff > 1)		{
			MidGameMat[Side]	+= KnightBishopVSrookMid;
			EndGameMat[Side]	+= KnightBishopVSrookEnd;
		}
	}
}

void	MaterialEvalEnd(void)	{
	int		tmp, MatDiff, PawnSide, PawnCside;
	TURN	Side, Cside;

	// Salida rapida, ambos bandos tienen las mismas piezas y peones
	if (BLACK_PAWNS == WHITE_PAWNS && (HashMat & 0x1C71C700) == ((HashMat >> 3) & 0x1C71C700))
		return;


	// Algunos valores para el desvalance material segun Larry Kaufman
	TURN_LOOP (Side)			{
		Cside		= FLIP_TURN(Side);
		PawnSide	= MIN(8, PAWNS(Side));
		PawnCside	= MIN(8, PAWNS(Cside));
		tmp			= -PawnAdjust[PawnSide]
					+ KNIGHTS(Side)	* KnightAdjust[PawnSide]- KnightPair* (KNIGHTS(Side)> 1)
					+ ROOKS(Side)	* RookAdjust[PawnSide]	- RookPair	* (ROOKS(Side)	> 1);

		/*
		 * Pareja de alfiles, depende de:
		 *
		 * 1) El numero total de peones y donde están
		 * 2) Si el rival tiene piezas menores
		 */
		if ((BISHOP_BB(Side) & WHITE_SQUARES_BB) && (BISHOP_BB(Side) & BLACK_SQUARES_BB))		{
			tmp += BishopPair[PawnSide + PawnCside];

			if (!BISHOPS(Cside)){
				if (!KNIGHTS(Cside))
					tmp		+= 3 * (WithOutBishop >> 1);

				else tmp	+= WithOutBishop;
			}
		}

		EndGameMat[Side]	+= tmp;
	}


	// Salida rapida, ambos bandos tienen las mismas piezas
	if ((HashMat & 0x1C71C700) == ((HashMat >> 3) & 0x1C71C700))
		return;

	MatDiff	= PieceMat[WHITE] - PieceMat[BLACK];
	Side	= WHITE;

	if (MatDiff < 0)			{
		MatDiff	= -MatDiff;
		Side	= BLACK;
	}


	// Mayor contra peones
	if (MatDiff >= ROOK_VALUE)
		EndGameMat[Side]	+= RookVSpawnsEnd;

	// Menor contra peones
	else if (MatDiff >= KNIGHT_VALUE)
		EndGameMat[Side]	+= KnightVSpawnsEnd;

	// Menor contra torre
	else if (MatDiff >= ROOK_VALUE - KNIGHT_VALUE)
		EndGameMat[Side]	+= RookVSknightEnd;

	else	{
		// Salida rapida, ambos bandos tienen la mismas cantidad de caballos y alfiles
		if ((HashMat & 0x1C700) == ((HashMat >> 3) & 0x1C700))
			return;

		MatDiff	= KNIGHTS(WHITE) + BISHOPS(WHITE) - KNIGHTS(BLACK) - BISHOPS(BLACK);
		Side	= WHITE;

		if (MatDiff < 0)		{
			MatDiff	= -MatDiff;
			Side	= BLACK;
		}

		// Tres menores contra dama o dos torres
		if (MatDiff > 1)
			EndGameMat[Side]	+= KnightBishopVSrookEnd;
	}
}


// Peón semi-pasado. Solo tiene en cuenta a los peones y sus ataques para detectarlo
U8		IsPawnCandidate(SQUARE Square)			{
	TURN		Side		= GET_COLOR(Board[Square]),
				Cside		= FLIP_TURN(Side);

	return PopCountBB(OutpostBB[Cside][Square + Front[Side]] & PAWN_BB(Side)) >= PopCountBB(OutpostBB[Side][Square + Front[Side]] & PAWN_BB(Cside));
}

// Peón retrasado. Solo tiene en cuenta a los peones y sus ataques para detectarlo
U8		IsPawnBackward(SQUARE Square)			{
	SQUARE		To;
	BITBOARD	BB;
	TURN		Side	= GET_COLOR(Board[Square]),
				Cside	= FLIP_TURN(Side);


	// No es retrasado. Existe peon vecino mas atrasado o los peones enemigos no impiden su avance
	if ((OutpostBB[Cside][Square] & PAWN_BB(Side)) || !(ForwardBB[Side][Square] & (PawnAttBB[Side] | PAWN_BB(Cside))))
		return 0;

	for (To	= Square + Front[Side];; To += Front[Side])			{
		BB	= SquareBB[To];

		if (BB & PAWN_BB(Cside))
			return 1;

		if (BB & PawnAttBB[Side])
			return PopCountBB(PAWN_ATTACK_BB(To, Cside)) > PopCountBB(PAWN_ATTACK_BB(To, Side));

		if (BB & PawnAttBB[Cside])
			return 1;
	}
}

// falla en detectar algunos casos
U8		IsPawnAdvanced(SQUARE Square)			{
	SQUARE		To;
	TURN		Side			= GET_COLOR(Board[Square]),
				Cside			= FLIP_TURN(Side);
	BITBOARD	PAWN_SIDE_BB	= PAWN_BB(Side),
				PAWN_CSIDE_BB	= PAWN_BB(Cside),
				BB;


	// No es avanzado.	Existen peones vecinos por delante o no existen peones enemigos estorbando la defensa
	if ((OutpostBB[Side][Square] & PAWN_SIDE_BB) || !(OutpostBB[Cside][Square] & PAWN_SIDE_BB) || (SquareBB[Square] & PawnAttBB[Side]))
		return 0;

	// Comprobando si puede ser defendido por la izquierda
	BB = ForwardBB[Cside][Square - 1];
	if (FILE(Square) != FILE_A && (BB & PAWN_SIDE_BB))			{
		if (BB & (PAWN_CSIDE_BB | PawnAttBB[Cside]))
			for (To = Square - Front[Side] - 1;; To -= Front[Side])				{
				if (MATCH(PAWN | Side, To))
					return 0;

				if (MATCH(PAWN | Cside, To) || PopCountBB(PAWN_ATTACK_BB(To, Side)) < PopCountBB(PAWN_ATTACK_BB(To, Cside)))
					break;
			}

		else return 0;
	}

	// Comprobando si puede ser defendido por la derecha
	BB = ForwardBB[Cside][Square + 1];
	if (FILE(Square) != FILE_H && (BB & PAWN_SIDE_BB))			{
		if (BB& (PAWN_CSIDE_BB | PawnAttBB[Cside]))
			for (To = Square - Front[Side] + 1;; To -= Front[Side])				{
				if (MATCH(PAWN | Side, To))
					return 0;

				if (MATCH(PAWN | Cside, To) || PopCountBB(PAWN_ATTACK_BB(To, Side)) < PopCountBB(PAWN_ATTACK_BB(To, Cside)))
					break;
			}

		else return 0;
	}

	return 1;
}

// Bloqueando a peones enemigos
int		IsPawnPassedBloqued(SQUARE BloquedSquare, SQUARE Square){
	int			Score,
				OutPost		= 0,
				PieceBlock	= Board[BloquedSquare];
	TURN		Side		= GET_COLOR(Board[Square]),
				Cside		= FLIP_TURN(Side);
	int			View		= Side ? 56 : 0;


	// Puntuacion por bloquear a peon pasado, bonifica si el bloqueador no puede ser desalojado
	if (BISHOPS(Side) && !KNIGHTS(Side))		{
		if (BLACK_SQUARES_BB & SquareBB[BloquedSquare])
			OutPost		= !(BISHOP_BB(Side) & BLACK_SQUARES_BB);

		else OutPost	= !(BISHOP_BB(Side) & WHITE_SQUARES_BB);
	}
	Score = ((Distance[Square][BloquedSquare] - 4) * (2 + OutPost)) / 2;


	switch (PieceBlock ^ Cside)	{
		// Torre amiga bloqueando el peón pasado
		case ROOK:			return -RookBlockPassedPawn;

		// Malos bloqueadores se penaliza
		case WHITE_ROOK:	return PawnPassed[Square ^ View];
		case WHITE_QUEEN:	return (PawnPassed[Square ^ View] * 3) / 2;

		// Buenos bloqueadores, mejores si no se pueden desalojar
		case WHITE_KNIGHT:	return KnightBloqPawn	* Score;
		case WHITE_BISHOP:	return BishopBloqPawn	* Score;
		case WHITE_KING:	return KingBloqPawn		* Score;

		default:			return 0;
	}
}


// Detecta tablas por insuficiencia material
U8		MatInsuf(void)			{
	BITBOARD	BB = BISHOP_BB(BLACK) | BISHOP_BB(WHITE);

	// Salida rápida, existen peones, damas o torres
	if (HashMat & 0xFFF000FF)
		return 0;

	switch ((U32)HashMat)		{
		case KK: case KNK: case KKN: case KKB: case KBK: case KNNK: case KKNN:	return 1;
		case KBBK: case KKBB: case KBKB: case KBBKB: case KBKBB: case KBBKBB:	return !((BB & BLACK_SQUARES_BB) && (BB & WHITE_SQUARES_BB));
		default: return 0;
	}
}

// Algunos finales soportados. El retorno por debajo de 16	es para posiciones tablas
int		SupportedEndGames(void)	{
	// Salida Rapida, no se tiene suficiente material
	if (GamePhase > 107)
		return 0;

	TURN		Side, Cside;
	SQUARE		K, k, P = 0, r, R,
				PromSQ;
	int			Dist, View, P_File, k_File, K_File, r_File, k_FrontPawn = 0,
				PawnDiff		= abs(BLACK_PAWNS - WHITE_PAWNS);
	U32			MatHashPiece	= (U32)(HashMat & 0xFFFFFF00);
	BITBOARD	BB;


	// Se pone el punto de vista del tablero del bando que tenga peones, sino al que tenga alfil
	if (BLACK_PAWNS && !WHITE_PAWNS)			{
		Side		= BLACK;
		P			= LSB_BB(BLACK_PAWN_BB);
		k_FrontPawn = (ForwardBB[Side][P] & WHITE_KING_BB) != 0;
	}

	else if (WHITE_PAWNS && !BLACK_PAWNS)		{
		Side		= WHITE;
		P			= LSB_BB(WHITE_PAWN_BB);
		k_FrontPawn = (ForwardBB[Side][P] & BLACK_KING_BB) != 0;
	}

	else if (BLACK_BISHOP_BB)
		Side	= BLACK;

	else if (WHITE_BISHOP_BB)
		Side	= WHITE;

	else return 0;


	Cside	= FLIP_TURN(Side);
	K		= KING_SQ(Side);
	k		= KING_SQ(Cside);
	k_File	= FILE(k);
	K_File	= FILE(K);
	View	= Side ? 0 : 56;


	switch ((U32)HashMat)		{
		// Insuficiencia material									0 -> 0
		case KK: case KNK: case KKN: case KBK: case KKB:
			return DRAW_SCORE(0);

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

		case KBNNKRPP: case KRPPKBNN:
			return DRAW_SCORE(MatSum);

		// Lo importante es no llevar rey débil a esquina de color contrario al alfil, para poder defenderse
		case KRKB: case KBKR:
			Side = ((U32)HashMat == KBKR);

			if (BISHOP_BB(Side) & BLACK_SQUARES_BB)	{
				if (KING_BB(Side) & 0xE0E0E00000070707)
					return DRAW_SCORE(MatSum);
			}
			else if (KING_BB(Side) & 0x707070000E0E0E0)
				return DRAW_SCORE(MatSum);

			return 0;

		/*
		 * Base de datos(bitboard) que segun donde esta el rey(sin peon) determina si es tablas.
		 * Tambien funciona para mas de 3 peones doblados.
		 * Puede fallar cuando existen varios peones y pueden avansar, la idea es que la busqueda compense este error
		 */
		case KQKP: case KPKQ: case KQKPP: case KPPKQ: case KQKPPP: case KPPPKQ:
			BB	= BLACK_PAWN_BB | WHITE_PAWN_BB;

			if (!((~FILE_A_BB) & BB))			{
				if ((MATCH(BLACK_PAWN, A2) && (WHITE_KING_BB & 0xFFFFFFF0E0E0E0E0L)) || (MATCH(WHITE_PAWN, A7) && (BLACK_KING_BB & 0xE0E0E0E0F0FFFFFFL)))
					return DRAW_SCORE(MatSum);
			}
			else if (!((~FILE_H_BB) & BB))		{
				if ((MATCH(BLACK_PAWN, H2) && (WHITE_KING_BB & 0xFFFFFF0F07070707L)) || (MATCH(WHITE_PAWN, H7) && (BLACK_KING_BB & 0x70707070FFFFFFFL)))
					return DRAW_SCORE(MatSum);
			}
			else if (!((~FILE_C_BB) & BB))		{
				if ((MATCH(BLACK_PAWN, C2) && (WHITE_KING_BB & 0xFFFFFFFFF8F0E0E0L)) || (MATCH(WHITE_PAWN, C7) && (BLACK_KING_BB & 0xE0E0F0F8FFFFFFFFL)))
					return DRAW_SCORE(MatSum);
			}
			else if (!((~FILE_F_BB) & BB))		{
				if ((MATCH(BLACK_PAWN, F2) && (WHITE_KING_BB & 0xFFFFFFFF1F0F0707)) || (MATCH(WHITE_PAWN, F7) && (BLACK_KING_BB & 0x7070F1FFFFFFFFFL)))
					return DRAW_SCORE(MatSum);
			}
			return 0;

		case KPK: case KKP:
			P		= LSB_BB(BLACK_PAWN_BB | WHITE_PAWN_BB);
			Side	= GET_COLOR(Board[P]);
			Cside	= FLIP_TURN(Side);
			PromSQ	= LSB_BB(ForwardBB[Side][P] & (RANK_1_BB | RANK_8_BB));

			// Peón no imparable o regla del cuadrado
			if (Distance[PromSQ][P] + (Turn == Cside) >= Distance[PromSQ][KING_SQ(Cside)])		{
				// Rey enemigo captura al peón
				if (Distance[KING_SQ(Side)][P] + (Turn == Cside) > Distance[P][KING_SQ(Cside)])
					return DRAW_SCORE(MatSum) - 118 * Distance[KING_SQ(Cside)][P];

				// Peón en columna torre
				if (FileBB[P] & (FILE_A_BB | FILE_H_BB))
					return DRAW_SCORE(MatSum) - 118 * Distance[KING_SQ(Cside)][PromSQ ^ 9];

				// Controlando con Rey las casillas por delante del peón
				if (KING_MOVES_BB(KING_SQ(Cside)) & SquareBB[P + Front[Side]])
					return DRAW_SCORE(MatSum);
			}
			return 0;

		case KPPK: case KKPP: case KPPPK: case KKPPP:
			BB	= BLACK_PAWN_BB | WHITE_PAWN_BB;

			if (!((~FILE_A_BB) & BB))			{
				if (BLACK_PAWN_BB)
					return DRAW_SCORE(MatSum) - 118 * Distance[B2][KING_SQ(WHITE)];

				return DRAW_SCORE(MatSum) - 118 * Distance[B7][KING_SQ(BLACK)];
			}
			if (!((~FILE_H_BB) & BB))		{
				if (BLACK_PAWN_BB)
					return DRAW_SCORE(MatSum) - 118 * Distance[G2][KING_SQ(WHITE)];

				return DRAW_SCORE(MatSum) - 118 * Distance[G7][KING_SQ(BLACK)];
			}
			return 0;

		case KBPK: case KKBP: case KBPPK: case KKBPP: case KBPPPK: case KKBPPP:
		case KBBPK: case KKBBP: case KBBPPK: case KKBBPP: case KBBPPPK: case KKBBPPP:
			BB	= BLACK_PAWN_BB | WHITE_PAWN_BB;

			if (!((~FILE_A_BB) & BB))			{
				if (BLACK_PAWN_BB)				{
					if (!(BLACK_SQUARES_BB & BLACK_BISHOP_BB))
						return DRAW_SCORE(MatSum) - 96 * Distance[B2][KING_SQ(WHITE)];
				}
				else if (!(WHITE_SQUARES_BB & WHITE_BISHOP_BB))
					return DRAW_SCORE(MatSum) - 96 * Distance[B7][KING_SQ(BLACK)];
			}
			else if (!((~FILE_H_BB) & BB))		{
				if (BLACK_PAWN_BB)				{
					if (!(BLACK_SQUARES_BB & BLACK_BISHOP_BB))
						return DRAW_SCORE(MatSum) - 96 * Distance[G2][KING_SQ(WHITE)];
				}
				else if (!(WHITE_SQUARES_BB & WHITE_BISHOP_BB))
					return DRAW_SCORE(MatSum) - 96 * Distance[G7][KING_SQ(BLACK)];
			}
			return 0;

		/*
		 * 1) Controlar casilla por delante del peón de color distinto al alfil con pieza(NO REY)
		 * 2) Ocupar casilla por delante del peón de color distinto al alfil
		 */
		case KNKBP:	case KBKBP:	case KBPKN:	case KBPKB:
			P		= LSB_BB(BLACK_PAWN_BB | WHITE_PAWN_BB);
			Side	= GET_COLOR(Board[P]);
			Cside	= FLIP_TURN(Side);

			if (BISHOP_BB(Side) & BLACK_SQUARES_BB)
				BB	= ForwardBB[Side][P] & WHITE_SQUARES_BB;

			else BB	= ForwardBB[Side][P] & BLACK_SQUARES_BB;


			if (AllPieceBB[Cside] & BB)
				return DRAW_SCORE(MatSum);

			if (KNIGHT_BB(Cside))				{
				if (KnightMoveBB[LSB_BB(KNIGHT_BB(Cside))] & BB)
					return DRAW_SCORE(MatSum);
			}
			else if (BISHOP_BB(Cside) && (BishopPseudoMoveBB[LSB_BB(BISHOP_BB(Cside))] & BB))
				return DRAW_SCORE(MatSum);

			return 0;
//===============================================================================================================================


		/*
		 * Mate elemental con caballo y alfil
		 *
		 * 1) acercar caballo a rey débil
		 * 2) acercar rey a rey débil
		 * 3) controlar casillas de color distinto al alfil con caballo
		 * 4) llevar a esquina de color del alfil al rey débil
		 */
		case KKBN:	case KBNK:
			EndGameMat[Cside] -= 150 / Distance[K][k] + 128 / Distance[k][LSB_BB(KNIGHT_BB(Side))];

			if (BISHOP_BB(Side) & BLACK_SQUARES_BB)				{
				if (KNIGHT_BB(Side) & BLACK_SQUARES_BB)
					EndGameMat[Cside] -= 32;

				return 16 * MIN(Distance[k][A1], Distance[k][H8]) + 4;
			}

			if (KNIGHT_BB(Side) & WHITE_SQUARES_BB)
				EndGameMat[Cside] -= 32;

			return 16 * MIN(Distance[k][H1], Distance[k][A8]) + 4;

		case KRKP:	case KPKR:
			// Salida rápida, el rey no seta cerca para apoyar el peón
			if (Distance[K][P] > 2)
				break;

			r		= LSB_BB(ROOK_BB(Cside));
			r_File	= FILE(r);
			P_File	= FILE(P);
			PromSQ	= LSB_BB(ForwardBB[Side][P] & (RANK_1_BB | RANK_8_BB));

			if ((Turn == Cside) && Distance[K][P] == 2 && abs(RANK(K) - RANK(P)) < 2 && r_File != (K_File + P_File) / 2)
				break;


			Dist = Distance[K][PromSQ] + Distance[P][PromSQ];

			if (K == PromSQ)
				Dist++;

			if (P + Front[Side] == K)	{
				if (FileBB[P] & (FILE_A_BB | FILE_H_BB))
					break;

				Dist++;
			}

			if (r_File != P_File && RANK(r ^ View) != RANK_8)
				Dist--;

			if (Turn == Side)
				Dist--;


			return 75 * MAX(0, Distance[k][PromSQ] - Dist) + 1;

		case KRKRP:	case KRPKR:
			R		= LSB_BB(ROOK_BB(Side));
			r		= LSB_BB(ROOK_BB(Cside));
			r_File	= FILE(r);
			P_File	= FILE(P);
			PromSQ	= LSB_BB(ForwardBB[Side][P] & (RANK_1_BB | RANK_8_BB));


			// Back-rank defense
			if (k_FrontPawn && (RANK(P ^ View) >= RANK_6) && (FILE_C > P_File || P_File > FILE_F) && RANK(r ^ View) == RANK_8 && RANK(k ^ View) == RANK_8)
				return 170;

			// Pawn in rook file
			if (FileBB[P] & (FILE_A_BB | FILE_H_BB))		{
				if (RANK(P ^ View) == RANK_7)				{
					// King in front of pawn
					if (k_FrontPawn && abs(P_File - r_File) == 1 && abs(P_File - k_File < 5))
						return 170;

					// Rook in front of pawn
					if (R == PromSQ && P_File == r_File && (Distance[k][R] < 3 || (RANK(k ^ View) == RANK_7 && Distance[k][P] > 5)))
						return 170;
				}
				// Vancura position
				else if (RANK(r) == RANK(P) && MIN(Distance[k ^ View][H8], Distance[k ^ View][A8]) < 2 && (ForwardBB[Side][P] & ROOK_BB(Side)))
					return 170;

				// Enemy king in front of pawn
				if (k_FrontPawn)
					return 170;
			}

			// Philidor position
			if (PawnPassedBB[Side][P] & KING_BB(Cside))		{
				if (RANK(P ^ View) < RANK_6)				{
					if (RANK(K ^ View) < RANK_6 && RANK(r ^ View) == RANK_6)
						return 133;
				}

				else if (RANK(K ^ View) > RANK_2)
					return 170;

				return 73;
			}

			// Short side defense
			if ((P_File == FILE_B || P_File == FILE_G) && Distance[K][P] == 1 && abs(P_File - r_File) > 3 && abs(P_File - k_File) < 3 && Distance[k][r] > 5)
				return 170;
			break;

		default:;
	}


	if (MatHashPiece && PieceMat[BLACK] == PieceMat[WHITE])		{
		// Difícil ganar cuando rey enemigo esta frente a único peón y se tiene igual material
		if (k_FrontPawn && PAWNS(Side) == 1)
			return 239;

		// Difícil ganar cuando: pocos peones, no debilidades o fortalezas sustanciales, se tiene igual material
		if (PawnDiff < 2 && ALL_PAWNS < 7 && BLACK_PAWNS && WHITE_PAWNS)		{
			// Mayor cantidad de peones negros, no peones pasados negros, no peones aislados blancos
			if (BLACK_PAWNS > WHITE_PAWNS)		{
				if (Distance[KING_SQ(WHITE)][LSB_BB(WHITE_PAWN_BB)] < 3 && !PassedPawns[BLACK] && (WHITE_PAWNS == 1 || !IsolatedPawns[WHITE]))
					return 64;
			}

			// Mayor cantidad de peones blancos, no peones pasados blancos, no peones aislados negros
			else if (BLACK_PAWNS < WHITE_PAWNS)	{
				if (Distance[KING_SQ(BLACK)][LSB_BB(BLACK_PAWN_BB)] < 3 && !PassedPawns[WHITE] && (BLACK_PAWNS == 1 || !IsolatedPawns[BLACK]))
					return 64;
			}

			// Igual cantidad de peones y no debilidades o fortalezas explotables
			else if (!(IsolatedPawns[BLACK] || IsolatedPawns[WHITE] || PassedPawns[BLACK] || PassedPawns[WHITE]) || (BLACK_PAWNS == 1))
				return 32;
		}
	}


	switch (MatHashPiece)		{
		// Prioriza ir final de peones o de caballos
		case KK: case KNKN: case KNNKNN:	return 14;

		// Final de alfiles. Ojo con los peones pasados
		case KBKB:
			BB	= BLACK_BISHOP_BB | WHITE_BISHOP_BB;

			// Alfiles de distinto color
			if ((BB & BLACK_SQUARES_BB) && (BB & WHITE_SQUARES_BB))	{
				if (PawnDiff < 4)
					return DiferentBishop[MIN(16, ALL_PAWNS)];
			}

			// Alfiles de igual color
			else if (PawnDiff < 2)
				return DiferentBishop[MIN(16, ALL_PAWNS)] / 2;

			return 0;

			// Estos finales posiblemente progresen a un final tablas. Asegurar no haber muchos peones pasados
		case KRKR: case KQKQ: case KRRKRR:
			if (PawnDiff >= 2 || (abs(PassedPawns[BLACK] - PassedPawns[WHITE]) > 1))
				return 0;
			return DiffEndGame[MIN(16, ALL_PAWNS)];

		case KBBKBB:
			if (PawnDiff >= 2 || (abs(PassedPawns[BLACK] - PassedPawns[WHITE]) > 1))
				return 0;

			if (ALL_PAWNS)
				return 128 / MIN(16, ALL_PAWNS);
			return MATE;

		case KRBKRB: case KQBKQB:
			if (PawnDiff >= 2 || (abs(PassedPawns[BLACK] - PassedPawns[WHITE]) > 1))
				return 0;

			BB	= BLACK_BISHOP_BB | WHITE_BISHOP_BB;

			if ((BB & BLACK_SQUARES_BB) && (BB & WHITE_SQUARES_BB))
				return DiffEndGame[MIN(16, ALL_PAWNS)] + 4;

			return DiffEndGame[MIN(16, ALL_PAWNS)];
		default:	return 0;
	}


	/*
	 * Regla general(no perfecta) para identificar posiciones tablas:
	 * Si ambos bandos no tienen peones se necesita el equivalente a casi 4 peones para poder ganar
	 */
	if (!ALL_PAWNS && abs(PieceMat[BLACK] - PieceMat[WHITE]) < 14 * PAWN_VALUE / 4)		{
		if (PieceMat[WHITE] > PieceMat[BLACK] && !(KING_BB(BLACK) & 0xFFC381818181C3FF))
			return DRAW_SCORE(MatSum);

		if (PieceMat[BLACK] > PieceMat[WHITE] && !(KING_BB(WHITE) & 0xFFC381818181C3FF))
			return DRAW_SCORE(MatSum);
	}
}


// Precalcula datos necesarios en la evaluacion
void	EvalIni(void)			{
	SQUARE	SQ1, SQ2;


	/*
	 * Inicializa las tablas del tropismo.
	 * Ver 'Evaluation Function Draft' de 'www.chessprogramming.org' en 'Home/Evaluation/Evaluation Function Draft'
	 */
	int DiagNW[64] = {0,1,2,3,4,5,6,7,1,2,3,4,5,6,7,8,2,3,4,5,6,7,8,9,3,4,5,6,7,8,9,10,4,5,6,7,8,9,10,11,5,6,7,8,9,10,11,12,6,7,8,9,10,11,12,13,7,8,9,10,11,12,13,14,};
	int DiagNE[64] = {7,6,5,4,3,2,1,0,8,7,6,5,4,3,2,1,9,8,7,6,5,4,3,2,10,9,8,7,6,5,4,3,11,10,9,8,7,6,5,4,12,11,10,9,8,7,6,5,13,12,11,10,9,8,7,6,14,13,12,11,10,9,8,7,};
	int Bonus_Distance[15] = {5, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


	SQ_LOOP(SQ1)
		SQ_LOOP(SQ2)			{
			Tropism[SQ1][SQ2]		= 14 - (abs(FILE(SQ1) - FILE(SQ2)) + abs(RANK(SQ1) - RANK(SQ2)));
			BishopTropism[SQ1][SQ2]	= Bonus_Distance[abs(DiagNE[SQ1] - DiagNE[SQ2])] + Bonus_Distance[abs(DiagNW[SQ1] - DiagNW[SQ2])];
		}
}


void	PutAttack(TURN Side, BITBOARD BB, int KingAttWeight)		{
	PieceAttBB[Side]	|= BB;
	BB					&= KING_ZONE[FLIP_TURN(Side)];

	if (KingAttWeight && BB)		{
		AttackCount[Side]	+= KingAttWeight / 2;
		AttackWeight[Side]	+= KingAttWeight * PopCountBB(BB);
	}
}


// 1700
