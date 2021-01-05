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

#include "bitboard.h"


BITBOARD			SquareBB[64];
BITBOARD			FileBB[64];
BITBOARD			RankBB[64];


BITBOARD			KnightMoveBB[64];
BITBOARD			BishopPseudoMoveBB[64];
BITBOARD			RookPseudoMoveBB[64];
BITBOARD			QueenPseudoMoveBB[64];
BITBOARD			KingMoveBB[64];


BITBOARD			PawnAdvanceBB[2][64];
BITBOARD			PawnAdvance2BB[2][64];
BITBOARD			PawnAttackBB[2][64];


BITBOARD			PawnPassedBB[2][64];

BITBOARD			ForwardBB[2][64];
BITBOARD			OutpostBB[2][64];


BITBOARD			KingPatternBB[2][64];
BITBOARD			KingFrontBB[2][64];
BITBOARD			KingZoneBB[2][64];


BITBOARD			MagicMoveB[64][512];
BITBOARD			MagicMoveR[64][4096];


#define				OUT_BOARD(Square)		((Square) & 0x88)
#define				MOBILITY_LOOP(Delta, Piece)		for ((Delta) = Offset[Piece]; *(Delta); (Delta)++)


// Convierte casilla de formato 0x88 a la usada por el motor
static const SQUARE	SquareTO64[128]			= {
	A1,B1,C1,D1,E1,F1,G1,H1,	0,0,0,0,0,0,0,0,
	A2,B2,C2,D2,E2,F2,G2,H2,	0,0,0,0,0,0,0,0,
	A3,B3,C3,D3,E3,F3,G3,H3,	0,0,0,0,0,0,0,0,
	A4,B4,C4,D4,E4,F4,G4,H4,	0,0,0,0,0,0,0,0,
	A5,B5,C5,D5,E5,F5,G5,H5,	0,0,0,0,0,0,0,0,
	A6,B6,C6,D6,E6,F6,G6,H6,	0,0,0,0,0,0,0,0,
	A7,B7,C7,D7,E7,F7,G7,H7,	0,0,0,0,0,0,0,0,
	A8,B8,C8,D8,E8,F8,G8,H8,	0,0,0,0,0,0,0,0,
};

// Convierte casilla de formato del motor a formato 0x88
static const SQUARE	SquareTO0x88[64]		= {
	  0,  1,  2,  3,  4,  5,  6,  7,
	 16, 17, 18, 19, 20, 21, 22, 23,
	 32, 33, 34, 35, 36, 37, 38, 39,
	 48, 49, 50, 51, 52, 53, 54, 55,
	 64, 65, 66, 67, 68, 69, 70, 71,
	 80, 81, 82, 83, 84, 85, 86, 87,
	 96, 97, 98, 99,100,101,102,103,
	112,113,114,115,116,117,118,119,
};

static const int	Front0x88[2]			= {-16, 16};

// Variacion a las casillas en formato 0x88 para obtener los movimientos de las piezas
int					Offset[16][16]			= {
	{0,0,0,0,0,0,0,0,0},			{0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0},			{0,0,0,0,0,0,0,0,0},
	{33,31,18,14,-33,-31,-14,-18,0},{-31,-33,-14,-18,33,31,14,18,0},
	{17,15,-15,-17,0,0,0,0,0},		{-15,-17,17,15,0,0,0,0,0},
	{16,-16,1,-1,0,0,0,0,0},		{-16,16,-1,1,0,0,0,0,0},
	{17,16,15,-15,-16,-17,1,-1,0},	{-15,-16,-17,17,16,15,-1,1,0},
	{17,16,15,-15,-16,-17,1,-1,0},	{-15,-16,-17,17,16,15,-1,1,0},
	{0,0,0,0,0,0,0,0,0},			{0,0,0,0,0,0,0,0,0}
};


// Variables necesarias en la siguiente funcion
static const BITBOARD	k1		= 0x5555555555555555L;	// -1/3
static const BITBOARD	k2		= 0x3333333333333333L;	// -1/5
static const BITBOARD	k4		= 0x0F0F0F0F0F0F0F0FL;	// -1/17
static const BITBOARD	kf		= 0x0101010101010101L;	// -1/255

/*
 * Cuenta la cantidad de bits activos en 'BitBoard'.
 * Ver 'SWAR-Popcount' de 'www.chessprogramming.org' en 'Home/Board Representation/Bitboards/Population Count'
 */
