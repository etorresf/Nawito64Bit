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

#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>
#include <string.h>

#include "turn.h"
#include "piece.h"
#include "square.h"


//====================================================================================================================================
typedef	uint64_t	BITBOARD;


/*
 * Cuenta la cantidad de bits activos en 'BitBoard'.
 * Ver (SWAR-Popcount) en <www.chessprogramming.org/Home/Board Representation/Bitboards/Population Count>
 */
int		PopCountBB(BITBOARD bb);


// Inicializa las variables y herramientas de los Bitboards
void	BitboardInitBB(void);


// Arreglo necesario en la siguiente funcion
static const int	index64[64]	= {
	0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

/*
 * Extrae la posicion del bit menos significativo de 'BB'.
 * Ver (De Bruijn Multiplication) en <www.chessprogramming.org/Home/Board Representation/Bitboards/BitScan>.
 * Se usa un #define para obtener mas velocidad
 */
#define LSB_BB(BB)	index64[(((BB) ^ ((BB) - 1)) * ((BITBOARD)(0x03f79d71b4cb0a89L))) >> 58]


// Casillas Blancas y Negras con Bitboards
#define				BLACK_SQUARES_BB		0XAA55AA55AA55AA55L
#define				WHITE_SQUARES_BB		0X55AA55AA55AA55AAL


static const BITBOARD	SquareColorBB[64]		= {
	BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,
	WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,
	BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,
	WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,
	BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,
	WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,
	BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,
	WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,WHITE_SQUARES_BB,BLACK_SQUARES_BB,
};


// Columnas con Bitboards
#define				FILE_A_BB				0x101010101010101L
#define				FILE_B_BB				0x202020202020202L
#define				FILE_C_BB				0x404040404040404L
#define				FILE_D_BB				0x808080808080808L
#define				FILE_E_BB				0x1010101010101010L
#define				FILE_F_BB				0x2020202020202020L
#define				FILE_G_BB				0x4040404040404040L
#define				FILE_H_BB				0x8080808080808080L


// Filas con Bitboards
#define				RANK_1_BB				0xFFL
#define				RANK_2_BB				0xFF00L
#define				RANK_3_BB				0xFF0000L
#define				RANK_4_BB				0xFF000000L
#define				RANK_5_BB				0xFF00000000L
#define				RANK_6_BB				0xFF0000000000L
#define				RANK_7_BB				0xFF000000000000L
#define				RANK_8_BB				0xFF00000000000000L


static const BITBOARD	In7thBB[2]			= {RANK_1_BB | RANK_2_BB, RANK_8_BB | RANK_7_BB};


// Extraccion de una columna, fila y conversion a una casilla Bitboard
extern BITBOARD		FileBB[64];
extern BITBOARD		RankBB[64];
#define	SQ_BB(sq)	(((BITBOARD)0x1L) << (sq))

// Psuedo-movimientos de CABALLOS, Alfles, TORRES, DAMAS y REYES en Bitboard
extern BITBOARD		KnightMoveBB[64];
extern BITBOARD		BishopPseudoMoveBB[64];
extern BITBOARD		RookPseudoMoveBB[64];
extern BITBOARD		QueenPseudoMoveBB[64];
extern BITBOARD		KingMoveBB[64];


// Obtiene las capturas de PEON, avance de 1 y 2 pasos en Bitboard
extern BITBOARD		PawnAttackBB[2][64];
extern BITBOARD		PawnAdvanceBB[2][64];
extern BITBOARD		PawnAdvance2BB[2][64];


// Para la evaluacion de los PEONES, verificar su correcto calculo
extern BITBOARD		PawnPassedBB[2][64];

extern BITBOARD		ForwardBB[2][64];
extern BITBOARD		OutpostBB[2][64];

extern BITBOARD		KingPatternBB[2][64];
extern BITBOARD		KingFrontBB[2][64];
extern BITBOARD		KingZoneBB[2][64];


// Numeros magicos para calcular los movimientos de ALFILES y TORRES
extern BITBOARD		MagicMoveB[64][512];
extern BITBOARD		MagicMoveR[64][4096];


static const BITBOARD	MagicBnumber[64]		= {
	0x2910054208004104L,0x2100630a7020180L,0x5822022042000000L,0x2ca804a100200020L,0x204042200000900L,0x2002121024000002L,0x80404104202000e8L,0x812a020205010840L,
	0x8005181184080048L,0x1001c20208010101L,0x1001080204002100L,0x1810080489021800L,0x62040420010a00L,0x5028043004300020L,0xc0080a4402605002L,0x8a00a0104220200L,
	0x940000410821212L,0x1808024a280210L,0x40c0422080a0598L,0x4228020082004050L,0x200800400e00100L,0x20b001230021040L,0x90a0201900c00L,0x4940120a0a0108L,
	0x20208050a42180L,0x1004804b280200L,0x2048020024040010L,0x102c04004010200L,0x20408204c002010L,0x2411100020080c1L,0x102a008084042100L,0x941030000a09846L,
	0x244100800400200L,0x4000901010080696L,0x280404180020L,0x800042008240100L,0x220008400088020L,0x4020182000904c9L,0x23010400020600L,0x41040020110302L,
	0x412101004020818L,0x8022080a09404208L,0x1401210240484800L,0x22244208010080L,0x1105040104000210L,0x2040088800c40081L,0x8184810252000400L,0x4004610041002200L,
	0x40201a444400810L,0x4611010802020008L,0x80000b0401040402L,0x20004821880a00L,0x8200002022440100L,0x9431801010068L,0x1040c20806108040L,0x804901403022a40L,
	0x2400202602104000L,0x208520209440204L,0x40c000022013020L,0x2000104000420600L,0x400000260142410L,0x800633408100500L,0x2404080a1410L,0x138200122002900L
};

static const BITBOARD	MagicRnumber[64]		= {
	0xA180022080400230L,0x40100040022000L,0x80088020001002L,0x80080280841000L,0x4200042010460008L,0x4800A0003040080L,0x400110082041008L,0x8000A041000880L,
	0x10138001A080C010L,0x804008200480L,0x10011012000C0L,0x22004128102200L,0x200081201200CL,0x202A001048460004L,0x81000100420004L,0x4000800380004500L,
	0x208002904001L,0x90004040026008L,0x208808010002001L,0x2002020020704940L,0x8048010008110005L,0x6820808004002200L,0xA80040008023011L,0xB1460000811044L,
	0x4204400080008EA0L,0xB002400180200184L,0x2020200080100380L,0x10080080100080L,0x2204080080800400L,0xA40080360080L,0x2040604002810B1L,0x8C218600004104L,
	0x8180004000402000L,0x488C402000401001L,0x4018A00080801004L,0x1230002105001008L,0x8904800800800400L,0x42000C42003810L,0x8408110400B012L,0x18086182000401L,
	0x2240088020C28000L,0x1001201040C004L,0xA02008010420020L,0x10003009010060L,0x4008008008014L,0x80020004008080L,0x282020001008080L,0x50000181204A0004L,
	0x102042111804200L,0x40002010004001C0L,0x19220045508200L,0x20030010060A900L,0x8018028040080L,0x88240002008080L,0x10301802830400L,0x332A4081140200L,
	0x8080010A601241L,0x1008010400021L,0x4082001007241L,0x211009001200509L,0x8015001002441801L,0x801000804000603L,0xC0900220024A401L,0x1000200608243L,
};


// Desplazamiento de numeros magicos para calcular los movimientos de ALFILES y TORRES
static const int		MagicNumShiftB[64]		= {
	58,59,59,59,59,59,59,58,
	59,59,59,59,59,59,59,59,
	59,59,57,57,57,57,59,59,
	59,59,57,55,55,57,59,59,
	59,59,57,55,55,57,59,59,
	59,59,57,57,57,57,59,59,
	59,59,59,59,59,59,59,59,
	58,59,59,59,59,59,59,58,
};

static const int		MagicNumShiftR[64]		= {
	52,53,53,53,53,53,53,52,
	53,54,54,54,54,54,54,53,
	53,54,54,54,54,54,54,53,
	53,54,54,54,54,54,54,53,
	53,54,54,54,54,54,54,53,
	53,54,54,54,54,54,54,53,
	53,54,54,54,54,54,54,53,
	52,53,53,53,53,53,53,52,
};


// Mascaras de casillas ocupadas para calcular los movimientos de ALFILES y TORRES
static const BITBOARD	BoccupancyMask[64]		= {
	0x40201008040200L,0x402010080400L,0x4020100A00L,0x40221400L,0x2442800L,0x204085000L,0x20408102000L,0x2040810204000L,
	0x20100804020000L,0x40201008040000L,0x4020100A0000L,0x4022140000L,0x244280000L,0x20408500000L,0x2040810200000L,0x4081020400000L,
	0x10080402000200L,0x20100804000400L,0x4020100A000A00L,0x402214001400L,0x24428002800L,0x2040850005000L,0x4081020002000L,0x8102040004000L,
	0x8040200020400L,0x10080400040800L,0x20100a000A1000L,0x40221400142200L,0x2442800284400L,0x4085000500800L,0x8102000201000L,0x10204000402000L,
	0x4020002040800L,0x8040004081000L,0x100a000a102000L,0x22140014224000L,0x44280028440200L,0x8500050080400L,0x10200020100800L,0x20400040201000L,
	0x2000204081000L,0x4000408102000L,0xa000a10204000L,0x14001422400000L,0x28002844020000L,0x50005008040200L,0x20002010080400L,0x40004020100800L,
	0x20408102000L,0x40810204000L,0xa1020400000L,0x142240000000L,0x284402000000L,0x500804020000L,0x201008040200L,0x402010080400L,
	0x2040810204000L,0x4081020400000L,0xa102040000000L,0x14224000000000L,0x28440200000000L,0x50080402000000L,0x20100804020000L,0x40201008040200L
};

static const BITBOARD	RoccupancyMask[64]		= {
	0x101010101017EL,0x202020202027CL,0x404040404047AL,0x8080808080876L,0x1010101010106EL,0x2020202020205EL,0x4040404040403EL,0x8080808080807EL,
	0x1010101017E00L,0x2020202027C00L,0x4040404047A00L,0x8080808087600L,0x10101010106E00L,0x20202020205E00L,0x40404040403E00L,0x80808080807E00L,
	0x10101017E0100L,0x20202027C0200L,0x40404047A0400L,0x8080808760800L,0x101010106E1000L,0x202020205E2000L,0x404040403E4000L,0x808080807E8000L,
	0x101017E010100L,0x202027C020200L,0x404047A040400L,0x8080876080800L,0x1010106E101000L,0x2020205E202000L,0x4040403E404000L,0x8080807E808000L,
	0x1017E01010100L,0x2027C02020200L,0x4047A04040400L,0x8087608080800L,0x10106E10101000L,0x20205E20202000L,0x40403E40404000L,0x80807E80808000L,
	0x17E0101010100L,0x27C0202020200L,0x47A0404040400L,0x8760808080800L,0x106E1010101000L,0x205E2020202000L,0x403E4040404000L,0x807E8080808000L,
	0x7E010101010100L,0x7C020202020200L,0x7A040404040400L,0x76080808080800L,0x6E101010101000L,0x5E202020202000L,0x3E404040404000L,0x7E808080808000L,
	0x7E01010101010100L,0x7C02020202020200L,0x7A04040404040400L,0x7608080808080800L,0x6E10101010101000L,0x5E20202020202000L,0x3E40404040404000L,0x7E80808080808000L,
};


// Calculos de movimientos y capturas para las PIEZAS con Bitboards
#define				PAWN_ADVANCE_BB(Side, sq)		PawnAdvanceBB[Side][sq]
#define				PAWN_ADVANCE2_BB(Side, sq)		PawnAdvance2BB[Side][sq]
#define				KNIGHT_MOVES_BB(sq)				KnightMoveBB[sq]
#define				ROOK_MOVES_BB(sq, all_piece)	(MagicMoveR[sq][(int)((((all_piece) & RoccupancyMask[sq]) * MagicRnumber[sq]) >> MagicNumShiftR[sq])])
#define				BISHOP_MOVES_BB(sq, all_piece)	(MagicMoveB[sq][(int)((((all_piece) & BoccupancyMask[sq]) * MagicBnumber[sq]) >> MagicNumShiftB[sq])])
#define				QUEEN_MOVES_BB(sq, all_piece)	(ROOK_MOVES_BB(sq, all_piece) | BISHOP_MOVES_BB(sq, all_piece))
#define				KING_MOVES_BB(sq)				KingMoveBB[sq]

#define				A1_BB					0x1L


static const BITBOARD	KingCasttleBB[2]	= {0x6000000000000000L, 0x60L,};
static const BITBOARD	QueenCasttleBB[2]	= {0xE00000000000000L,  0xEL,};


// Determina casillas de promocion en BITBOARD
#define				PROM_SQ_BB(sq_BB)				((sq_BB) & (RANK_1_BB | RANK_8_BB))

// Determina casilla de promocion de un bando y su casilla
#define				PROM_SQ(sq, side)				LSB_BB(PROM_SQ_BB(ForwardBB[side][sq]))


// 200


#endif // BITBOARD_H
