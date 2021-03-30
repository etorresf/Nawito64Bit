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


#ifndef BOOK_H
#define BOOK_H

#include "test.h"



#define				LAST_MOVE				0x8000

extern int			OutOfBook;
extern int			MaxNumMove;



// Movimiento de libro
typedef struct		BOOK_MOVE_STRUCT			{
	U16				Move,
					Weight;										// Numero de partidas ganadas
}	BOOK_MOVE;


// Movimientos del libro para una posición
typedef struct		BOOK_MOVE_LIST_STRUCT		{
	U64				Key;										// Número asociado a la posición. Basado en Polyglot hash
	U8				Moves;

	BOOK_MOVE*		MoveList;
}	BOOK_POS;


// Libro usado por el motor
typedef struct	BOOK_STRUCT		{
	I64				NumKey;
	U32				State;
	char			Name[64];

	BOOK_POS*		Position;
}	BOOK;


extern BOOK			MyBook;



int					LoadBook(BOOK* book, char* name);			// Carga un libro. Devuelve 0 si no ocurre error
void				CloseBook(BOOK* book);						// Cierra el libro devolviendo la memoria usada por el mismo

MOVE				CheckBook(BOOK* book, U64 key, int num_move);// Consulta al libro de aperturas

MOVE				ScanBookMove(char* c);
void				PrintBookMove(char* buffer, MOVE move);		// Convierte a 'string' un movimiento del libro


#endif // BOOK_H