int		PopCountBB(BITBOARD BB)	{
	if (BB)				{
		BB	=	BB - ((BB >> 1) & k1);
		BB	=	(BB & k2) + ((BB >> 2) & k2);
		BB	=	(BB + (BB >> 4)) & k4;
		return	(int)((BB * kf) >> 56);
	}

	return 0;
}


// Extrae todos los Bit de 'BBmask' y los anhade a la lista 'BBlist'
void	ExtractBit(BITBOARD* BBlist, BITBOARD BBmask)			{
	int		i	= 0;

	for (; BBmask; BBmask ^= BBlist[i - 1])
		BBlist[i++] = SquareBB[LSB_BB(BBmask)];

	BBlist[i++] = SquareBB[A1];
}


int				IndMask;
BITBOARD		RookVariation[64][4096];
BITBOARD		BishopVariation[64][512];

// Calcula los movimientos de Alfiles, Torres y Damas. Lo devuelve en Bitboard
BITBOARD		GetSlideMoves(BITBOARD Variation, SQUARE Square, PIECE Piece)	{
	SQUARE		*Delta, To;
	BITBOARD	Moves	= 0;

	for (Delta = Offset[Piece]; *Delta; Delta++)
		for (To = SquareTO0x88[Square] + *Delta; !OUT_BOARD(To); To += *Delta)	{
			Moves |= SquareBB[SquareTO64[To]];

			if (SquareBB[SquareTO64[To]] & Variation)
				break;
		}

	return Moves;
}


// Funcion auxiliar para calcular la variacion para los movimientos de Torre utilizado en los numeros magicos
void	RvariationAux(BITBOARD* BBlist, SQUARE Square, int Depth, BITBOARD Mask){
	if (BBlist[Depth] == SquareBB[A1])
		RookVariation[Square][IndMask++] = Mask;

	else		{
		RvariationAux(BBlist, Square, Depth + 1, Mask | BBlist[Depth]);
		RvariationAux(BBlist, Square, Depth + 1, Mask);
	}
}


// Calcula la variacion para los movimientos de Torre utilizado en los numeros magicos
void	Rvariation(SQUARE Square)				{
	BITBOARD	BBlist[13];
	IndMask		= 0;

	ExtractBit(BBlist, RoccupancyMask[Square]);
	RvariationAux(BBlist, Square, 0, 0);
}


// Funcion auxiliar para calcular la variacion para los movimientos de Alfil utilizado en los numeros magicos
void	BvariationAux(BITBOARD* BBlist, SQUARE Square, int Depth, BITBOARD Mask){
	if (BBlist[Depth] == SquareBB[A1])
		BishopVariation[Square][IndMask++] = Mask;

	else		{
		BvariationAux(BBlist, Square, Depth + 1, Mask | BBlist[Depth]);
		BvariationAux(BBlist, Square, Depth + 1, Mask);
	}
}


// Calcula la variacion para los movimientos de Alfil utilizado en los numeros magicos
void	Bvariation(SQUARE Square)				{
	BITBOARD	BBlist[13];
	IndMask		= 0;

	ExtractBit(BBlist, BoccupancyMask[Square]);
	BvariationAux(BBlist, Square, 0, 0);
}


// Inicializa los movimientos magicos de Torre
void	MagicRookMoveInit(SQUARE Square)		{
	int		MagicIndex,
			RookVariationSize[64]	= {4096,2048,2048,2048,2048,2048,2048,4096,2048,1024,1024,1024,1024,1024,1024,2048,2048,1024,1024,1024,1024,1024,1024,2048,2048,1024,1024,1024,1024,1024,1024,2048,2048,1024,1024,1024,1024,1024,1024,2048,2048,1024,1024,1024,1024,1024,1024,2048,2048,1024,1024,1024,1024,1024,1024,2048,4096,2048,2048,2048,2048,2048,2048,4096};

	for (IndMask = 0; IndMask < RookVariationSize[Square]; IndMask++)			{
		MagicIndex						= (int)((RookVariation[Square][IndMask] * MagicRnumber[Square]) >> MagicNumShiftR[Square]);
		MagicMoveR[Square][MagicIndex]	= GetSlideMoves(RookVariation[Square][IndMask], Square, BLACK_ROOK);
	}
}


