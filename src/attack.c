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

#include "attack.h"


/*
 * Detecta si la casilla 'Square' es atacada por el bando 'Side'.
 * Devuelve el tipo de pieza atacante (el ataque de Dama es devuelto como Alfil o Torre)
 */
U8		Attacked(SQUARE Square, TURN Side)		{
	if (PAWN_ATTACK_BB(Square, Side))
		return PAWN;

	if (KNIGHT_BB(Side)	& KNIGHT_MOVES_BB(Square))
		return KNIGHT;

	if (KING_BB(Side)	& KING_MOVES_BB(Square))
		return KING;

	if ((BISHOP_BB(Side)| QUEEN_BB(Side)) & BishopPseudoMoveBB[Square]	& BISHOP_MOVES_BB(Square,	ALL_PIECES_BB))
		return BISHOP;

	if ((ROOK_BB(Side)	| QUEEN_BB(Side)) & RookPseudoMoveBB[Square]	& ROOK_MOVES_BB(Square,		ALL_PIECES_BB))
		return ROOK;

	return EMPTY_PIECE;
}


// Determina si es una mala captura desde 'From' a 'To'. Evitando realizar un costoso SEE
int		BadCapture(SQUARE From, SQUARE To)		{
	int			Gain	= PieceValue[Board[From]] - PieceValue[Board[To]];
	TURN		Side	= GET_COLOR(Board[From]),
				Cside	= FLIP_TURN(Side);


	// Buscar si existe pieza enemiga recapturadora con valor inferior a 'Gain'
	if (Gain > PAWN_VALUE)		{
		if (PAWN_ATTACK_BB(To, Cside))
			return PAWN;

		if (Gain > KNIGHT_VALUE){
			if (KNIGHT_BB(Cside) & KNIGHT_MOVES_BB(To))
				return KNIGHT;

			if (BISHOP_BB(Cside) & BishopPseudoMoveBB[To] & BISHOP_MOVES_BB(To, ALL_PIECES_BB))
				return BISHOP;

			if ((Gain > ROOK_VALUE) && (ROOK_BB(Cside)	& RookPseudoMoveBB[To] & ROOK_MOVES_BB(To, ALL_PIECES_BB)))
				return ROOK;
		}
	}

	return 0;													// Capturas de piezas >= al capturador son buenas
}


PIECE	PieceBySide[2][8]			= {
	{BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING, 0, 0,},
	{WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING, 0, 0,},
};

/*
 * Usado para evitar una explosion de la QuiesceSearch. no tiene en cuenta la captura al paso.
 * Ver 'SEE - The Swap Algorithm' de 'www.chessprogramming.org' en 'Home/Board Representation/Bitboards/SEE - The Swap Algorithm'
 */
int		SEE(TURN Side, SQUARE To, SQUARE From, U8 Fast)	{
	int			Gain[32],
				d				= 0;
	PIECE		*Piece[2]		= {PieceBySide[BLACK], PieceBySide[WHITE],},
				APiece			= Board[From];
	BITBOARD	FromBB			= SquareBB[From],
				AllBB, AttackersBB, Bxray, Rxray, BB;


	// Salida rápida
	if (Fast && APiece && Board[To] && PieceValue[Board[To]] >= PieceValue[APiece])
		return 0;

	if (!Attacked(To, FLIP_TURN(Side)))
		return 0;


	// Inicializando ataques a la casilla y posibles ataques de piezas escondidas
	AllBB		= ALL_PIECES_BB ^ SquareBB[To];
	AttackersBB	= GetAttackers(To, AllBB) & AllBB;
	Bxray		= BishopPseudoMoveBB[To]& (~AttackersBB) & (BLACK_BISHOP_BB	|BLACK_QUEEN_BB	| WHITE_BISHOP_BB	| WHITE_QUEEN_BB);
	Rxray		= RookPseudoMoveBB[To]	& (~AttackersBB) & (BLACK_ROOK_BB	|BLACK_QUEEN_BB	| WHITE_ROOK_BB		| WHITE_QUEEN_BB);
	Gain[d]		= PieceValue[Board[To]];


	do	{
		d++;													// Profundidad siguiente
		Gain[d] = PieceValue[APiece] - Gain[d - 1];

		if (Fast && MAX(-Gain[d - 1], Gain[d]) < 0)
			break;


		AttackersBB ^= FromBB;
		AllBB		^= FromBB;
		SAWP_TURN(Side);										// Turno siguiente


		// Buscando al menor atacante
		for (; *Piece[Side]; Piece[Side]++)		{
			FromBB = AttackersBB & PieceBB[*Piece[Side]];

			if (FromBB)			{
				FromBB	= FromBB & -FromBB;						// 1 bit
				APiece	= *Piece[Side];							// Guarda cual es el menor atacante
				break;
			}
		}


		// Detectando si existen piezas detras de otras que atacan a la casilla
		if (*Piece[Side])		{
			if (Bxray && (FromBB & BishopPseudoMoveBB[To]))		{
				BB			= BISHOP_MOVES_BB(To, AllBB ^ FromBB) & (~AttackersBB) & Bxray;
				Bxray		^= BB;
				AttackersBB	^= BB;

				// Puede haber alfiles tras la dama
				if (BB & (BLACK_QUEEN_BB | WHITE_QUEEN_BB))
					Piece[Side] = &PieceBySide[Side][2];
			}

			else if (Rxray && (FromBB & RookPseudoMoveBB[To]))	{
				BB			= ROOK_MOVES_BB(To, AllBB ^ FromBB) & (~AttackersBB) & Rxray;
				Rxray		^= BB;
				AttackersBB	^= BB;

				// Puede haber torres tras la dama
				if (BB & (BLACK_QUEEN_BB | WHITE_QUEEN_BB))
					Piece[Side] = &PieceBySide[Side][3];
			}
		}
	}
	while (*Piece[Side]);


	while (--d)
		Gain[d - 1] = -MAX(-Gain[d - 1], Gain[d]);

	return Gain[0];
}


// 128
