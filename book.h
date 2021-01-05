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

#ifndef BOOK_H
#define BOOK_H

#include "test.h"
#include <stdlib.h>
#include <stdio.h>


typedef	enum		BOOK_CONFIG_TYPE	{USE=1<<12, LEARN=1<<13, LOCK=1<<14, } BOOK_CONFIG;


extern int			OutOfBook;


// Movimiento de libro
typedef struct		BOOK_MOVE_STRUCT{
	U16				Move,
					Weight;										// Numero de partidas ganadas
}	BOOK_MOVE;


// Movimientos del libro para una posición
typedef struct		BOOK_MOVE_LIST_STRUCT			{
	U64				Key;										// Número asociado a la posición. Basado en Polyglot hash
	U8				Moves;

	BOOK_MOVE*		MoveList;
}	BOOK_POS;


// Libro usado por el motor
typedef struct	BOOK_STRUCT		{
	U32				NumKey,
					Size;
	U32				State;
	char			Name[64];

	BOOK_POS*		Position;
}	BOOK;


extern BOOK			MyBook;


int					LoadBook(BOOK* Book, char* Name);			// Carga un libro. Devuelve 0 si no ocurre error
void				CloseBook(BOOK* Book);						// Cierra el libro devolviendo la memoria usada por el mismo

void				PrintBookConfig(CONFIG Config);

MOVE				CheckBook(BOOK* Book, U64 Key);				// Consulta al libro de aperturas


#endif // BOOK_H
