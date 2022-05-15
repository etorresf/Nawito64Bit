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

#ifndef COLOR_H
#define COLOR_H


//====================================================================================================================================
typedef	int			TURN;


// Colores y turnos para las piezas, casillas y bandos
typedef	enum	{
	BLACK,
	WHITE,
	EMPTY_TURN,
}	TURN_TYPE;


//====================================================================================================================================
// Intercambia de turno o obtiene el turno del oponente
#define				FLIP_TURN(turn)			((turn) ^ 1)

// Recorre todos los turnos
#define				TURN_LOOP(turn)			for (turn = BLACK; turn <= WHITE; turn++)


#endif // COLOR_H


//	40
