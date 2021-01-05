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

#ifndef ATTACK_H
#define ATTACK_H

#include "board.h"


U8				Attacked(SQUARE Square, TURN Side);				// Detecta si la casilla 'Square' es atacada por el bando 'Side'

#define			InCheck(Side)				Attacked(KING_SQ(Side), FLIP_TURN(Side))			// Determina si el Rey del bando 'Side' esta en jaque

int				BadCapture(SQUARE From, SQUARE To);				// Determina si es una mala captura desde 'From' a 'To'. Evitando realizar un costoso SEE

#define			GetAttackers(SQ, AllBB) (PAWN_ATTACK_BB(SQ, WHITE)|PAWN_ATTACK_BB(SQ,BLACK)|(KNIGHT_MOVES_BB(SQ)&(BLACK_KNIGHT_BB|WHITE_KNIGHT_BB))|(KING_MOVES_BB(SQ)&(BLACK_KING_BB|WHITE_KING_BB))|(BISHOP_MOVES_BB(SQ,AllBB)&(BLACK_BISHOP_BB|WHITE_BISHOP_BB|BLACK_QUEEN_BB|WHITE_QUEEN_BB))|(ROOK_MOVES_BB(SQ,AllBB)&(BLACK_ROOK_BB|WHITE_ROOK_BB|BLACK_QUEEN_BB|WHITE_QUEEN_BB)))

int				SEE(TURN Side, SQUARE To, SQUARE From, U8 Fast);// Usado para evitar una explosion de la QuiesceSearch


#endif // ATTACK_H
