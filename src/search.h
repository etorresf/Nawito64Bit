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


#ifndef SEARCH_H
#define SEARCH_H


#include <math.h>
#include "time.h"
#include "eval.h"
#include "config.h"
//#include "book.h"



#define				QS_LIMITE				-7					// Para prevenir la qsearch exploxión
#define				MARGEN					20					// Usado en la Aspiration Windows



typedef	enum		MOVE_SORT_SCORE_TYPE	{
	FUTILITY_PRUNNING_SCORE		= 1<<20,
	BAD_CAPTURE_SCORE			= 1<<21,
	KILLER_SCORE				= 1<<25,
	MATE_KILLER_SCORE			= 1<<26,
	GOOD_CAPTURE_SCORE			= 1<<27,
	PROM_SCORE					= 1<<28,
	HASH_SCORE					= 1<<29,
	FOLLOW_PV_SCORE				= 1<<30,
}	MOVE_SORT_SCORE;			// Estados de la búsqueda


// Usado para extensiones fracciónales
typedef	enum		PLY_TYPES				{QUARTER_PLY=1, HALF_PLY=2, PLY=4, PLY2=8, PLY3=12, PLY4=16, PLY5=20, PLY6=24, PLY7=28, PLY11=44, PLY16=64}	PLY_TYPE;


typedef	enum		SEARCH_STATE_TYPE		{
	ABORT			= 1<<0,
	STOP			= 1<<1,
	FOLLOW_PV		= 1<<2,
} SEARCH_STATE;			// Estados de la búsqueda


typedef	enum		ENGINE_STATE_TYPE		{
	SEARCHING		= 1<<0,
	PONDERING		= 1<<1,
	ANALYZING		= 1<<2,
}	ENGINE_STATE;	// Estados del motor



extern U8			WindowFlag;									// BUG, PARCHE. A veces el motor se queda en ciclo infinito con las ventanas
static const int	ContemFactor[4]			= {500,500,536,536};// Prefiere tablas cuando hay menos material y se lleva negras


extern U8			InCheckRoot;
extern int			LegalMovesRoot;
extern int			Cknodes;									// Indica cada que numero de nodos se comprobara el tiempo
extern U8			CurrentAge;
extern int			DepthNull;									// Profundidad para el movimiento nulo
extern STATE		SearchState;								// Estado de la búsqueda


extern MOVE			PV[MAX_DEPHT + 1][MAX_DEPHT + 1];			// Variante principal,ver libro "How Computers Play Chess" by Levy and Newborn
extern int			PVLenght[MAX_DEPHT + 1];					// Cuantos movimientos tiene la variante principal


extern char			Command[256];								// Para leer la información del GUI y saber que comando llega
extern char			Line[256];									// Para leer la información del GUI y saber que comando llega


extern int			Value1[MAX_DEPHT_1];


extern double		LMRtable[64][256];


// Usados en el movimiento nulo
static const int	RazoringMargen[13]		= {0,40,80,120,160,180,200,220,240,260,280,300,320};
static const int	ReverseFutilityMargen[29] = {0,25,50,75,100,125,150,175,200,225,250,275,300,325,350,375,400,425,450,475,500,525,550,575,600,625,650,675,700};
static const int	Margen[4]				= {0, 120, 140, 160};
static const int	DepthNullTable[128]		= {					// Controla la profundidad del movimiento nulo
	0,0,0,0,0,0,0,2,4,6,8,10,12,10,12,14,16,18,20,22,24,26,28,26,28,30,32,34,36,38,40,42,44,
	42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,
	100,102,104,106,108,110,112,114,116,118,120,122,124,126,128,130,132,134,136,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,166,168,170,172,174,176,178,180,182,184,186,
	188,190,192,194,196,198,200,202,204,206,208,210,212,214,216,218,220,222,224,226,228,230,
};


// Usados las extensiones
static const int	PawnEndgameExt[16]		= {
	0,0,
	0,0,
	PLY,	PLY,
	PLY,	PLY,
	PLY3>>1,PLY3>>1,
	PLY2,	PLY2,
	0,0,
	0,0,
};


MOVE				EngineThink(void);							// Retorna el mejor movimiento para una profundidad

void				IniLMRtable(void);							// Formula de ruduccion logaritmica

void				PrintResult(void);							// Si es final de partida indica el resultado


#endif // SEARCH_H
