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

#ifndef _HEADER_FILE_H_
#define _HEADER_FILE_H_

#include "book.h"


//====================================================================================================================================
// Comprueba si la cadena recibida es true o TRUE
#define		STRING_TO_BOOL(str)			(!(strcmp(str, "true") && strcmp(str, "TRUE")))


// algunos comandos UCI
typedef	enum	{
	CHANGE_HASH			= 1<<0,
	CHANGE_BOOK			= 1<<1,
	ALL_UCI_OPTION		= CHANGE_HASH | CHANGE_BOOK,
}	UCI_TYPE;


//====================================================================================================================================
extern UCI_TYPE		UCIoptions;
extern int			OwnBook;


//====================================================================================================================================
void	UCI();


#endif //_HEADER_FILE_H_
