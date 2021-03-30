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


#include "time.h"



struct timezone	tz;
struct timeval	tv;


int		MaxTime;
int		Mps;
int		Base;
int		Inc;
double	RemainingMove;
int		TimeLeft;
int		TimeLimit;
int		NoNewMov;
int		NoNewIteration;
int		TimeTotal;



// Retorna en milisegundos el tiempo que ha pasado desde la medianoche del 1ro de enero de 1970
int		ClockTime(void)			{
	gettimeofday (&tv, &tz);
	return (int)(tv.tv_sec * 1000 + (tv.tv_usec / 1000));
}


void	CalcTimeForMove(int time_ms, int num_move, int side, int out_of_book, int material)		{
	double factor, target;


	time_ms		= MAX(10, time_ms - 100);						// Mantiene margen de seguridad de 100 y un minimo de 10 ms
	TimeTotal	= MaxTime = time_ms;


	// Movimientos que quedaran para llegar al control si no es partida con incremento
	RemainingMove = Mps - num_move / 2;
	if (side == 1)
		RemainingMove -= 1;

	if (Mps)
		while (RemainingMove <= 0)
			RemainingMove += Mps;

	else RemainingMove	= RMoveDB[MIN(79, material)];			// Control del tiempo basado en el material


	factor	= 2.0 - (MIN(10.0, (double)out_of_book)) / 10.0;
	target	= (double)TimeTotal / (double)RemainingMove + (double)(8.0 * Inc) / 10.0;


	TimeLimit		= (int)(factor * target);
	NoNewIteration	= (int)(target / 2);
	NoNewMov		= (int)(3 * target) / 4;
}


// 64
