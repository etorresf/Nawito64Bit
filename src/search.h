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

#ifndef SEARCH_H
#define SEARCH_H

#include <math.h>
#include "time.h"
#include "eval.h"
#include "move_gen.h"
#include "see.h"


//====================================================================================================================================



//====================================================================================================================================
#define				QS_LIMITE				-7					// Para prevenir la qsearch exploxión
#define				MARGEN					20					// Usado en la Aspiration Windows

#define				DEFAULT_WINDOW			20

#define				MIN_POST				8


typedef enum	{
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
typedef enum	{
	PLY				= 4,
	HALF_PLY		= PLY / 2,
	PLY2			= PLY * 2,
	PLY3			= PLY * 3,
	PLY4			= PLY * 4,
	PLY5			= PLY * 5,
	PLY6			= PLY * 6,
	PLY7			= PLY * 7,
	PLY11			= PLY * 11,
	PLY16			= PLY * 16,
}	PLY_TYPE;


//====================================================================================================================================
/*
 * Retorna valor para posicion tablas(no necesariamente 0).
 * Depende del material para evitar aceptar tablas tempranamente
 */
#define				DRAW_VALUE(side)		((GAME_PHASE / 32) * ((side == EngineTurn) ? -1: 1))


extern int			InCheckRoot;
extern int			Cknodes;									// Indica cada que numero de nodos se comprobara el tiempo
extern int			Li;											// Profundidad desde la que se inicia


extern MOVE			PV[MAX_DEPHT * MAX_DEPHT];					// Variante principal,ver libro {How Computers Play Chess} by Levy and Newborn
extern int			PVLenght[MAX_DEPHT];						// Cuantos movimientos tiene la variante principal
#define				PV_INDEX(ply)		(PV + (ply) * MAX_DEPHT)


extern int			Value1[MAX_DEPHT];

extern int			LMRtable[MAX_DEPHT][256];


#define				R								PLY2		// Controla la profundidad del movimiento nulo
#define				DR								PLY4

#define				RAZOR_DEPTH						PLY3
#define				RAZORING_MARGEN(d)				((d) <= PLY ? ((d) * 40) : ((d) * 20 + 80))

#define				REVERSE_FUTILITY_DEPTH			PLY7
#define				REVERSE_FUTILITY_MARGEN(d)		((d) * 25)

#define				FUTILITY_MARGEN(d)				((d) <= PLY ? ((d) * 30) : ((d) * 5 + 100))

#define				DEPTH_NULL(d)					((d) - (PLY + R + (d) / 6.0))


// Usados las extensiones
static const int	PawnEndgameExt[16]		= {
	0,0,
	0,0,
	PLY,		PLY,
	PLY,		PLY,
	PLY3>>1,	PLY3>>1,
	PLY2,		PLY2,
	0,0,
	0,0,
};


extern int			UpdateInfoUCI;
extern int			FailUCI;

// Cortes bajos o altos para enviar informacion al protocolo UCI
typedef enum	{
	LOW	= 1,
	HIGH,
}	FAIL;


extern int			Nodes;										// Contador de todas las visitas a nodos


extern STACK_MOVE	MoveStack[MAX_DEPHT * 256];					// Memoria para los movimientos generados por el generador de movimientos
#define				STACK_INDEX(ply)		(MoveStack + (ply) * 256)


extern MOVE			Killer1[MAX_DEPHT];							// Guarda los movimientos que producen un corte
extern MOVE			Killer2[MAX_DEPHT];							// Guarda los movimientos que producen un corte
extern MOVE			MateKiller[MAX_DEPHT];						// Movimiento que amenaza mate


//====================================================================================================================================
MOVE			EngineThink(int state, int remaining_time, int inc_time, int mps, int max_depth, int movetime, int out_of_book);		// Retorna el mejor movimiento para una profundidad
void			IniSearch();									// Formula de ruduccion logaritmica
int				QuiesenceSearch(int alpha, int beta, int depth);
void			HistoryAge(TURN side, int dec);



#endif // SEARCH_H


//	100	150
