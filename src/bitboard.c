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
static const BITBOARD	k1		= 0x5555555555555555L;			// -1/3
static const BITBOARD	k2		= 0x3333333333333333L;			// -1/5
static const BITBOARD	k4		= 0x0F0F0F0F0F0F0F0FL;			// -1/17
static const BITBOARD	kf		= 0x0101010101010101L;			// -1/255

/*
 * Cuenta la cantidad de bits activos en 'BitBoard'.
 * Ver 'SWAR-Popcount' de 'www.chessprogramming.org' en 'Home/Board Representation/Bitboards/Population Count'
 */
int		PopCountBB(BITBOARD bb)	{
	if (bb)				{
		bb	=	bb - ((bb >> 1) & k1);
		bb	=	(bb & k2) + ((bb >> 2) & k2);
		bb	=	(bb + (bb >> 4)) & k4;
		return	(int)((bb * kf) >> 56);
	}

	return 0;
}


// Extrae todos los Bit de 'BBmask' y los anhade a la lista 'BBlist'
void	ExtractBit(BITBOARD* list_bb, BITBOARD mask_bb)			{
	int		i	= 0;

	for (; mask_bb; mask_bb ^= list_bb[i - 1])
		list_bb[i++] = SquareBB[LSB_BB(mask_bb)];

	list_bb[i++] = SquareBB[A1];
}


int				IndMask;
BITBOARD		RookVariation[64][4096];
BITBOARD		BishopVariation[64][512];

// Calcula los movimientos de Alfiles, Torres y Damas. Lo devuelve en Bitboard
BITBOARD		GetSlideMoves(BITBOARD variation, SQUARE sq, PIECE piece)		{
	SQUARE		*delta, to;
	BITBOARD	moves	= 0;

	for (delta = Offset[piece]; *delta; delta++)
		for (to = SquareTO0x88[sq] + *delta; !OUT_BOARD(to); to += *delta)	{
			moves |= SquareBB[SquareTO64[to]];

			if (SquareBB[SquareTO64[to]] & variation)
				break;
		}

	return moves;
}


// Funcion auxiliar para calcular la variacion para los movimientos de Torre utilizado en los numeros magicos
void	RvariationAux(BITBOARD* list_bb, SQUARE sq, int d, BITBOARD mask_bb)	{
	if (list_bb[d] == SquareBB[A1])
		RookVariation[sq][IndMask++] = mask_bb;

	else		{
		RvariationAux(list_bb, sq, d + 1, mask_bb | list_bb[d]);
		RvariationAux(list_bb, sq, d + 1, mask_bb);
	}
}


// Calcula la variacion para los movimientos de Torre utilizado en los numeros magicos
void	Rvariation(SQUARE sq)	{
	BITBOARD	list_bb[13];
	IndMask		= 0;

	ExtractBit(list_bb, RoccupancyMask[sq]);
	RvariationAux(list_bb, sq, 0, 0);
}


// Funcion auxiliar para calcular la variacion para los movimientos de Alfil utilizado en los numeros magicos
void	BvariationAux(BITBOARD* list_bb, SQUARE sq, int d, BITBOARD mask_bb)	{
	if (list_bb[d] == SquareBB[A1])
		BishopVariation[sq][IndMask++] = mask_bb;

	else		{
		BvariationAux(list_bb, sq, d + 1, mask_bb | list_bb[d]);
		BvariationAux(list_bb, sq, d + 1, mask_bb);
	}
}


// Calcula la variacion para los movimientos de Alfil utilizado en los numeros magicos
void	Bvariation(SQUARE sq)	{
	BITBOARD	list_bb[13];
	IndMask		= 0;

	ExtractBit(list_bb, BoccupancyMask[sq]);
	BvariationAux(list_bb, sq, 0, 0);
}


// Inicializa los movimientos magicos de Torre
void	MagicRookMoveInit(SQUARE sq)			{
	int		magic_index,
			rook_variation_size[64]	= {4096,2048,2048,2048,2048,2048,2048,4096,2048,1024,1024,1024,1024,1024,1024,2048,2048,1024,1024,1024,1024,1024,1024,2048,2048,1024,1024,1024,1024,1024,1024,2048,2048,1024,1024,1024,1024,1024,1024,2048,2048,1024,1024,1024,1024,1024,1024,2048,2048,1024,1024,1024,1024,1024,1024,2048,4096,2048,2048,2048,2048,2048,2048,4096};

	for (IndMask = 0; IndMask < rook_variation_size[sq]; IndMask++)			{
		magic_index					= (int)((RookVariation[sq][IndMask] * MagicRnumber[sq]) >> MagicNumShiftR[sq]);
		MagicMoveR[sq][magic_index]	= GetSlideMoves(RookVariation[sq][IndMask], sq, BLACK_ROOK);
	}
}


