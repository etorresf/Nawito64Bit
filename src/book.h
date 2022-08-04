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

#ifndef BOOK_H
#define BOOK_H

#include "test.h"


//====================================================================================================================================
typedef struct	{
	U64			Key;
	U16			Move,
				Weight;
	U32			Learn;
}	BOOK_ENTRY;


//====================================================================================================================================
extern char		BookPath[2048];

extern int		OutOfBook;
extern int		OpenBook;


//====================================================================================================================================
void			BookIni();
int				BookOpen();
int				BookClose();
MOVE			CheckBook(U64 key);								// Consulta al libro de aperturas

int				IntFromFile(U64* r, FILE* file, int l);
void			IntToFile(FILE* file, U64 r, int l);

void			LoadBookConfig(char* file);


#endif // BOOK_H


//	50
