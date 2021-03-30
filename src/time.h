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


#ifndef TIME_H
#define TIME_H

#include <sys/time.h>
#include "util.h"



#define				INF_TIME				(1<<25)				// Valor bastante alto(como si fuera infinito)
#define				DEFAULT_TIME			8000				// Equivalente a 8 segundos


extern struct		timezone				tz;
extern struct		timeval					tv;


extern int			MaxTime;									// Máximo de tiempo disponible para un movimiento en milisegundos
extern int			Mps;										// Número de jugadas a realizar para cumplir el control de tiempo
extern int			Base;										// Tiempo base en minutos para cumplir el control de tiempo
extern int			Inc;										// Tiempo de incremento para el reloj en segundos después de jugar
extern double		RemainingMove;								// Número de movimientos que quedan para el control de tiempo
extern int			TimeLeft;									// Momento en el que empezamos un movimiento
extern int			TimeLimit;									// No podremos exceder este límite de tiempo nunca durante una búsqueda
extern int			NoNewMov;									// No empezar a buscar un nuevo movimiento si ya hemos sobrepasado un tiempo
extern int			NoNewIteration;								// No empezar un nuevo nivel de profundidad si no disponemos de suficiente tiempo
extern int			TimeTotal;									// Tiempo total consumido


/*
 * Valores basados(entre 16 y 32) para el Control del tiempo segun material de:
 * "TIME MANAGEMENT PROCEDURE IN COMPUTER CHESS"
 * por "Vladan Vučković, Rade Šolak"
 */
static const		double RMoveDB[80] = {16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16.06,16.25,16.44,16.62,16.81,17,17.19,17.38,17.56,17.75,17.94,18.12,18.31,18.5,18.69,18.88,19.06,19.25,19.44,19.62,19.81,20,20.19,20.38,20.56,20.75,20.94,21.12,21.31,21.50,21.69,21.88,22.06,22.25,23.12,23.75,24.38,25,25.62,26.25,26.88,27.50,28.12,28.75,29.38,30,30.62,31.25,31.88,32,32,32,32,};



int					ClockTime(void);							// Retorna en milisegundos el tiempo que ha pasado desde medianoche del 1ro de enero de 1970
void				CalcTimeForMove(int time_ms, int num_move, int side, int out_of_book, int material);


#endif // TIME_H