// Inicializa los movimientos magicos de Alfil
void	MagicBishopMoveInit(SQUARE Square)		{
	int		MagicIndex,
			BishopVariationSize[64]	= {64,32,32,32,32,32,32,64,32,32,32,32,32,32,32,32,32,32,128,128,128,128,32,32,32,32,128,512,512,128,32,32,32,32,128,512,512,128,32,32,32,32,128,128,128,128,32,32,32,32,32,32,32,32,32,32,64,32,32,32,32,32,32,64};

	for (IndMask = 0; IndMask < BishopVariationSize[Square]; IndMask++)			{
		MagicIndex						= (int)((BishopVariation[Square][IndMask] * MagicBnumber[Square]) >> MagicNumShiftB[Square]);
		MagicMoveB[Square][MagicIndex]	= GetSlideMoves(BishopVariation[Square][IndMask], Square, BLACK_BISHOP);
	}
}


void	SquareInitBB(void)		{
	SQUARE		SQ;

	SQ_LOOP(SQ)
		SquareBB[SQ] = ((BITBOARD)0x1L) << SQ;
}


void	RankFileInitBB(void)	{
	SQUARE		SQ;

	SQ_LOOP(SQ)	{
		FileBB[SQ] = (BITBOARD)(FILE_A_BB) << FILE(SQ);
		RankBB[SQ] = (BITBOARD)(RANK_1_BB) << (RANK(SQ) * 8);
	}
}


void	ForwardInitBB(void)		{
	SQUARE		SQ, SQx88;
	TURN		Side;

	memset(ForwardBB, 0, sizeof(ForwardBB));

	SQ_LOOP(SQ)
		TURN_LOOP(Side)
			for (SQx88 = SquareTO0x88[SQ] + Front0x88[Side]; !OUT_BOARD(SQx88); SQx88 += Front0x88[Side])
				ForwardBB[Side][SQ] |= SquareBB[SquareTO64[SQx88]];
}


void	PawnPassedInitBB(void)	{
	TURN		Side;
	SQUARE		SQ, SQx88;

	memset(PawnPassedBB, 0, sizeof(PawnPassedBB));

	SQ_LOOP(SQ)
		TURN_LOOP(Side)
			for (SQx88 = SquareTO0x88[SQ] + Front0x88[Side]; !OUT_BOARD(SQx88); SQx88 += Front0x88[Side])		{
				PawnPassedBB[Side][SQ] |= SquareBB[SquareTO64[SQx88]];

				if (!OUT_BOARD(SQx88 - 1))
					PawnPassedBB[Side][SQ] |= SquareBB[SquareTO64[SQx88 - 1]];

				if (!OUT_BOARD(SQx88 + 1))
					PawnPassedBB[Side][SQ] |= SquareBB[SquareTO64[SQx88 + 1]];
			}
}


void	OutpostInitBB(void)		{
	SQUARE		SQ, SQx88;
	TURN		Side;

	memset(OutpostBB, 0, sizeof(OutpostBB));

	SQ_LOOP(SQ)
		TURN_LOOP(Side)
			for (SQx88 = SquareTO0x88[SQ] + Front0x88[Side]; !OUT_BOARD(SQx88); SQx88 += Front0x88[Side])		{
				if (!OUT_BOARD(SQx88 - 1))
					OutpostBB[Side][SQ] |= SquareBB[SquareTO64[SQx88 - 1]];

				if (!OUT_BOARD(SQx88 + 1))
					OutpostBB[Side][SQ] |= SquareBB[SquareTO64[SQx88 + 1]];
			}
}


void	KingPatternInitBB(void)	{
	TURN		Side;
	SQUARE		SQ, *Delta, To,
				KingPattern[2][4]	= {{-31, -32, -33, 0}, {31, 32, 33, 0},};

	memset(KingPatternBB, 0, sizeof(KingPatternBB));

	SQ_LOOP(SQ)
		TURN_LOOP(Side)
			for (Delta = KingPattern[Side]; *Delta; Delta++)	{
				To = SquareTO0x88[SQ] + *Delta;

				if (!OUT_BOARD(To))
					KingPatternBB[Side][SQ] |= SquareBB[SquareTO64[To]];
			}
}


void	KingFrontInitBB(void)	{
	TURN		Side;
	SQUARE		SQ, *Delta, To,
				KingFront[2][4]		= {{-17, -16, -15, 0}, {17, 16, 15, 0},};

	memset(KingFrontBB, 0, sizeof(KingFrontBB));

	SQ_LOOP(SQ)
		TURN_LOOP(Side)
			for (Delta = KingFront[Side]; *Delta; Delta++)		{
				To = SquareTO0x88[SQ] + *Delta;

				if (!OUT_BOARD(To))
					KingFrontBB[Side][SQ] |= SquareBB[SquareTO64[To]];
			}
}