// Inicializa los movimientos magicos de Alfil
void	MagicBishopMoveInit(SQUARE sq)			{
	int		magic_index,
			bishop_variation_size[64]	= {64,32,32,32,32,32,32,64,32,32,32,32,32,32,32,32,32,32,128,128,128,128,32,32,32,32,128,512,512,128,32,32,32,32,128,512,512,128,32,32,32,32,128,128,128,128,32,32,32,32,32,32,32,32,32,32,64,32,32,32,32,32,32,64};

	for (IndMask = 0; IndMask < bishop_variation_size[sq]; IndMask++)		{
		magic_index					= (int)((BishopVariation[sq][IndMask] * MagicBnumber[sq]) >> MagicNumShiftB[sq]);
		MagicMoveB[sq][magic_index]	= GetSlideMoves(BishopVariation[sq][IndMask], sq, BLACK_BISHOP);
	}
}


void	SquareInitBB(void)		{
	SQUARE		sq;

	SQ_LOOP(sq)
		SquareBB[sq] = SQ_BB(sq);
}


void	RankFileInitBB(void)	{
	SQUARE		sq;

	SQ_LOOP(sq)	{
		FileBB[sq] = (BITBOARD)(FILE_A_BB) << FILE(sq);
		RankBB[sq] = (BITBOARD)(RANK_1_BB) << (RANK(sq) * 8);
	}
}


void	ForwardInitBB(void)		{
	SQUARE		sq, to;
	TURN		side;
	int			front;


	memset(ForwardBB, 0, sizeof(ForwardBB));

	TURN_LOOP(side)				{
		front	= Front[side];

		SQ_LOOP(sq)
			for (to = sq + front; A1 <= to && to <= H8; to += front)
				ForwardBB[side][sq] |= SQ_BB(to);
	}
}


void	PawnPassedInitBB(void)	{
	SQUARE		sq, to;
	TURN		side;
	int			front;


	memset(PawnPassedBB, 0, sizeof(PawnPassedBB));

	TURN_LOOP(side)				{
		front	= Front[side];

		SQ_LOOP(sq)
			for (to = sq + front; A1 <= to && to <= H8; to += front)			{
				PawnPassedBB[side][sq]		|= SQ_BB(to);

				if (FILE(to) != FILE_A)
					PawnPassedBB[side][sq]	|= SQ_BB(to - 1);

				if (FILE(to) != FILE_H)
					PawnPassedBB[side][sq]	|= SQ_BB(to + 1);
			}
	}
}


void	OutpostInitBB(void)		{
	SQUARE		sq, to;
	TURN		side;
	int			front;


	memset(OutpostBB, 0, sizeof(OutpostBB));

	TURN_LOOP(side)				{
		front	= Front[side];

		SQ_LOOP(sq)
			for (to = sq + front; A1 <= to && to <= H8; to += front)			{
				if (FILE(to) != FILE_A)
					OutpostBB[side][sq]	|= SQ_BB(to - 1);

				if (FILE(to) != FILE_H)
					OutpostBB[side][sq]	|= SQ_BB(to + 1);
			}
	}
}


void	KingPatternInitBB(void)	{
	TURN		side;
	SQUARE		sq, to;
	int			front;


	memset(KingPatternBB, 0, sizeof(KingPatternBB));

	TURN_LOOP(side)				{
		front	= Front[side];

		SQ_LOOP(sq)				{
			to	= sq + 2 * front;

			if (A1 <= to && to <= H8)			{
				KingPatternBB[side][sq]		|= SQ_BB(to);

				if (FILE(to) != FILE_A)
					KingPatternBB[side][sq]	|= SQ_BB(to - 1);

				if (FILE(to) != FILE_H)
					KingPatternBB[side][sq]	|= SQ_BB(to + 1);
			}
		}
	}
}


