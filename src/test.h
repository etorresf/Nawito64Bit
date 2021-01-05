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

#ifndef TEST_H
#define TEST_H

#include <inttypes.h>
#include "search.h"


#define				PERFT_POS				"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ; D1 48; D2 2039; D3 97862; D4 4085603; D5 193690690; D6 8031647685;"	// Posición para el perft por defecto
#define				BENCH_POS				"r4nk1/4brp1/q2p1pQP/3P4/2p1NP2/P7/1B6/1KR3R1 w - - 1 34 ; M11;"	//[Event "CCRL 40/4"][Date "2019.01.24"][Round "464.4.713"][White "Nawito 1812"][Black "Amyan 1.72"][Result "1-0"]


void				Perft(int Depth);							// Comprueba el generador de movimientos, contando el número de nodos totales para una profundidad
void				Bench(void);								// Resuelve un mate, sirve para comprobar la velocidad relativa del ordenador respecto a uno determinado


#endif // TEST_H
