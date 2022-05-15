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

#ifndef MOVE_H
#define MOVE_H

#include "hash.h"
#include "piece.h"


//====================================================================================================================================
// En realidad solo es necesario 16 bit para codificar los movimientos
typedef	int			MOVE;


// Tipos de movimientos. Los movimientos de promoción son los mismos tipos de piezas
typedef enum	{
	NORMAL,
	CASTTLE,
	PAWN_PUSH_2_SQUARE,
	EN_PASSANT_CAPTURE,
}	MOVE_TYPE;


#define				EMPTY_MOVE				0
#define				ILLEGAL_MOVE			0x10000
#define				UNKNOWN_MOVE			0x20000


// Maximo numero de movimientos para cada bando en la partida. Parece execisivo pero he detectado algunas partidas con mas de 500 movimientos
#define				HIST_STACK				2048


// Guarda los movimientos tras su generacion
typedef struct	{
	MOVE			ID;
	int				Score;
}	STACK_MOVE;


// Guarda los movimientos con sus datos de la partida
typedef struct	{
	MOVE			Move;
	PIECE			Capture;
	SQUARE			EnPassant;
	CASTTLE_RIGHT	Casttle;
	int				Rule50;
	HASH_KEY		Hash;
}	HIST_MOVE;


//====================================================================================================================================
// Codifica en 16 bit un movimiento
#define				ENCODE_MOVE(From, To, Type)	(((From) << 10) | ((To) << 4) | (Type))
#define				GET_MOVE_TYPE(EncodeMove)	((EncodeMove) & 15)
#define				GET_FROM(EncodeMove)		((EncodeMove)	>> 10)
#define				GET_TO(EncodeMove)			(((EncodeMove)	>> 4) & 63)
#define				GET_ID(EncodeMove)			((EncodeMove)	>> 4)
#define				PROMOTION(Type)				((Type) & 12)	// Determina si el tipo de movimento o un movimento codificado es de promocion

void				PrintMove(MOVE move);						// Imprime un movimiento


#endif // MOVE_H


//	50
