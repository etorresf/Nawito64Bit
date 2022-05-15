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
int		Distance[64][64];
int		DistanceLineal[64][64];
int		DistanceCenter[64];
int		DistanceCenterFile[64];
int		DistanceCasttle[64];


//====================================================================================================================================
// Calcula la distancias entre las casillas
void	DistanceIni()			{
	SQUARE		sq1, sq2;


	// Inicializa distancia: manhattan, chevisev entre casillas
	SQ_LOOP(sq1)
		SQ_LOOP(sq2){
			Distance[sq1][sq2]		= abs(FILE(sq1) - FILE(sq2)) + abs(RANK(sq1) - RANK(sq2));
			DistanceLineal[sq1][sq2]= MAX(abs(FILE(sq1) - FILE(sq2)), abs(RANK(sq1) - RANK(sq2)));
		}

	// Inicializa distancia: manhattan al centro, manhattan a columnas centrales, manhattan a enroque
	SQ_LOOP(sq1){
		DistanceCenter[sq1]			= MIN(MIN(Distance[sq1][E4], Distance[sq1][E5]), MIN(Distance[sq1][D4], Distance[sq1][D5]));
		DistanceCenterFile[sq1]		= MAX(FILE_D - FILE(sq1), FILE(sq1) - FILE_E);
		DistanceCasttle[sq1]		= MIN(Distance[sq1][B8], Distance[sq1][G8]);
	}
}


//	50
