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

#include "see.h"


//====================================================================================================================================
#define			GetAttackers(sq, all_bb)	(PAWN_ATTACK_BB(sq, WHITE)|PAWN_ATTACK_BB(sq,BLACK)|(KNIGHT_MOVES_BB(sq)&(BLACK_KNIGHT_BB|WHITE_KNIGHT_BB))|(KING_MOVES_BB(sq)&(BLACK_KING_BB|WHITE_KING_BB))|(BISHOP_MOVES_BB(sq,all_bb)&(BLACK_BISHOP_BB|WHITE_BISHOP_BB|BLACK_QUEEN_BB|WHITE_QUEEN_BB))|(ROOK_MOVES_BB(sq,all_bb)&(BLACK_ROOK_BB|WHITE_ROOK_BB|BLACK_QUEEN_BB|WHITE_QUEEN_BB)))


/*
 * No tiene en cuenta la captura al paso, ni promociones.
 * Ver (SEE - The Swap Algorithm) de <www.chessprogramming.org/Home/Board Representation/Bitboards/SEE - The Swap Algorithm>
 *
 * 'fast = 0'.		Realiza el SEE completamente
 * 'fast = 1'.		Realiza el SEE con algunas optimizaciones
 */
int		SEE(SQUARE from, SQUARE to, int fast)	{
	int			gain[32],
				d					= 0;
	PIECE		piece,
				attacker_piece		= Board[from];
	TURN		side				= GET_COLOR(attacker_piece);
	BITBOARD	from_bb				= SQ_BB(from),
				attacker_side_bb	= 1,
				all_piece_bb, attackers_bb, slide_piece_bb, b_xray, r_xray, bb;


	gain[0]		= PieceValue[Board[to]];

	if (fast && gain[0] >= PieceValue[attacker_piece])
		return gain[0] - PieceValue[attacker_piece];			// Salida rápida

	if (!Attacked(to, FLIP_TURN(side)))
		return gain[0];											// Salida rápida


	// Inicializando ataques a la casilla y posibles ataques de PIEZAS escondidas
	all_piece_bb	= ALL_PIECES_BB;
	slide_piece_bb	= all_piece_bb ^ (BLACK_KING_BB | BLACK_KNIGHT_BB | WHITE_KING_BB | WHITE_KNIGHT_BB);
	if (gain[0])
		all_piece_bb^= SQ_BB(to);


	attackers_bb= GetAttackers(to, all_piece_bb) & all_piece_bb;
	b_xray		= BishopPseudoMoveBB[to]& (~attackers_bb) & (BLACK_QUEEN_BB	| WHITE_QUEEN_BB | BLACK_BISHOP_BB	| WHITE_BISHOP_BB);
	r_xray		= RookPseudoMoveBB[to]	& (~attackers_bb) & (BLACK_QUEEN_BB	| WHITE_QUEEN_BB | BLACK_ROOK_BB	| WHITE_ROOK_BB);


	while (attacker_side_bb)	{
		d++;													// Profundidad siguiente
		gain[d] = PieceValue[attacker_piece] - gain[d - 1];

		if (fast && MAX(-gain[d - 1], gain[d]) < 0)
			break;


		attackers_bb	^= from_bb;
		all_piece_bb	^= from_bb;
		side			= FLIP_TURN(side);						// Turno siguiente


		attacker_side_bb = attackers_bb & PieceBB[side];

		if (attacker_side_bb)	{
			// Buscando al menor atacante
			for (piece = PAWN | side; piece <= WHITE_KING; piece += 2)			{
				from_bb = attackers_bb & PieceBB[piece];

				if (from_bb)	{
					from_bb			= from_bb & -from_bb;		// 1 bit
					attacker_piece	= piece;					// Guarda cual es el menor atacante
					break;
				}
			}


			// Detectando si existen PIEZAS detras de otras que atacan a la casilla
			if (slide_piece_bb & from_bb)		{
				if (b_xray && (from_bb & BishopPseudoMoveBB[to]))				{
					bb				= BISHOP_MOVES_BB(to, all_piece_bb ^ from_bb) & b_xray;
					b_xray			^= bb;
					attackers_bb	^= bb;
				}

				else if (r_xray && (from_bb & RookPseudoMoveBB[to]))			{
					bb				= ROOK_MOVES_BB(to, all_piece_bb ^ from_bb) & r_xray;
					r_xray			^= bb;
					attackers_bb	^= bb;
				}
			}
		}
	}


	while (--d)
		gain[d - 1] = -MAX(-gain[d - 1], gain[d]);

	return gain[0];
}


//	100
