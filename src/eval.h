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

#ifndef EVAL_H
#define EVAL_H


#include "move_gen.h"


extern int			Tropism[64][64];
extern int			BishopTropism[64][64];

#define				QUEEN_TROPISM(QueenSQ, KinkSQ)		((5 * Tropism[QueenSQ][KinkSQ]) / 2)
#define				ROOK_TROPISM(RookSQ, KinkSQ)		(Tropism[RookSQ][KinkSQ] / 2)
#define				KNIGHT_TROPISM(KnightSQ, KinkSQ)	(Tropism[KnightSQ][KinkSQ])
#define				BISHOP_TROPISM(BishopSQ, KinkSQ)	(BishopTropism[BishopSQ][KinkSQ])


extern int			GamePhase;

extern BITBOARD		PawnAttBB[2];
extern BITBOARD		PieceAttBB[2];
extern BITBOARD		KingAttBB[2];
extern BITBOARD		WeakPieceBB[2];


extern int			Weight[32];

extern int*			MidGameMat;
extern int*			EndGameMat;
extern int*			MidGameMob;
extern int*			EndGameMob;
extern int*			MidGamePawn;
extern int*			EndGamePawn;
extern int*			TrappedPiece;
extern int*			KingSafety;

extern int*			PassedPawns;
extern int*			IsolatedPawns;

extern int*			AttackWeight;
extern int*			AttackCount;

extern BITBOARD		KING_ZONE[2];


int					Eval(int Alpha, int Beta);					// Evaluación de la posición(completa o rapida)

int					FullEval(void);								// Evaluación completa de la posicion

// Evaluacion de las constelaciones del material
void				MaterialEval(void);
void				MaterialEvalEnd(void);

U8					IsPawnCandidate(SQUARE Square);				// Peón semi-pasado
U8					IsPawnBackward(SQUARE Square);				// Peón retrasado
U8					IsPawnAdvanced(SQUARE Square);				// Peón avanzado

int					IsPawnPassedBloqued(SQUARE BloquedSquare, SQUARE Square);

// Evalúa Peones
void				PawnEval(void);
void				PawnEvalEnd(void);

// Evalúa Caballos
void				KnightEval(void);
void				KnightEvalEnd(void);

// Evalúa Alfiles
void				BishopEval(void);
void				BishopEvalEnd(void);


// Evalúa Torres
void				RookEval(void);
void				RookEvalEnd(void);

// Evalúa Damas
void				QueenEval(void);
void				QueenEvalEnd(void);

// Evalúa Reyes
void				KingEval(void);
void				KingEvalEnd(void);

int					KingPawnShield(SQUARE Square, TURN Side);	// Evalua una de las columnas del escudo del Rey para un bando

#define				DRAW_SCORE(MatS)		(728 - (MatS) / 5)

U8					MatInsuf(void);								// Detecta tablas por insuficiencia material
int					SupportedEndGames(void);					// Finales soportados por el motor

void				EvalIni(void);								// Precalcula datos necesarios en la evaluacion

void				PutAttack(TURN Side, BITBOARD BB, int AttWeight);

#define				EV_ATTACKED_BB(Side)						(PawnAttBB[Side] | PieceAttBB[Side] | KingAttBB[Side])
#define				EV_ATTACKED(Square, Side)					(SquareBB[Square] & EV_ATTACKED_BB(Side))
#define				EV_ATTACKED_WITHOUT_KING_BB(Side)			(PawnAttBB[Side] | PieceAttBB[Side])


//==============Piece Square Table(PST). Evalúa las piezas por su tipo, posición en el tablero y fase de juego================

static const int	PawnMidGamePST[64]		= {
	100, 100, 100, 100, 100, 100, 100, 100,
	104, 110, 114, 120, 120, 114, 110, 104,
	104, 108, 112, 116, 116, 112, 108, 104,
	102, 106, 108, 112, 112, 108, 106, 102,
	102, 104, 108, 108, 108, 106, 104, 102,
	102, 102, 102, 104, 104, 102, 102, 102,
	100, 100, 100,  84,  84, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
};
static const int	PawnEndGamePST[64]		= {
	100, 100, 100, 100, 100, 100, 100, 100,
	104, 110, 114, 120, 120, 114, 110, 104,
	104, 108, 112, 116, 116, 112, 108, 104,
	102, 106, 108, 112, 112, 108, 106, 102,
	102, 104, 106, 108, 108, 106, 104, 102,
	100, 102, 102, 104, 104, 102, 102, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100,
};

