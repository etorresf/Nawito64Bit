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


#include "move.h"



// Imprime un movimiento. Útil para solucionar errores y mostrar el pensamiento del motor
int		SprintMove(MOVE move, char* move_char)	{
	char*		n	= move_char;

	move_char	+= sprintf(move_char, "%s%s", SquareChar[GET_FROM(move)], SquareChar[GET_TO(move)]);

	if (PROMOTION(move))		{
		move_char += sprintf(move_char, "%c", PromotionChar[GET_MOVE_TYPE(move)]);
		return (int)(move_char - n) + 1;
	}

	return (int)(move_char - n);
}


// Imprime un movimiento. Útil para solucionar errores y mostrar el pensamiento del motor. Similar al anterior, imprime directamente el movimiento
void	PrintMove(MOVE move)	{
	printf("%s%s", SquareChar[GET_FROM(move)], SquareChar[GET_TO(move)]);

	if (PROMOTION(move))
		printf("%c", PromotionChar[GET_MOVE_TYPE(move)]);
}


// 32
