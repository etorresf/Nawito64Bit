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


#ifndef CASTTLE_H
#define CASTTLE_H



typedef	unsigned char		CASTTLE_RIGHT;


// Enroques largos y cortos para el blanco y negro
typedef	enum				CASTTLE_RIGHT_TYPES		{
	WK		= 1<<0,
	WQ		= 1<<1,
	BK		= 1<<2,
	BQ		= 1<<3,
}	CASTTLE_RIGHT_TYPE;


static const CASTTLE_RIGHT		KingSideCasttle[2]	= {BK, WK};
static const CASTTLE_RIGHT		QueenSideCasttle[2]	= {BQ, WQ};



#endif // CASTTLE_H
