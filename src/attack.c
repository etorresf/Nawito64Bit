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


#include "attack.h"



PIECE	PieceBySide[2][8]			= {
	{BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING, 0, 0,},
	{WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING, 0, 0,},
};


/*
 * Usado para evitar una explosion de la QuiesceSearch. no tiene en cuenta la captura al paso.
 * Ver 'SEE - The Swap Algorithm' de 'www.chessprogramming.org' en 'Home/Board Representation/Bitboards/SEE - The Swap Algorithm'
 */
int		SEE(SQUARE from, SQUARE to, U8 fast)	{
	int			gain[32],
				d				= 0;
	PIECE		*piece[2]		= {PieceBySide[BLACK], PieceBySide[WHITE],},
				attacker_piece	= Board[from];
	BITBOARD	from_bb			= SquareBB[from],
				all_bb, attackers_bb, b_xray, r_xray, bb;
	TURN		side			= GET_COLOR(attacker_piece);


	if (fast && PieceValue[Board[to]] >= PieceValue[attacker_piece])
		return 0;												// Salida rápida

	if (!Attacked(to, FLIP_TURN(side)))
		return 0;												// Salida rápida


	// Inicializando ataques a la casilla y posibles ataques de piezas escondidas
	all_bb		= ALL_PIECES_BB ^ SquareBB[to];
	attackers_bb= GetAttackers(to, all_bb) & all_bb;
	b_xray		= BishopPseudoMoveBB[to]& (~attackers_bb) & (BLACK_BISHOP_BB	|BLACK_QUEEN_BB	| WHITE_BISHOP_BB	| WHITE_QUEEN_BB);
	r_xray		= RookPseudoMoveBB[to]	& (~attackers_bb) & (BLACK_ROOK_BB	|BLACK_QUEEN_BB	| WHITE_ROOK_BB		| WHITE_QUEEN_BB);
	gain[d]		= PieceValue[Board[to]];


	do	{
		d++;													// Profundidad siguiente
		gain[d] = PieceValue[attacker_piece] - gain[d - 1];

		if (fast && MAX(-gain[d - 1], gain[d]) < 0)
			break;


		attackers_bb^= from_bb;
		all_bb		^= from_bb;
		SAWP_TURN(side);										// Turno siguiente


		// Buscando al menor atacante
		for (; *piece[side]; piece[side]++)		{
			from_bb = attackers_bb & PieceBB[*piece[side]];

			if (from_bb)			{
				from_bb			= from_bb & -from_bb;			// 1 bit
				attacker_piece	= *piece[side];					// Guarda cual es el menor atacante
				break;
			}
		}


		// Detectando si existen piezas detras de otras que atacan a la casilla
		if (*piece[side])		{
			if (b_xray && (from_bb & BishopPseudoMoveBB[to]))	{
				bb				= BISHOP_MOVES_BB(to, all_bb ^ from_bb) & (~attackers_bb) & b_xray;
				b_xray			^= bb;
				attackers_bb	^= bb;

				// Puede haber alfiles tras la dama
				if (bb & (BLACK_QUEEN_BB | WHITE_QUEEN_BB))
					piece[side]	= &PieceBySide[side][2];
			}

			else if (r_xray && (from_bb & RookPseudoMoveBB[to])){
				bb				= ROOK_MOVES_BB(to, all_bb ^ from_bb) & (~attackers_bb) & r_xray;
				r_xray			^= bb;
				attackers_bb	^= bb;

				// Puede haber torres tras la dama
				if (bb & (BLACK_QUEEN_BB | WHITE_QUEEN_BB))
					piece[side]	= &PieceBySide[side][3];
			}
		}
	}
	while (*piece[side]);


	while (--d)
		gain[d - 1] = -MAX(-gain[d - 1], gain[d]);

	return gain[0];
}


// 128