static const int	KnightMidGamePST[64]	= {
	172, 282, 292, 304, 304, 292, 282, 172,
	286, 304, 314, 320, 320, 314, 304, 286,
	310, 320, 330, 334, 334, 330, 320, 310,
	310, 320, 330, 334, 334, 330, 320, 310,
	304, 314, 324, 330, 330, 324, 314, 304,
	282, 304, 314, 320, 320, 314, 304, 282,
	272, 282, 298, 304, 304, 298, 282, 272,
	256, 266, 276, 282, 282, 276, 266, 256,
};
static const int	KnightEndGamePST[64]	= {
	274, 284, 294, 300, 300, 294, 284, 274,
	284, 294, 304, 310, 310, 304, 294, 284,
	294, 304, 314, 320, 320, 314, 304, 294,
	300, 310, 320, 324, 324, 320, 310, 300,
	300, 310, 320, 324, 324, 320, 310, 300,
	294, 304, 314, 320, 320, 314, 304, 294,
	284, 294, 304, 310, 310, 304, 294, 284,
	274, 284, 294, 300, 300, 294, 284, 274,
};

static const int	BishopMidGamePST[64]	= {
	306, 306, 308, 310, 310, 308, 306, 306,
	306, 320, 318, 320, 320, 318, 320, 306,
	308, 318, 324, 322, 322, 324, 318, 308,
	310, 320, 322, 328, 328, 322, 320, 310,
	310, 320, 322, 328, 328, 322, 320, 310,
	308, 318, 324, 322, 322, 324, 318, 308,
	306, 320, 318, 320, 320, 318, 320, 306,
	296, 296, 294, 300, 300, 294, 296, 296,
};
static const int	BishopEndGamePST[64]	= {
	302, 308, 310, 314, 314, 310, 308, 302,
	308, 314, 316, 320, 320, 316, 314, 308,
	310, 316, 320, 322, 322, 320, 316, 310,
	314, 320, 322, 328, 328, 322, 320, 314,
	314, 320, 322, 328, 328, 322, 320, 314,
	310, 316, 320, 322, 322, 320, 316, 310,
	308, 314, 316, 320, 320, 316, 314, 308,
	302, 308, 310, 314, 314, 310, 308, 302,
};

static const int	RookMidGamePST[64]		= {
	496, 500, 502, 506, 506, 502, 500, 496,
	494, 496, 500, 502, 502, 500, 496, 494,
	496, 500, 502, 506, 506, 502, 500, 496,
	488, 490, 494, 496, 496, 494, 490, 488,
	490, 494, 496, 500, 500, 496, 494, 490,
	494, 496, 500, 502, 502, 500, 496, 494,
	496, 498, 502, 504, 504, 502, 498, 496,
	498, 502, 504, 508, 508, 504, 502, 498,
};
static const int	RookEndGamePST[64]		= {
	500, 502, 506, 508, 508, 506, 502, 500,
	494, 496, 500, 502, 502, 500, 496, 494,
	496, 500, 502, 506, 506, 502, 500, 496,
	488, 490, 494, 496, 496, 494, 490, 488,
	490, 494, 496, 500, 500, 496, 494, 490,
	494, 496, 500, 502, 502, 500, 496, 494,
	496, 498, 502, 504, 504, 502, 498, 496,
	498, 502, 504, 508, 508, 504, 502, 498,
};

