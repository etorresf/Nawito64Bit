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

#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <inttypes.h>


//====================================================================================================================================
typedef	uint64_t	U64;
typedef	uint32_t	U32;
typedef	uint16_t	U16;
typedef	uint8_t		U8;

typedef	int16_t		I16;
typedef	int64_t		I64;


typedef	U32			STATE;


//====================================================================================================================================
#define				MAX_DEPHT				64					// Máxima profundidad con la que pensar

// Nombre y Arquitectura el motor
#define				ENGINE_NAME				"Nawito-22.07"
#define				ENGINE_ARCH				"amd64"
#define				ENGINE_VERSION			"22.07"				// Nombre del motor. anho. mes. estado de dasarrollo
#define				ENGINE_PROTOCOL			"uci"				// Protocolo de comunicación que utiliza el motor

#define				AUTOR_NAME				"Ernesto Torres"	// Nombre del autor del motor

#define				ENGINE_CONFIG_FILE		"Nawito.ini"


#define				MATE					10000
#define				MATE_SCORE				9900				// Identifica evaluación de mate

#define				INITIAL_MATERIAL		3100				// (QUEEN_VALUE + 2 * (ROOK_VALUE + BISHOP_VALUE + KNIGHT_VALUE))


//====================================================================================================================================
#define				CONSUME_ESPACE(c)		for (; *c == ' '; c++);		// Consume espacios de una cadena de caracteres

// Calcula el maximo y minimo entre 2 numeros. Usados en la deteccion de distancias entre casillas
#define				MAX(x, y)				((x) > (y) ? (x) : (y))
#define				MIN(x, y)				((x) < (y) ? (x) : (y))


#endif // UTIL_H


//	50	70
