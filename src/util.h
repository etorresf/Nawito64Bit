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


#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <inttypes.h>



// Algunos tipos necesarios en el programa
typedef	uint64_t	U64;
typedef	uint32_t	U32;
typedef	uint16_t	U16;
typedef	uint8_t		U8;

typedef	int16_t		I16;
typedef	int64_t		I64;


typedef	U32			STATE;


#define				MAX_DEPHT				63					// Máxima profundidad con la que pensar
#define				MAX_DEPHT_1				64					// Máxima profundidad con la que pensar + 1


#define				ENGINE_NAME				"Nawito2103"		// Nombre del motor. anho. mes. estado de dasarrollo
#define				ENGINE_VERSION			"2103"				// Nombre del motor. anho. mes. estado de dasarrollo

#define				ENGINE_DATE				"10/03/2021"		// Fecha en que se creo el motor
#define				ENGINE_PROTOCOL			"xboard"			// Protocolo de comunicación que utiliza el motor
#define				ENGINE_ARCH				"amd64"				// Arquitectura el motor

#define				AUTOR_NAME				"Ernesto Torres Feliciano"	// Nombre del autor del motor


#define				MATE					10000
#define				MATE_SCORE				9900				// Identifica evaluación de mate

#define				END_GAME_MATERIAL		3000				// (4 * ROOK_VALUE + BISHOP_VALUE + KNIGHT_VALUE + 4 * PAWN_VALUE)
#define				MID_GAME_MATERIAL		3900				// (QUEEN_VALUE + 2 * (ROOK_VALUE + BISHOP_VALUE + KNIGHT_VALUE) + 8 * PAWN_VALUE)
#define				INITIAL_MATERIAL		3100				// (QUEEN_VALUE + 2 * (ROOK_VALUE + BISHOP_VALUE + KNIGHT_VALUE))


// Calcula el maximo y minimo entre 2 numeros. Usados en la deteccion de distancias entre casillas
#define				MAX(x, y)				((x) > (y) ? (x) : (y))
#define				MIN(x, y)				((x) < (y) ? (x) : (y))


#endif // UTIL_H
