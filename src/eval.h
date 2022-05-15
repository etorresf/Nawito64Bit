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

#ifndef EVAL_H
#define EVAL_H

#include "board.h"
#include "eval_param.h"


//====================================================================================================================================
extern BITBOARD		PawnAttBB[2];
extern BITBOARD		PieceAttBB[2];
extern BITBOARD		KingAttBB[2];
extern BITBOARD		KING_ZONE[2];

// Almacenamiento de la evaluacion por fase de juego y otros parametros
extern int			Weight[16];
extern int*			MidGame;
extern int*			EndGame;
extern int*			MidEndGame;
extern int*			AttackWeight;
extern int*			AttackCount;
extern int*			KingSafety;
extern int*			RookTrapped;

// Almacena para el NEGRO y BLANCO las casillas atacadas por: Peones, Piezas, Rey en Bitboard
extern BITBOARD		PawnAttBB[2];


//====================================================================================================================================
int					Eval(int alpha, int beta);					// Evaluación de la posición(completa o rapida)
int					MatInsuf();									// Detecta tablas por insuficiencia material
void				EvalIni();									// Precalcula datos necesarios en la evaluacion
int					SupportedEndGames();


//====================================================================================================================================
/// TRATAR DE ELIMINAR ESTE ARREGLOS
static const int	KnightOutpost[64]	= {
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	2,	3,	3,	2,	0,	0,
	0,	1,	3,	5,	5,	3,	1,	0,
	0,	1,	3,	5,	5,	3,	1,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
};

#define		KNIGHT_OUTPOST_BB		0x3C7E7E000000L

#define		KNIGHT_TRAPPED_A1_BB	0x8100000000000000L
#define		KNIGHT_TRAPPED_A2_BB	0x81000000000000L
#define		KNIGHT_TRAPPED_A1_A2_BB	(KNIGHT_TRAPPED_A1_BB | KNIGHT_TRAPPED_A2_BB)

#define		BISHOP_TRAPPED_A2_BB	0x4281000000000000L
#define		BISHOP_TRAPPED_A3_BB	0x8142810000000000L
#define		BISHOP_TRAPPED_A2_A3_BB	(BISHOP_TRAPPED_A2_BB | BISHOP_TRAPPED_A3_BB)


//====================================================================================================================================
// Penalización, determina que tan seguro esta el rey según tipo y cantidad de piezas enemiga atacándolo
static const int	SafetyTable[100]		= {
	0,0,1,2,3,5,7,9,12,15,18,22,26,30,35,39,44,50,56,62,68,75,82,85,89,97,105,113,122,131,140,
	150,169,180,191,202,213,225,237,248,260,272,283,295,307,319,330,342,354,366,377,389,401,412,
	424,436,448,459,471,483,494,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,
	500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,
};

/// no me gusta para nada esto
extern int			BishopTropism[64][64];


#define		OPEN_FILE(sq, side)			(!(FileBB[sq] & PAWN_BB(side)))							// Determina si la columna de una casilla no tiene Peones de un bando
#define		GET_FILE_STATE(sq, side)	(2 * OPEN_FILE(sq, side) + OPEN_FILE(sq, (side) ^ 1))

// Tipos de fila en cuanto a la existencia de Peones
typedef enum	{
	FILE_CLOSED,
	FILE_HALF_OPEN	= 2,
	FILE_OPEN,
}	FILE_STATE;


#define		BAD_B5		0x1818000000L
#define		BAD_B3		0x3C24243C0000L
#define		BAD_B2		0x3C424242423CL
#define		BAD_B1		0xC381818181C300


#define		EV_ATTACKED_BB(side)	(PawnAttBB[side] | PieceAttBB[side] | KingAttBB[side])	// Obtiene todas las casillas atacadas por un bando en Bitboard
#define		EV_ATTACKED(sq, side)	(SQ_BB(sq) & EV_ATTACKED_BB(side))						// Determina si una casilla es atacada por un bando en Bitboard
#define		EV_ATTACKED_WITHOUT_KING_BB(side)	(PawnAttBB[side] | PieceAttBB[side])		// Obtiene todas las casillas atacadas(excepto Rey) por un bando en Bitboard

#define		DRAW_SCORE(mat_sum)		(2048 - 64 * MIN(78, mat_sum / 100))					// Ayuda a determinar la puntuacion de una posicion tablas de acuerdo al material. 0 <= mat_sum <= 78


/*
 * Base de datos(bitboard), segun donde esta el rey(sin peon) determina si es tablas.
 * Tambien funciona para mas de 3 peones doblados.
 * Puede fallar cuando existen varios peones y pueden avansar, la idea es que la busqueda compense este error.
 */
static const BITBOARD	KQKP_DB1[2]		= {0xE0E0E0E0F0FFFFFFL,	0xFFFFFFF0E0E0E0E0L,};
static const BITBOARD	KQKP_DB2[2]		= {0x70707070FFFFFFFL,	0xFFFFFF0F07070707L,};
static const BITBOARD	KQKP_DB3[2]		= {0xE0E0F0F8FFFFFFFFL,	0xFFFFFFFFF8F0E0E0L,};
static const BITBOARD	KQKP_DB4[2]		= {0x7070F1FFFFFFFFFL,	0xFFFFFFFF1F0F0707L,};


// Determina que tan importante son algunas casillas para la mobilidad
#define				IMP4					0x3C3C3C3C0000L
#define				IMP2					0x7E424242427E00L
#define				IMP1					0x7E8181818181817EL


#define				ARRIM_BB				0xFF818181818181FFL
#define				BLACK_CORNER_BB			0xEC0800000010307L
#define				WHITE_CORNER_BB			0x70301000080C0E0L
#define				ARRIM_WHITE_CORNER_BB	0x707070000E0E0E0L
#define				ARRIM_BLACK_CORNER_BB	0xE0E0E00000070707L

#define				ROOK_KNIGHT_FILE_BB		0xC3C3C3C3C3C3C3C3L


#define				CENTER_WEIGHT(bb, w1)	(4 * PopCountBB((bb) & IMP4) + 2 * PopCountBB((bb) & IMP2) + (w1) * PopCountBB((bb) & IMP1))


#endif // EVAL_H


//	100
