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


#ifndef EVAL_H
#define EVAL_H


#include "move_gen.h"



extern int			Tropism[64][64];
extern int			BishopTropism[64][64];

#define				QUEEN_TROPISM(sq, kink_sq)		((5 * Tropism[sq][kink_sq]) / 2)
#define				ROOK_TROPISM(sq, kink_sq)		(Tropism[sq][kink_sq] / 2)
#define				KNIGHT_TROPISM(sq, kink_sq)		(Tropism[sq][kink_sq])
#define				BISHOP_TROPISM(sq, kink_sq)		(BishopTropism[sq][kink_sq])


extern int			GamePhase;

extern BITBOARD		PawnAttBB[2];
extern BITBOARD		PieceAttBB[2];
extern BITBOARD		KingAttBB[2];


//==============================================================
extern int			Weight[16];

extern int*			PassedPawns;
extern int*			IsolatedPawns;

extern int*			AttackWeight;
extern int*			AttackCount;

extern int*			MidGame;
extern int*			EndGame;
//==============================================================



//==============================================================
extern int			EvWeight[64];

extern int*			KingTropism;
extern int*			KingShield;
extern int*			KingAttack;

extern int*			KingSafetyMidGame;
extern int*			KingSafetyEndGame;

extern int*			ActMidGame;
extern int*			ActEndGame;

extern int*			MaterialMidGame;
extern int*			MaterialEndGame;

extern int*			MobilityMidGame;
extern int*			MobilityEndGame;

extern int*			WeakPawnMidGame;
extern int*			WeakPawnEndGame;
extern int*			PassedPawnMidGame;
extern int*			PassedPawnEndGame;

extern int*			TrappedPiece;
//==============================================================


extern BITBOARD		KING_ZONE[2];


int					Eval(int alpha, int beta);					// Evaluación de la posición(completa o rapida)

int					FullEval(void);								// Evaluación completa de la posicion

// Evaluacion de las constelaciones del material
void				MaterialEval(void);
void				MaterialEvalEnd(void);

U8					IsPawnCandidate(SQUARE sq);					// Peón semi-pasado
U8					IsPawnBackward(SQUARE sq);					// Peón retrasado
U8					IsPawnAdvanced(SQUARE sq);					// Peón avanzado

int					IsPawnPassedBloqued(SQUARE bloqued_sq, SQUARE sq);

void				PawnEval(U8 simple_end);
void				KnightEval(U8 simple_end);
void				BishopEval(U8 simple_end);
void				RookEval(U8 simple_end);
void				QueenEval(U8 simple_end);
void				KingEval(U8 simple_end);


// Ayuda a determinar la puntuacion de una posicion tablas de acuerdo al material, diferencia material y distancia a algun pto.
#define				DRAW_SCORE(mat, mat_diff)				(int)((512.0 - 0.063589743589744 * (double)(mat)) * MAX(1.0, (double)(mat_diff) / 256.0))
#define				DRAW_DIST_SCORE(mat, mat_diff, Dist)	(int)(((256.0 - 0.031794871794872 * (double)(mat)) / (2.0 + (double)(Dist))) * MAX(1.0, (double)(mat_diff) / 256.0))


U8					MatInsuf(void);								// Detecta tablas por insuficiencia material
int					SupportedEndGames(void);					// Finales soportados por el motor

void				EvalIni(void);								// Precalcula datos necesarios en la evaluacion

void				PutAttack(TURN side, BITBOARD bb, int king_att_weight);

#define				EV_ATTACKED_BB(side)				(PawnAttBB[side] | PieceAttBB[side] | KingAttBB[side])
#define				EV_ATTACKED(sq, side)				(SquareBB[sq] & EV_ATTACKED_BB(side))
#define				EV_ATTACKED_WITHOUT_KING_BB(side)	(PawnAttBB[side] | PieceAttBB[side])


//
static const int	PawnMidGameMat			= 100;
static const int	PawnEndGameMat			= 120;
static const int	KnightMidGameMat		= 320;
static const int	KnightEndGameMat		= 330;
static const int	BishopMidGameMat		= 324;
static const int	BishopEndGameMat		= 334;
static const int	RookMidGameMat			= 500;
static const int	RookEndGameMat			= 510;
static const int	QueenMidGameMat			= 1000;
static const int	QueenEndGameMat			= 950;


//==============Piece Square Table(PST). Evalúa las piezas por su tipo, posición en el tablero y fase de juego================

static const int	PawnMidGamePST[64]		= {
	0,  0,  0,  0,  0,  0,  0,  0,
	4, 10, 14, 20, 20, 14, 10,  4,
	4,  8, 12, 16, 16, 12,  8,  4,
	2,  6,  8, 12, 12,  8,  6,  2,
	2,  4,  8,  8,  8,  6,  4,  2,
	2,  2,  2,  4,  4,  2,  2,  2,
	0,  0,  0,-16,-16,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
};
static const int	PawnEndGamePST[64]		= {
	0,  0,  0,  0,  0,  0,  0,  0,
	4, 10, 14, 20, 20, 14, 10,  4,
	4,  8, 12, 16, 16, 12,  8,  4,
	2,  6,  8, 12, 12,  8,  6,  2,
	2,  4,  6,  8,  8,  6,  4,  2,
	0,  2,  2,  4,  4,  2,  2,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,
};