static const int	QueenMidGamePST[64]		= {
	980, 990, 990, 994, 994, 990, 990, 980,
	998,1008,1008,1008,1008,1008,1008, 998,
	990,1000,1004,1004,1004,1004,1000, 990,
	994,1000,1004,1004,1004,1004,1000, 994,
   1000,1000,1004,1004,1004,1004,1000, 994,
	990,1004,1004,1004,1004,1004,1000, 950,
	990,1000,1100,1000,1000,1000,1000, 990,
	980, 990, 990, 994, 994, 990, 990, 980,
};
static const int	QueenEndGamePST[64]		= {
	950, 958, 962, 966, 966, 962, 958, 950,
	974, 982, 986, 990, 990, 986, 982, 974,
	962, 970, 974, 978, 978, 974, 970, 962,
	966, 974, 978, 982, 982, 978, 974, 966,
	966, 974, 978, 982, 982, 978, 974, 966,
	962, 970, 974, 978, 978, 974, 970, 962,
	974, 982, 986, 990, 990, 986, 982, 974,
	950, 958, 962, 966, 966, 962, 958, 950,
};

// Incluye pts por rey enrocado
static const int	KingMidGamePST[64]		= {
	160, 170, 150, 100, 100, 150, 170, 160,
	160, 170, 150, 100, 100, 150, 170, 160,
	180, 190, 170, 110, 110, 170, 190, 180,
	200, 210, 190, 130, 130, 190, 210, 200,
	220, 230, 210, 150, 150, 210, 230, 220,
	240, 250, 230, 170, 170, 230, 250, 240,
	260, 270, 250, 190, 190, 250, 270, 260,
	270, 280, 260, 200, 200, 260, 280, 270,
};
static const int	KingEndGamePST[64]		= {
	118, 142, 154, 166, 166, 154, 142, 118,
	142, 166, 178, 190, 190, 178, 166, 142,
	154, 178, 190, 202, 202, 190, 178, 154,
	166, 190, 202, 214, 214, 202, 190, 166,
	166, 190, 202, 214, 214, 202, 190, 166,
	154, 178, 190, 202, 202, 190, 178, 154,
	142, 166, 178, 190, 190, 178, 166, 142,
	118, 142, 154, 166, 166, 154, 142, 118,
};


//==============Material. Evalúa otros datos relacionados con el material=====================================================
// Penalización, redundancia de piezas cuando se tiene mas de 1 caballo o torre
static const int	KnightPair				= 8;
static const int	RookPair				= 16;


// Penalización, menos peones mas posibilidad de tablas
static const int	PawnAdjust[9]			= {50, 28, 18, 4, 0, 0,0,0,0,};


// Bonus, incrementa su valor en posiciones cerradas(cuando hay muchos peones)
static const int	KnightAdjust[9]			= {-20, -16, -12, -8, -4, 0, 4, 8, 12,};


// Bonus, incrementa su valor en posiciones abiertas(cuando hay pocos peones)
static const int	RookAdjust[9]			= { 16, 12, 8, 6, 2, 0, -2, -6, -8,};


/*
 * Bonus, pareja de alfiles
 *
 * 1) Incrementa su valor en posiciones abiertas(cuando hay pocos peones)
 * 2) Incrementa su valor si el rival no posee alfiles o caballos
 */
static const int	BishopPair[17]			= { 50, 48, 48, 46, 44, 44, 42, 40, 40, 38, 38, 36, 34, 34, 32, 30, 30,};
static const int	WithOutBishop			= 20;


// Bonus, por estar presente algunas configuraciones de material
static const int	RookVSpawnsMid			= 200;
static const int	RookVSpawnsEnd			= 50;

static const int	RookVSknightMid			= 50;
static const int	RookVSknightEnd			= 30;

static const int	KnightVSpawnsMid		= 100;
static const int	KnightVSpawnsEnd		= 50;

static const int	KnightBishopVSrookMid	= 50;
static const int	KnightBishopVSrookEnd	= 24;


//==============Estructura de peones. Evalúa algunos de los tipos de peones===================================================

/*
 * Peones doblados en medio juego
 *
 * 1) Penaliza distancia hacia al centro, entre más cerca más valor
 * 2) Penaliza peones en columnas torre, también son aislados y son más difíciles de desdoblar
 */