void	KingZoneInitBB(void)	{
	TURN		Side;
	SQUARE		SQ;

	SQ_LOOP(SQ)
		TURN_LOOP(Side)
			KingZoneBB[Side][SQ] = SquareBB[SQ] | KingPatternBB[Side][SQ] | KingMoveBB[SQ];
}


void	PawnAttackInitBB(void)	{
	SQUARE		Square, SQx88;
	TURN		Side;

	memset(PawnAttackBB, 0, sizeof(PawnAttackBB));

	SQ_LOOP(Square)
		TURN_LOOP(Side)
			for (SQx88 = SquareTO0x88[Square] + Front0x88[Side] - 1; SQx88 < SquareTO0x88[Square] + Front0x88[Side] + 2; SQx88 += 2)
				if (!OUT_BOARD(SQx88))
					PawnAttackBB[Side][Square] |= SquareBB[SquareTO64[SQx88]];
}


void	PawnAdvanceInitBB(void)	{
	SQUARE		Square, To;
	TURN		Side;

	memset(PawnAdvanceBB, 0, sizeof(PawnAdvanceBB));

	SQ_LOOP(Square)
		TURN_LOOP(Side)			{
			To = Square + Front[Side];

			if (A1 <= To && To <= H8)
				PawnAdvanceBB[Side][Square]	= SquareBB[To];
		}
}


void	PawnAdvance2InitBB(void){
	SQUARE		Square;

	memset(PawnAdvance2BB, 0, sizeof(PawnAdvance2BB));

	for (Square = A2; Square <= H2; Square++)
		PawnAdvance2BB[WHITE][Square]	= SquareBB[Square + 16];

	for (Square = A7; Square <= H7; Square++)
		PawnAdvance2BB[BLACK][Square]	= SquareBB[Square - 16];
}


void	MobInitBB(void)			{
	SQUARE	Square, To0x88, *Delta;


	SQ_LOOP(Square)				{
		KnightMoveBB[Square] = KingMoveBB[Square] = BishopPseudoMoveBB[Square] = RookPseudoMoveBB[Square] = 0x0L;

		MOBILITY_LOOP(Delta, KNIGHT)			{
			To0x88 = SquareTO0x88[Square] + *Delta;

			if (!OUT_BOARD(To0x88))
				KnightMoveBB[Square]		|= SquareBB[SquareTO64[To0x88]];
		}

		MOBILITY_LOOP(Delta, KING)				{
			To0x88 = SquareTO0x88[Square] + *Delta;

			if (!OUT_BOARD(To0x88))
				KingMoveBB[Square]			|= SquareBB[SquareTO64[To0x88]];
		}

		MOBILITY_LOOP(Delta, BISHOP)
			for (To0x88 = SquareTO0x88[Square] + *Delta; !OUT_BOARD(To0x88); To0x88 += *Delta)
				BishopPseudoMoveBB[Square]	|= SquareBB[SquareTO64[To0x88]];

		MOBILITY_LOOP(Delta, ROOK)
			for (To0x88 = SquareTO0x88[Square] + *Delta; !OUT_BOARD(To0x88); To0x88 += *Delta)
				RookPseudoMoveBB[Square]	|= SquareBB[SquareTO64[To0x88]];

		QueenPseudoMoveBB[Square] = BishopPseudoMoveBB[Square] | RookPseudoMoveBB[Square];
	}
}


// Inicializa datos y varaiables para el funcionamiento de los Bitboard
void	BitboardInitBB(void)	{
	SQUARE		Square;


	SquareInitBB();
	RankFileInitBB();
	ForwardInitBB();

	PawnPassedInitBB();
	PawnAttackInitBB();
	PawnAdvanceInitBB();
	PawnAdvance2InitBB();

	OutpostInitBB();
	MobInitBB();

	KingPatternInitBB();
	KingFrontInitBB();
	KingZoneInitBB();


	SQ_LOOP(Square)				{
		Rvariation(Square);
		Bvariation(Square);
	}


	SQ_LOOP(Square)				{
		MagicRookMoveInit(Square);
		MagicBishopMoveInit(Square);
	}
}


// 400
