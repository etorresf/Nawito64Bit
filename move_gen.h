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

#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include "attack.h"


#define				FUTILITY_PRUNNING_SCORE	0x100000
#define				BAD_CAPTURE_SCORE		0x200000

#define				KILLER_SCORE			0x2000000
#define				MATE_KILLER_SCORE		0x4000000

#define				GOOD_CAPTURE_SCORE		0x8000000
#define				PROM_SCORE				0x10000000
#define				GOOD_CAPTURE_PROM_SCORE	0x18000000

#define				HASH_SCORE				0x20000000
#define				FOLLOW_PV_SCORE			0x40000000


STACK_MOVE*			MoveGen(STACK_MOVE* HMove);					// Genera los pseudos movimientos del turno que le toca jugar
STACK_MOVE*			CaptureGen(STACK_MOVE* HMove);				// Genera los movimientos de captura y promociones. Básicamente una copia de MoveGen

void				UnMakeMove(void);							// Lo contrario a hacer un movimiento
U8					MakeMove(MOVE Move);						// Se encarga de realizar un movimiento

void				PushMove(STACK_MOVE** HMove, SQUARE From, SQUARE To, int Type);		// Agrega un movimiento a la lista de movimientos
void				PushCapture(STACK_MOVE** HMove, SQUARE From, SQUARE To, int Type);	// Agrega un movimiento de captura o promoción a dama a la lista de movimientos

int					CountLegals(int MaxNumLegals);				// Retorna el número de movimientos legales(<= 'MaxNumLegals')

STACK_MOVE			CheckOneMove(char* s, int* l);				// Comprueba si un movimiento es legal
long				LoadEPD(char* EPD, STACK_MOVE* MoveList);	// Carga un archivo en formato EPD



#endif // MOVE_GEN_H
