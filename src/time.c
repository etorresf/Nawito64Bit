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

int				StartTime;
int				TimeLimit;
int				MoveTimeLimit;
int				IterationTimeLimit;


//====================================================================================================================================
// Retorna en milisegundos el tiempo que ha pasado desde la medianoche del 1ro de enero de 1970
int		ClockTime()				{
	gettimeofday (&tv, &tz);
	return (int)(tv.tv_sec * 1000 + (tv.tv_usec / 1000));
}


/*
 * Determina que tiempo debe pensar el motor para una posicion.
 * El algoritmo principal es descrito por (Vladan Vučković, Rade Šolak")
 * en {TIME MANAGEMENT PROCEDURE IN COMPUTER CHESS}, en su variante
 * del calculo del tiempo segun el material presente en el tablero.
 * 
 * Se ha aproximado a una formula usando un algoritmo de evolucion diferencial.
 * Ademas se han realizado varios ajustes.
 */
int		CalcTimeForMove(double remaining_time, double inc_time, double mps, double out_of_book, double mat_sum)	{
	if (remaining_time == INF_TIME)
		return INF_TIME;

	// Ajuste del material, no debe sobrepasar el material inicial y se divide por 100 para mejor compatibilidad con el algoritmo.
	double	x	= MIN(78, mat_sum / 100);

	// Ajuste de MPS, en esta modalidad se agrega un margen para no tener problemas en el ultimo movimientos antes del control.
	double remaining_moves	= mps ? (mps + 0.60) : (0.00012 * (x * x *x) - 0.01279 * (x * x) + 0.62836 * (x) + 5.86123);

	// Mantiene margen de seguridad de 120 y un minimo de 10 ms
	remaining_time	= MAX(10, remaining_time - 120);

	/*
	 * Ajustes del control del tiempo inspirado en formula de (Robert Hyatt),
	 * ver: {Time Management} en <www.chessprogramming.org/Home/Search/Time Management>
	 */
	double factor	= 2.00 - MIN(10.00, out_of_book) * 0.10;

	return (factor * (remaining_time / remaining_moves + inc_time * 0.80));
}


// Inicializa datos necesarios para llevar el control del tiempo
void	StartTimer(int remaining_time, int inc_time, int mps, int out_of_book, int mat_sum, int move_time)		{
	StartTime			= ClockTime();
	int time_for_move	= CalcTimeForMove(remaining_time, inc_time, mps, out_of_book, mat_sum);

	if (move_time)
		TimeLimit = IterationTimeLimit = MoveTimeLimit = move_time;

	else	{
		TimeLimit			= time_for_move;
		IterationTimeLimit	= TimeLimit / 2;
		MoveTimeLimit		= (TimeLimit + IterationTimeLimit) / 2;
	}
}

//	70