static const int	KnightMidGamePST[64]	= {
   -128, -18,  -8,   4,   4,  -8, -18,-128,
	-14,   4,  14,  20,  20,  14,   4, -14,
	 10,  20,  30,  34,  34,  30,  20,  10,
	 10,  20,  30,  34,  34,  30,  20,  10,
	  4,  14,  24,  30,  30,  24,  14,   4,
	-18,   4,  14,  20,  20,  14,   4, -18,
	-28, -18,  -2,   4,   4,  -2, -18, -28,
	-44, -34, -24, -18, -18, -24, -34, -44,
};
static const int	KnightEndGamePST[64]	= {
	-26, -16,  -6,   0,   0,  -6, -16, -26,
	-16,  -6,   4,  10,  10,   4,  -6, -16,
	 -6,   4,  14,  20,  20,  14,   4,  -6,
	  0,  10,  20,  24,  24,  20,  10,   0,
	  0,  10,  20,  24,  24,  20,  10,   0,
	 -6,   4,  14,  20,  20,  14,   4,  -6,
	-16,  -6,   4,  10,  10,   4,  -6, -16,
	-26, -16,  -6,   0,   0,  -6, -16, -26,
};

static const int	BishopMidGamePST[64]	= {
	2,  2,  4,  6,  6,  4,  2,  2,
	2, 16, 14, 16, 16, 14, 16,  2,
	4, 14, 20, 18, 18, 20, 14,  4,
	6, 16, 18, 24, 24, 18, 16,  6,
	6, 16, 18, 24, 24, 18, 16,  6,
	4, 14, 20, 18, 18, 20, 14,  4,
	2, 16, 14, 16, 16, 14, 16,  2,
   -8, -8,-10, -4, -4,-10, -8, -8,
};
static const int	BishopEndGamePST[64]	= {
	-2,  4,  6, 10, 10, 6,  4,  -2,
	 4, 10, 12, 16, 16, 12, 10,  4,
	 6, 12, 16, 18, 18, 16, 12,  6,
	10, 16, 18, 24, 24, 18, 16, 10,
	10, 16, 18, 24, 24, 18, 16, 10,
	 6, 12, 16, 18, 18, 16, 12,  6,
	 4, 10, 12, 16, 16, 12, 10,  4,
	-2,  4,  6, 10, 10, 6,  4,  -2,
};

static const int	RookMidGamePST[64]		= {
	-4,  0,  2,  6,  6,  2,  0,  6,
	-6, -4,  0,  2,  2,  0, -4, -6,
	-4,  0,  2,  6,  6,  2,  0, -4,
   -12,-10, -6, -4, -4, -6,-10,-12,
   -10, -6, -4,  0,  0, -4, -6,-10,
	-6, -4,  0,  2,  2,  0, -4, -6,
	-4, -2,  2,  4,  4,  2, -2, -4,
	-2,  2,  4,  8,  8,  4,  2, -2,
};
static const int	RookEndGamePST[64]		= {
	 0,  2,  6,  8,  8,  6,  2,  0,
	-6, -4,  0,  2,  2,  0, -4, -6,
	-4,  0,  2,  6,  6,  2,  0, -4,
   -12,-10, -6, -4, -4, -6,-10,-12,
   -10, -6, -4,  0,  0, -4, -6,-10,
	-6, -4,  0,  2,  2,  0, -4, -6,
	-4, -2,  2,  4,  4,  2, -2, -4,
	-2,  2,  4,  8,  8,  4,  2, -2,
};

static const int	QueenMidGamePST[64]		= {
	 4, 14, 14, 20, 20, 14, 14,  4,
	48, 58, 58, 58, 58, 58, 58, 48,
	14, 24, 30, 30, 30, 30, 24, 14,
	20, 24, 30, 30, 30, 30, 24, 20,
	25, 24, 30, 30, 30, 30, 24, 20,
	14, 30, 30, 30, 30, 30, 24, 14,
	14, 24, 34, 24, 24, 24, 24, 14,
	 4, 14, 14, 20, 20, 14, 14,  4,

};
static const int	QueenEndGamePST[64]		= {
	 0,  8, 12, 16, 16, 12, 8,   0,
	24, 32, 36, 40, 40, 36, 32, 24,
	12, 20, 24, 28, 28, 24, 20, 12,
	16, 24, 28, 32, 32, 28, 24, 16,
	16, 24, 28, 32, 32, 28, 24, 16,
	12, 20, 24, 28, 28, 24, 20, 12,    
	 8, 16, 20, 24, 24, 20, 16,  8,
	 0,  8, 12, 16, 16, 12, 8,   0,
};

