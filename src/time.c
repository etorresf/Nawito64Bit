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

#include "time.h"


struct timezone	tz;
struct timeval	tv;


int		MaxTime;
int		Mps;
int		Base;
int		Inc;
int		RemainingMove;
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


void	CalcRemainingMoves(int Material, int Nmove, int EngineTurn)				{
	double		RMove = 0;


	// Movimientos que quedaran para llegar al control si no es partida con incremento
	RemainingMove = Mps - Nmove / 2;
	if (EngineTurn == 1)
		RemainingMove--;

	if (Mps)
		while (RemainingMove <= 0)
			RemainingMove += Mps;

	else		{
		/*
		 * Control del tiempo basado en el material ver:
		 * "TIME MANAGEMENT PROCEDURE IN COMPUTER CHESS"
		 * por "Vladan Vučković, Rade Šolak"
		 */
		if (Material > 78)
			Material = 78;

		if (Material < 20)
			RMove	+= ((double)Material + 10.0) / 2.0;//16;

		else if (Material <= 60)
			RMove	+= ((3.0 * (double)Material) / 8.0 + 22.0) / 2.0;

		else RMove	+= ((5.0 * (double)Material) / 4.0 - 30.0) / 2.0;


		/*
		 * Control del tiempo basado en el numero de movimientos ver:
		 * "TIME MANAGEMENT PROCEDURE IN COMPUTER CHESS"
		 * por "Vladan Vučković, Rade Šolak"
		 */
		if (Nmove <= 35)
			RMove	+= (175.0 - (double)Nmove) / 5.0;

		else if (Nmove <= 60)
			RMove	+= (204.0 - (double)Nmove) / 6.0;

		else RMove	+= 24.0;

		RemainingMove = (int)(RMove / 2.0);
	}
}


void	CalcTimeForMove(int TimeMS, int Nmove, int EngineTurn, int OutOfBook, int Material)		{
	double	factor, target;


	TimeMS	-= 100;			// Mantiene un margen de seguridad de 100 ms
	if (TimeMS < 10)		// Mínimo
		TimeMS	= 10;

	TimeTotal	= MaxTime = TimeMS;


	CalcRemainingMoves(Material, Nmove, EngineTurn);

	factor	= 2.0 - (MIN(10.0, (double)OutOfBook)) / 10.0;
	target	= (double)TimeTotal / (double)RemainingMove + (double)(8.0 * Inc) / 10.0;


	TimeLimit		= (int)(factor * target);
	NoNewIteration	= (int)(target / 2);
	NoNewMov		= (int)(3 * target) / 4;
}


// 64