static const int	PawnDoubled[64]			= {
	 0,  0,  0,  0,  0,  0,  0,  0,
	16, 10, 12, 14, 14, 12, 10, 16,
	16, 10, 12, 14, 14, 12, 10, 16,
	16, 10, 12, 14, 14, 12, 10, 16,
	16, 10, 12, 14, 14, 12, 10, 16,
	16, 10, 12, 14, 14, 12, 10, 16,
	16, 10, 12, 14, 14, 12, 10, 16,
	 0,  0,  0,  0,  0,  0,  0,  0,
};

// Peones aislados
static const int	PawnIsolated[64]		= {
	 0,  0,  0,  0,  0,  0,  0,  0,
	22, 24, 26, 28, 28, 26, 24, 22,
	18, 20, 22, 24, 24, 22, 20, 18,
	14, 16, 18, 20, 20, 18, 16, 14,
	12, 14, 16, 18, 18, 16, 14, 12,
	10, 12, 14, 16, 16, 14, 12, 10,
	10, 12, 14, 16, 16, 14, 12, 10,
	 0,  0,  0,  0,  0,  0,  0,  0,
};

// Peones retrasados
static const int	PawnBackward[64]		= {
	 0,  0,  0,  0,  0,  0,  0,  0,
	18, 20, 22, 24, 24, 22, 20, 18,
	16, 18, 20, 22, 22, 20, 18, 16,
	14, 16, 18, 20, 20, 18, 16, 14,
	12, 14, 16, 18, 18, 16, 14, 12,
	10, 12, 14, 16, 16, 14, 12, 10,
	10, 12, 14, 16, 16, 14, 12, 10,
	 0,  0,  0,  0,  0,  0,  0,  0,
};


/*
 * Peones pasados en medio juego
 *
 * 1) Bonifica distancia hacia al centro, entre más cerca más valor
 * 2) Bonifica distancia hacia casilla de promoción, entre más cerca más valor
 */
static const int	PawnPassed[64]			= {
	 0,  0,  0,  0,  0,  0,  0,  0,
	70, 70, 70, 70, 70, 70, 70, 70,
	46, 46, 46, 46, 46, 46, 46, 46,
	28, 28, 28, 28, 28, 28, 28, 28,
	16, 16, 16, 16, 16, 16, 16, 16,
	10, 10, 10, 10, 10, 10, 10, 10,
	10, 10, 10, 10, 10, 10, 10, 10,
	 0,  0,  0,  0,  0,  0,  0,  0,
};


// Penalización, peones pasados bloqueados no son tan buenos
static const int	KingBloqPawn			= 25;
static const int	KnightBloqPawn			= 15;
static const int	BishopBloqPawn			= 10;


//==============Movilidad de las piezas. Evalúa la movilidad de las piezas según su tipo y fase de juego======================
// Bonus, por llevar la iniciativa en le juego
static const int	Tempo					= 10;


// Penalización por piezas bloqueadas, dificultan la movilidad a otras piezas o así mismas
static const int	BlockedPawnC2			= 16;
static const int	BlockedCentralPawn		= 20;


// Penalización, alfil malo. Se incrementa en el final con idea de intercambiar piezas
static const int	BadBishop[9]			= {0, 16, 32, 48, 64, 80, 96,112,128,};

// Bonus, Caballo protegido por peones e in-desalojable
static const int	KnightOutpost[64]		= {
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  4,  6,  6,  4,  0,  0,
	0,  2,  6, 10, 10,  6,  2,  0,
	0,  2,  6, 10, 10,  6,  2,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
};


// Penalización por piezas atrapadas, con muy poca movilidad, posiblemente sean capturadas
static const int	KnightTrappedA8			= 150;
static const int	KnightTrappedA7			= 100;

static const int	BishopTrappedA7			= 50;
static const int	BishopTrappedA6			= 50;
static const int	BishopTrappedB8			= 150;

static const int	RookTrapped				= 70;


// Penalización. El rival con amenazas a la dama puede desarrollarse
static const int	QueenPremature			= 18;