void	KingFrontInitBB(void)	{
	TURN		side;
	SQUARE		sq, to;
	int			front;


	memset(KingFrontBB, 0, sizeof(KingFrontBB));

	TURN_LOOP(side)				{
		front	= Front[side];

		SQ_LOOP(sq)				{
			to	= sq + front;

			if (A1 <= to && to <= H8)			{
				KingFrontBB[side][sq]		|= SQ_BB(to);

				if (FILE(to) != FILE_A)
					KingFrontBB[side][sq]	|= SQ_BB(to - 1);

				if (FILE(to) != FILE_H)
					KingFrontBB[side][sq]	|= SQ_BB(to + 1);
			}
		}
	}
}


void	KingZoneInitBB(void)	{
	TURN		side;
	SQUARE		sq;

	SQ_LOOP(sq)
		TURN_LOOP(side)
			KingZoneBB[side][sq] = SquareBB[sq] | KingPatternBB[side][sq] | KingMoveBB[sq];
}


void	PawnAttackInitBB(void)	{
	TURN		side;
	SQUARE		sq, to;
	int			front;


	memset(PawnAttackBB, 0, sizeof(PawnAttackBB));

	TURN_LOOP(side)				{
		front	= Front[side];

		SQ_LOOP(sq)				{
			to	= sq + front;

			if (A1 <= to && to <= H8)			{
				if (FILE(to) != FILE_A)
					PawnAttackBB[side][sq]	|= SQ_BB(to - 1);

				if (FILE(to) != FILE_H)
					PawnAttackBB[side][sq]	|= SQ_BB(to + 1);
			}
		}
	}
}


void	PawnAdvanceInitBB(void)	{
	TURN		side;
	SQUARE		sq, to;
	int			front;


	memset(PawnAdvanceBB, 0, sizeof(PawnAdvanceBB));

	TURN_LOOP(side)				{
		front	= Front[side];

		SQ_LOOP(sq)				{
			to	= sq + front;

			if (A1 <= to && to <= H8)
				PawnAdvanceBB[side][sq]	= SQ_BB(to);
		}
	}
}


void	PawnAdvance2InitBB(void){
	SQUARE		sq;


	memset(PawnAdvance2BB, 0, sizeof(PawnAdvance2BB));

	for (sq = A2; sq <= H2; sq++)
		PawnAdvance2BB[WHITE][sq]	= SQ_BB(sq + 16);

	for (sq = A7; sq <= H7; sq++)
		PawnAdvance2BB[BLACK][sq]	= SQ_BB(sq - 16);
}


void	MobInitBB(void)			{
	SQUARE	sq, to_0x88, *delta;


	SQ_LOOP(sq)	{
		KnightMoveBB[sq] = KingMoveBB[sq] = BishopPseudoMoveBB[sq] = RookPseudoMoveBB[sq] = 0x0L;

		MOBILITY_LOOP(delta, KNIGHT)			{
			to_0x88 = SquareTO0x88[sq] + *delta;

			if (!OUT_BOARD(to_0x88))
				KnightMoveBB[sq]		|= SquareBB[SquareTO64[to_0x88]];
		}

		MOBILITY_LOOP(delta, KING)				{
			to_0x88 = SquareTO0x88[sq] + *delta;

			if (!OUT_BOARD(to_0x88))
				KingMoveBB[sq]			|= SquareBB[SquareTO64[to_0x88]];
		}

		MOBILITY_LOOP(delta, BISHOP)
			for (to_0x88 = SquareTO0x88[sq] + *delta; !OUT_BOARD(to_0x88); to_0x88 += *delta)
				BishopPseudoMoveBB[sq]	|= SquareBB[SquareTO64[to_0x88]];

		MOBILITY_LOOP(delta, ROOK)
			for (to_0x88 = SquareTO0x88[sq] + *delta; !OUT_BOARD(to_0x88); to_0x88 += *delta)
				RookPseudoMoveBB[sq]	|= SquareBB[SquareTO64[to_0x88]];

		QueenPseudoMoveBB[sq] = BishopPseudoMoveBB[sq] | RookPseudoMoveBB[sq];
	}
}


// Inicializa datos y varaiables para el funcionamiento de los Bitboard
void	BitboardInitBB(void)	{
	SQUARE		sq;

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


	SQ_LOOP(sq)	{
		Rvariation(sq);
		Bvariation(sq);
	}


	SQ_LOOP(sq)	{
		MagicRookMoveInit(sq);
		MagicBishopMoveInit(sq);
	}
}


// 500