// Incluye pts por rey enrocado
static const int	KingMidGamePST[64]		= {
	120, 130, 110, 100, 100, 110, 130, 120,
	120, 130, 110, 100, 100, 110, 130, 120,
	140, 150, 130, 110, 110, 130, 150, 140,
	160, 170, 150, 130, 130, 150, 170, 160,
	180, 190, 170, 150, 150, 170, 190, 180,
	200, 210, 190, 170, 170, 190, 210, 200,
	220, 230, 210, 190, 190, 210, 230, 220,
	230, 240, 220, 200, 200, 220, 240, 230,
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
static const int	BishopPairMid[17]		= {50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22, 20, 18,};
static const int	BishopPairEnd[17]		= {60, 58, 56, 54, 52, 50, 48, 46, 44, 42, 40, 38, 36, 34, 32, 30, 28,};

static const int	WithOutBishop			= 20;


// Bonus, por estar presente algunas configuraciones de material
static const int	KnightVSpawnsMid		= 130;
static const int	KnightVSpawnsEnd		= 90;

static const int	RookVSknightMid			= 64;
static const int	RookVSknightEnd			= 40;

static const int	KnightBishopVSrookMid	= 56;
static const int	KnightBishopVSrookEnd	= 26;

static const int	RookVSpawnsMid			= 200;
static const int	RookVSpawnsEnd			= 100;



//==============Estructura de peones. Evalúa algunos de los tipos de peones===================================================

/*
 * Peones doblados en medio juego
 *
 * 1) Penaliza distancia hacia al centro, entre más cerca más valor
 * 2) Penaliza peones en columnas torre, también son aislados y son más difíciles de desdoblar
 */
static const int	PawnDoubled[64]			= {
	 0,  0,  0,  0,  0,  0,  0,  0,
	14, 10, 12, 14, 14, 12, 10, 14,
	14, 10, 12, 14, 14, 12, 10, 14,
	14, 10, 12, 14, 14, 12, 10, 14,
	14, 10, 12, 14, 14, 12, 10, 14,
	14, 10, 12, 14, 14, 12, 10, 14,
	14, 10, 12, 14, 14, 12, 10, 14,
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
static const int	TempoMid				= 14;
static const int	TempoEnd				= 10;


// Penalización por piezas bloqueadas, dificultan la movilidad a otras piezas o así mismas
static const int	BlockedPawnC2			= 16;
static const int	BlockedCentralPawn		= 20;


// Penalización, alfil malo. Se incrementa en el final con idea de intercambiar piezas
static const int	BadBishop[9]			= {0,  8, 24, 40, 56, 72, 88, 104, 120,};

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
#define				IMP4					0x3C3C3C3C0000L
#define				IMP2					0x7E424242427E00L
#define				IMP1					0x7E8181818181817EL


static const int	KnightMob[32]			= {-16,-15,-13,-12,-11,-9,-8,-7,-5,-4,-3,-1,0,1,3,4,5,7,8,9,11,12,13,15,16,18,19,21,22,24,};
static const int	BishopMob[32]			= {-21,-20,-18,-17,-15,-14,-12,-11,-9,-8,-6,-5,-4,-2,-1,1,2,4,5,7,8,9,11,12,14,15,17,18,20,21,23,24,};
//static const int	BishopMob[16]			= {-21,-18,-15,-12,-9,-6,-3,0,3,6,9,12,15,18,21,24,};
static const int	RookMob[32]				= {-14,-13,-11,-10,-9,-7,-6,-5,-3,-2,-1,1,2,3,5,6,7,9,10,11,13,14,16,17,19,20,22,23,25,26,28,29,};
static const int	QueenMob[32]			= {-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,};

// Bonus, movilidad de rey. De momento solo usado en el final
static const int	KingMob					= 16;

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

static const int	KingInOpenFile			= 12;

static const int	CasttleDone				= 40;				// Penalización o Bonus, por hacer o perder el enroque

static const int	Fianchetto				= 12;				// Penalización o Bonus, por tener o perder el Fianchetto


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
static const int	DiffEndGame[17]			= {32,30,28,26,24,22,20,18,16,16,16,16,16,16,16,16,16,};


/*
 * Base de datos(bitboard) que segun donde esta el rey(sin peon) determina si es tablas.
 * Tambien funciona para mas de 3 peones doblados.
 * Puede fallar cuando existen varios peones y pueden avansar, la idea es que la busqueda compense este error
 */
static const U64	KQKP_DB1[2]				= {0xE0E0E0E0F0FFFFFFL, 0xFFFFFFF0E0E0E0E0L,};
static const U64	KQKP_DB2[2]				= {0x70707070FFFFFFFL, 0xFFFFFF0F07070707L,};
static const U64	KQKP_DB3[2]				= {0xE0E0F0F8FFFFFFFFL, 0xFFFFFFFFF8F0E0E0L,};
static const U64	KQKP_DB4[2]				= {0x7070F1FFFFFFFFFL, 0xFFFFFFFF1F0F0707L,};

#endif // EVAL_H


//	500
