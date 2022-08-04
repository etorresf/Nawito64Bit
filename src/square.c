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

#include "square.h"
#include "util.h"


//====================================================================================================================================
// Se almacenan las distancias entre casillas
int		Dist[64][64];
int		BisopDist[64][64];
int		DistLineal[64][64];
int		DistCenter[64];
int		DistCenterFile[64];
int		DistCasttle[64];


//====================================================================================================================================
// Calcula la distancias entre las casillas
void	DistanceIni()			{
	SQUARE	sq1, sq2;
	int		diag_NW[64]		= {0,1,2,3,4,5,6,7,1,2,3,4,5,6,7,8,2,3,4,5,6,7,8,9,3,4,5,6,7,8,9,10,4,5,6,7,8,9,10,11,5,6,7,8,9,10,11,12,6,7,8,9,10,11,12,13,7,8,9,10,11,12,13,14,},
			diag_NE[64]		= {7,6,5,4,3,2,1,0,8,7,6,5,4,3,2,1,9,8,7,6,5,4,3,2,10,9,8,7,6,5,4,3,11,10,9,8,7,6,5,4,12,11,10,9,8,7,6,5,13,12,11,10,9,8,7,6,14,13,12,11,10,9,8,7,};

	// Inicializa distancia: manhattan, chevisev entre casillas
	SQ_LOOP(sq1)
		SQ_LOOP(sq2){
			Dist[sq1][sq2]			= abs(FILE(sq1) - FILE(sq2)) + abs(RANK(sq1) - RANK(sq2));
			BisopDist[sq1][sq2]		= abs(diag_NE[sq1] - diag_NE[sq2]) + abs(diag_NW[sq1] - diag_NW[sq2]);
			DistLineal[sq1][sq2]	= MAX(abs(FILE(sq1) - FILE(sq2)), abs(RANK(sq1) - RANK(sq2)));
		}

	// Inicializa distancia: manhattan al centro, manhattan a columnas centrales, manhattan a enroque
	SQ_LOOP(sq1){
		DistCenter[sq1]		= MIN(MIN(MANHATTAN_DIST(sq1, E4), MANHATTAN_DIST(sq1, E5)), MIN(MANHATTAN_DIST(sq1, D4), MANHATTAN_DIST(sq1, D5)));
		DistCenterFile[sq1]	= MAX(FILE_D - FILE(sq1), FILE(sq1) - FILE_E);
		DistCasttle[sq1]	= MIN(MANHATTAN_DIST(sq1, B8), MANHATTAN_DIST(sq1, G8));
	}
}


//	50
