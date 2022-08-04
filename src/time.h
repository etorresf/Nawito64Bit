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

#ifndef TIME_H
#define TIME_H

#include <sys/time.h>


//====================================================================================================================================
#define			SEC						(1000)					// 1 segundo
#define			INF_TIME				0x3FFFFFFF				// Valor bastante alto(como si fuera infinito)
#define			DEFAULT_TIME			(8 * SEC)
#define			DEFAULT_MOVE_TO_GO		0
#define			DEFAULT_INC_TIME		0
#define			DEFAULT_MOVE_TIME		0


//====================================================================================================================================
extern struct	timezone				tz;
extern struct	timeval					tv;

extern int		StartTime;										// Tiempo en que el motor comienza a pensar
extern int		TimeLimit;										// No podremos exceder este límite de tiempo nunca durante una búsqueda
extern int		MoveTimeLimit;									// No empezar a buscar un nuevo movimiento si ya hemos sobrepasado un tiempo
extern int		IterationTimeLimit;								// No empezar un nuevo nivel de profundidad si no disponemos de suficiente tiempo


//====================================================================================================================================
int				ClockTime();									// Retorna en milisegundos el tiempo que ha pasado desde medianoche del 1ro de enero de 1970
void			StartTimer(int remaining_time, int inc_time, int mps, int out_of_book, int mat_sum, int move_time);

#define			ELAPSED_TIME()			(ClockTime() - StartTime)				// Tiempo transcurrido desde que el motor comienza a pensar
#define			TIME_EXC()				(ELAPSED_TIME() >= TimeLimit)			// Tiempo para pensar excedido
#define			MOVE_TIME_EXC()			(ELAPSED_TIME() >= MoveTimeLimit)		// limite de tiempo para un nuevo movimiento excedido
#define			ITERATION_TIME_EXC()	(ELAPSED_TIME() >= IterationTimeLimit)	// limite de tiempo para una nueva iteracion excedida


#endif // TIME_H


//	50
