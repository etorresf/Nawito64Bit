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

#ifndef SQUARE_H
#define SQUARE_H

#include <stdlib.h>


//====================================================================================================================================
typedef	int			SQUARE;


// Casillas del tablero
typedef enum	{
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
}	SQUARE_TYPE;

// Filas y Columnas del tablero
typedef	enum		{RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8}	RANK_TYPE;
typedef	enum		{FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H}	FILE_TYPE;


//====================================================================================================================================
static const int	Front[2]				= {-8, 8};			// Variacion a una casilla, segun el bando, para obtener la casilla de arriba

extern int			Dist[64][64];								// Distancias manhattan entre casillas
extern int			BisopDist[64][64];							// Distancia entre casillas segun sus diagonales
extern int			DistLineal[64][64];							// Distancias lineales entre casillas
extern int			DistCenter[64];								// Distancias manhattan al centro del tablero
extern int			DistCenterFile[64];							// Distancias manhattan a las columnas centrales
extern int			DistCasttle[64];							// Distancias manhattan a la ultimas filas

#define				LAST_RANK_DIST(sq)				(RANK(sq))				// Distancia a la ultima fila del tablero, punto de vista NEGRO
#define				MANHATTAN_DIST(sq1, sq2)		(Dist[sq1][sq2])		// Distancia manhattan entre casillas
#define				MANHATTAN_DIST_CASTTLE(sq)		(DistCasttle[sq])		// Distancia manhattan al enroque
#define				MANHATTAN_DIST_CENTER(sq)		(DistCenter[sq])		// Distancia manhattan al centro del tablero
#define				MANHATTAN_DIST_CENTER_FILE(sq)	(DistCenterFile[sq])	// Distancia manhattan a las columnas centrales
#define				CHEBYSHEV_DIST(sq1, sq2)		(DistLineal[sq1][sq2])	// Distancia entre casillas
#define				MANHATTAN_DIST_BISHOP(sq1, sq2)	(BisopDist[sq1][sq2] / 2)	// Distancia entre casillas segun sus diagonales


//====================================================================================================================================
void				DistanceIni();								// Calcula distancias entre casillas


// Obtencion de filas y columnas del tablero
#define				RANK(sq)				((sq) >> 3)
#define				FILE(sq)				((sq) & 7)


// Invierte una casilla o obtiene una casilla segun el punto de vista
#define				FLIP_SQ(sq)				((sq) ^ 56)
#define				REL_SQ(sq, side)		((side) ? FLIP_SQ(sq) : (sq))

#define				SQ_LOOP(sq)				for (sq = A1; sq <= H8; sq++)		// Recorre las casillas


#endif // SQUARE_H


// 50	80