// Bonus, movilidad de caballos. Se decrementa en el final para evitar cambios si se tiene mayor movilidad
#define				KNIGHT_IMP4				0x3C3C3C3C0000L
#define				KNIGHT_IMP2				0x7E424242427E00L


static const int	KnightMob[32]			= {-12,-11,-10,-8,-7,-6,-5,-4,-2,-1,0,1,2,4,5,6,7,8,10,11,12,13,14,16,17,18,19,20,21,22,23,24,};
static const int	BishopMob[16]			= {-20,-12,-4,4,12,18,24,30,38,44,48,52,54,56,60,};
static const int	RookMob[32]				= {-12,-10,-8,-5,-3,0,3,6,8,10,13,15,18,21,24,26,28,30,33,35,37,39,41,43,44,46,47,48,49,50,52,53,};
static const int	QueenMob[32]			= {-10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10, 12, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,};


// Bonus, movilidad de rey. De momento solo usado en el final
static const int	KingEndGameMob			= 16;

// Torres en columnas abiertas semi-abiertas y en 7ma
static const int	RookIn7th				= 22;
static const int	RookInOpenFile			= 16;
static const int	RookInHalfOpenFile		= 10;


// Torres y Damas dobladas en columnas abiertas
static const int	RookSuppRook			= 10;
static const int	QueenSuppRook			= 6;


// Bonificación para otras valoraciones de peones pasados
static const int	PassedPawnPotential[64]	= {
	10, 10, 10, 10, 10, 10, 10, 10,
	10, 10, 10, 10, 10, 10, 10, 10,
	 6,  6,  6,  6,  6,  6,  6,  6,
	 3,  3,  3,  3,  3,  3,  3,  3,
	 1,  1,  1,  1,  1,  1,  1,  1,
	 0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,
	 0,  0,  0,  0,  0,  0,  0,  0,
};

static const int	RookBlockPassedPawn		= 30;


//==============Seguridad del rey. Evalúa la seguridad del rey================================================================

// Penalización, por peones movidos del escudo del rey
static const int	NoPawnInFile			= 30;
static const int	Pawn4th					= 24;
static const int	Pawn3th					= 12;

static const int	NoEnemyPawnInFile		= 18;
static const int	EnemyPawn3th			= 12;
static const int	EnemyPawn4th			= 6;


// Penalización o Bonus, por hacer o perder el enroque
static const int	CasttleDone				= 40;

static const int	KingInOpenFile			= 12;

// Penalización o Bonus, por tener o perder el Fianchetto
static const int	Fianchetto				= 12;


// Penalización, determina que tan seguro esta el rey según tipo y cantidad de piezas enemiga atacándolo
static const int	SafetyTable[100]		= {
	0,0,1,2,3,5,7,9,12,15,18,22,26,30,35,39,44,50,56,62,68,75,82,85,89,97,105,113,122,131,140,
	150,169,180,191,202,213,225,237,248,260,272,283,295,307,319,330,342,354,366,377,389,401,412,
	424,436,448,459,471,483,494,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,
	500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,
};

// Penalización, rey en centro del tablero o territorio enemigo
static const int	SRKingCount[64]			= {
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 3, 3, 3, 3, 3, 3, 4,
	2, 1, 1, 1, 1, 1, 1, 2,
	2, 0, 0, 0, 0, 0, 0, 2,
};


//==============OTROS===========================================================================================
static const int	LazyScore[128]			= {
	0,990,955,920,885,850,815,780,745,710,675,640,605,570,535,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,
	500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,		// final
	478,456,435,413,391,369,347,325,303,281,260,238,216,194,183,172,		// mediojuego - final	(OK)
	150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,// mediojuego
	150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,
};


// Para finales de alfiles
static const int	DiferentBishop[17]		= {MATE, 256, 128, 84, 64, 50, 42, 36, 32, 28, 26, 22, 20, 20, 18, 16, 16,};

// Hace menor la evaluación entre menos peones en finales tablas
static const int	DiffEndGame[17]			= {32,30,28,26,26,24,22,20,18,16,16,16,16,16,16,16,16,};



#endif // EVAL_H

