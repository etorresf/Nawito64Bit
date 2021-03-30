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


#ifndef ATTACK_H
#define ATTACK_H

#include "board.h"



/*
 * Detecta si la casilla 'Square' es atacada por el bando 'Side'.
 * Devuelve el tipo de pieza atacante (el ataque de Dama es devuelto como Alfil o Torre)
 */
#define			Attacked(sq, side)			(PAWN_ATTACK_BB(sq, side) || KNIGHT_ATTACK_BB(sq, side) || KING_ATTACK_BB(sq, side) || ((BISHOP_BB((side))| QUEEN_BB((side))) & BishopPseudoMoveBB[sq] & BISHOP_MOVES_BB(sq, ALL_PIECES_BB)) || ((ROOK_BB((side)) | QUEEN_BB((side))) & RookPseudoMoveBB[sq] & ROOK_MOVES_BB(sq, ALL_PIECES_BB)))

#define			InCheck(side)				Attacked(KING_SQ(side), FLIP_TURN(side))			// Determina si el Rey del bando 'side' esta en jaque

#define			GetAttackers(sq, all_bb)	(PAWN_ATTACK_BB(sq, WHITE)|PAWN_ATTACK_BB(sq,BLACK)|(KNIGHT_MOVES_BB(sq)&(BLACK_KNIGHT_BB|WHITE_KNIGHT_BB))|(KING_MOVES_BB(sq)&(BLACK_KING_BB|WHITE_KING_BB))|(BISHOP_MOVES_BB(sq,all_bb)&(BLACK_BISHOP_BB|WHITE_BISHOP_BB|BLACK_QUEEN_BB|WHITE_QUEEN_BB))|(ROOK_MOVES_BB(sq,all_bb)&(BLACK_ROOK_BB|WHITE_ROOK_BB|BLACK_QUEEN_BB|WHITE_QUEEN_BB)))

int				SEE(SQUARE from, SQUARE to, U8 fast);			// Usado para evitar una explosion de la QuiesceSearch


#endif // ATTACK_H
