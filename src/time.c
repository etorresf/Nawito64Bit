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

#include "time.h"
#include "util.h"


//====================================================================================================================================
struct timezone	tz;
struct timeval	tv;

int				Mps;
int				Inc;
int				TimeLeft;
int				TimeLimit;
int				NoNewMov;
int				NoNewIteration;
double			RemainingMove;
double			TimeTotal;


//====================================================================================================================================
// Retorna en milisegundos el tiempo que ha pasado desde la medianoche del 1ro de enero de 1970
int		ClockTime()				{
	gettimeofday (&tv, &tz);
	return (int)(tv.tv_sec * 1000 + (tv.tv_usec / 1000));
}


// Determina que tiempo debe pensar el motor para una posicion
void	CalcTimeForMove(int time_ms, double out_of_book, int material)			{
	double	factor, target,
			x		= MIN(78, material);						// Si material mayor al inicial, entonces asignarle el material inicial


	// Basado en el control del tiempo basado en el material de:
	// {TIME MANAGEMENT PROCEDURE IN COMPUTER CHESS} por "Vladan Vučković, Rade Šolak"
	// 
	// Se ha creado formula usando algoritmo de evolucion diferencial para emularla
	// se agrega un margen con idea de no tener problemas en el ultimo movimientos antes del control, en caso de Mps
	RemainingMove	= Mps ? ((double)Mps + 0.60) : (0.00012 * (x * x *x) - 0.01279 * (x * x) + 0.62836 * (x) + 5.86123);


	// Adjustes del control del tiempo inspirado en formula de Robert Hyatt, ver:
	// {Time Management} en <www.chessprogramming.org/Home/Search/Time Management>
	// En los 10 primeros movimientos fuera del libro pensar mas
	TimeTotal		= MAX(10, time_ms - 120);					// Mantiene margen de seguridad de 120 y un minimo de 10 ms
	factor			= 2.00 - MIN(10.00, out_of_book) * 0.10;
	target			= factor * (TimeTotal / RemainingMove + (double)(Inc) * 0.80);


	TimeLimit		= (int)(target);
	NoNewIteration	= (int)(target * 0.50);
	NoNewMov		= (int)(target * 0.75);
}


//	70
