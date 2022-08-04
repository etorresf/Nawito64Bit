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

#ifndef MOVE_GEN_H
#define MOVE_GEN_H

#include "board.h"


//====================================================================================================================================
void	MoveGen(TURN side, STACK_MOVE* move_list, int qmoves);	// Genera los pseudos movimientos del turno que le toca jugar
void	UnMakeMove();											// Lo contrario a hacer un movimiento
int		MakeMove(MOVE move);									// Se encarga de realizar un movimiento
MOVE	CheckMove(char* s, int make_move);


#endif // MOVE_GEN_H


//	30
