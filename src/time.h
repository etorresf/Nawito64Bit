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

#ifndef TIME_H
#define TIME_H

#include <sys/time.h>
#include "util.h"


#define				INF_TIME				(1<<25)
#define				DEFAULT_TIME			8000


extern struct		timezone				tz;
extern struct		timeval					tv;


extern int			MaxTime;									// Máximo de tiempo disponible para un movimiento en milisegundos
extern int			Mps;										// Número de jugadas a realizar para cumplir el control de tiempo
extern int			Base;										// Tiempo base en minutos para cumplir el control de tiempo
extern int			Inc;										// Tiempo de incremento para el reloj en segundos después de jugar
extern int			RemainingMove;								// Número de movimientos que quedan para el control de tiempo
extern int			TimeLeft;									// Momento en el que empezamos un movimiento
extern int			TimeLimit;									// No podremos exceder este límite de tiempo nunca durante una búsqueda
extern int			NoNewMov;									// No empezar a buscar un nuevo movimiento si ya hemos sobrepasado un tiempo
extern int			NoNewIteration;								// No empezar un nuevo nivel de profundidad si no disponemos de suficiente tiempo
extern int			TimeTotal;									// Tiempo total consumido


int					ClockTime(void);							// Retorna en milisegundos el tiempo que ha pasado desde medianoche del 1ro de enero de 1970
void				CalcTimeForMove(int TimeMS, int Nmove, int EngineTurn, int OutOfBook, int Material);


#endif // TIME_H
