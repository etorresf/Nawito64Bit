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

#ifndef PIECE_H
#define PIECE_H


//====================================================================================================================================
typedef	int			PIECE;


// Representación de las PIEZAS con dígitos, sin color
typedef enum	{
	EMPTY_PIECE		= 0,
	PAWN			= 2,
	KNIGHT			= 4,
	BISHOP			= 6,
	ROOK			= 8,
	QUEEN			= 10,
	KING			= 12,
}	PIECE_WHITOUT_COLOR;


// Representación de las PIEZAS con dígitos, con color
typedef enum	{
	BLACK_PAWN = 2,	WHITE_PAWN,
	BLACK_KNIGHT,	WHITE_KNIGHT,
	BLACK_BISHOP,	WHITE_BISHOP,
	BLACK_ROOK,		WHITE_ROOK,
	BLACK_QUEEN,	WHITE_QUEEN,
	BLACK_KING,		WHITE_KING,
}	PIECE_WHIT_COLOR;


//====================================================================================================================================
// Valor de las PIEZAS en centipeones. Estos valores no se utilizan en la evaluación
#define				PAWN_VALUE				100
#define				KNIGHT_VALUE			300
#define				BISHOP_VALUE			300
#define				ROOK_VALUE				500
#define				QUEEN_VALUE				900
#define				KING_VALUE				1000


// Valor de las PIEZAS según su tipo. Estos valores no se utilizan en la evaluación
static const int	PieceValue[16]			= {
	0, 0,
	PAWN_VALUE,		PAWN_VALUE,
	KNIGHT_VALUE,	KNIGHT_VALUE,
	BISHOP_VALUE,	BISHOP_VALUE,
	ROOK_VALUE,		ROOK_VALUE,
	QUEEN_VALUE,	QUEEN_VALUE,
	KING_VALUE,		KING_VALUE,
	0, 0,
};


// Muestra el tipo de promocion con caracteres
static const char	PromotionChar[16]		= "    nnbbrrqq    ";


//====================================================================================================================================
// Determina el color de una PIEZA
#define				GET_COLOR(piece)		((piece) & 1)


#endif // PIECE_H


//	80

