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

#ifndef BOARD_H
#define BOARD_H

#include "bitboard.h"
#include "hash_mat.h"
#include "move.h"


//====================================================================================================================================
// Posición inicial del juego por defecto
#define				START_POS			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ; ;"

// Configuración del motor
typedef enum	{
	ABORT		= 1<<0,
	STOP		= 1<<1,
	FOLLOW_PV	= 1<<2,
	SEARCHING	= 1<<3,
	ANALYZING	= 1<<4,
	LEARNING	= 1<<5,
}	ENGINE_CONFIG;


extern TURN			EngineTurn;									// Color que tiene el programa (BLANCO o NEGRO)
extern ENGINE_CONFIG	Engine;											// Configuración del motor

extern TURN			Turn;										// Lado que mueve (BLANCO o NEGRO)
extern CASTTLE_RIGHT Casttle;									// Posibilidad para el enroque
extern SQUARE		EnPassant;									// Posibilidad para comer al paso
extern int			Rule50;										// Contador para la regla de los 50 movimientos
extern HASH_KEY		Hash;										// Llave hash para la actual posición
extern HASH_KEY		PawnHash;
extern HASH_MAT		HashMat;									// Asigna un número(no necesariamente único) a la posición según el material existente


extern int			PieceMat[2];								// Material acumulado por las PIEZAS(excepto PEONES)
extern int			PawnMat[2];									// Material acumulado por los PEONES


extern int			MaxDepth;									// Nivel de búsqueda en el árbol
extern int			Ply;										// Nivel de profundidad que estamos en el alpha_beta


extern PIECE		Board[64];									// Posición actual
extern SQUARE		KingSquare[2];

#define				MATCH(piece, sq)		(Board[sq] == (piece))
#define				NO_EMPTY(sq)			Board[sq]
#define				KING_SQ(side)			KingSquare[side]


extern int			PieceNum[16];

#define				BLACK_PAWNS				PieceNum[BLACK_PAWN]
#define				WHITE_PAWNS				PieceNum[WHITE_PAWN]
#define				BLACK_KNIGHTS			PieceNum[BLACK_KNIGHT]
#define				WHITE_KNIGHTS			PieceNum[WHITE_KNIGHT]
#define				BLACK_BISHOPS			PieceNum[BLACK_BISHOP]
#define				WHITE_BISHOPS			PieceNum[WHITE_BISHOP]
#define				BLACK_ROOKS				PieceNum[BLACK_ROOK]
#define				WHITE_ROOKS				PieceNum[WHITE_ROOK]
#define				BLACK_QUEENS			PieceNum[BLACK_QUEEN]
#define				WHITE_QUEENS			PieceNum[WHITE_QUEEN]


#define				PAWNS(side)				PieceNum[PAWN	| (side)]
#define				KNIGHTS(side)			PieceNum[KNIGHT	| (side)]
#define				BISHOPS(side)			PieceNum[BISHOP	| (side)]
#define				ROOKS(side)				PieceNum[ROOK	| (side)]
#define				QUEENS(side)			PieceNum[QUEEN	| (side)]

#define				ALL_PAWNS				(BLACK_PAWNS	+ WHITE_PAWNS)
#define				ALL_KNIGHTS				(BLACK_KNIGHTS	+ WHITE_KNIGHTS)
#define				ALL_BISHOPS				(BLACK_BISHOPS	+ WHITE_BISHOPS)
#define				ALL_ROOKS				(BLACK_ROOKS	+ WHITE_ROOKS)
#define				ALL_QUEENS				(BLACK_QUEENS	+ WHITE_QUEENS)


extern BITBOARD		PieceBB[16];

#define				ALL_PIECES_BB			(PieceBB[WHITE] | PieceBB[BLACK])

#define				PAWN_BB(side)			PieceBB[PAWN	| (side)]
#define				KNIGHT_BB(side)			PieceBB[KNIGHT	| (side)]
#define				BISHOP_BB(side)			PieceBB[BISHOP	| (side)]
#define				ROOK_BB(side)			PieceBB[ROOK	| (side)]
#define				QUEEN_BB(side)			PieceBB[QUEEN	| (side)]
#define				KING_BB(side)			PieceBB[KING	| (side)]

#define				BLACK_PAWN_BB			PieceBB[BLACK_PAWN]
#define				BLACK_KNIGHT_BB			PieceBB[BLACK_KNIGHT]
#define				BLACK_BISHOP_BB			PieceBB[BLACK_BISHOP]
#define				BLACK_ROOK_BB			PieceBB[BLACK_ROOK]
#define				BLACK_QUEEN_BB			PieceBB[BLACK_QUEEN]
#define				BLACK_KING_BB			PieceBB[BLACK_KING]

#define				WHITE_PAWN_BB			PieceBB[WHITE_PAWN]
#define				WHITE_KNIGHT_BB			PieceBB[WHITE_KNIGHT]
#define				WHITE_BISHOP_BB			PieceBB[WHITE_BISHOP]
#define				WHITE_ROOK_BB			PieceBB[WHITE_ROOK]
#define				WHITE_QUEEN_BB			PieceBB[WHITE_QUEEN]
#define				WHITE_KING_BB			PieceBB[WHITE_KING]

#define				PAWN_ATTACK_BB(sq, side)	(PAWN_BB(side) & PawnAttackBB[FLIP_TURN(side)][sq])
#define				KNIGHT_ATTACK_BB(sq, side)	(KNIGHT_BB(side) & KNIGHT_MOVES_BB(sq))
#define				KING_ATTACK_BB(sq, side)	(KING_BB(side) & KING_MOVES_BB(sq))
#define				BISHOP_ATTACK_BB(sq, side)	(BISHOP_BB(side) & BishopPseudoMoveBB[sq] & BISHOP_MOVES_BB(sq, ALL_PIECES_BB))
#define				ROOK_ATTACK_BB(sq, side)	(ROOK_BB(side) & RookPseudoMoveBB[sq] & ROOK_MOVES_BB(sq, ALL_PIECES_BB))
#define				QUEEN_ATTACK_BB(sq, side)	(QUEEN_BB(side) & QueenPseudoMoveBB[sq] & QUEEN_MOVES_BB(sq, ALL_PIECES_BB))


extern HIST_MOVE	Moves[HIST_STACK];							// Donde se almacena las jugadas que se van produciendo
extern int			Nmove;										// Número de movimientos en la partida (BLANCOS + NEGROS)


#define				HISTORY_SIZE	4096
#define				MAX_HISTORY		0
extern int			History[2][HISTORY_SIZE];					// Utilizado para la ordenación movimientos


extern CASTTLE_RIGHT CasttleMask[64];							// Si se mueve PIEZA que no sea el REY o TORRE estarán intactas las posibilidades de enroque(15)

#define				GAME_PHASE					MIN(256, (PieceMat[BLACK] + PieceMat[WHITE]) / 24)
#define				TAPERED_SCORE(mid_score, end_score)	((((mid_score) - (end_score)) * GAME_PHASE) / 256 + (end_score))


//====================================================================================================================================
/*
 * Detecta si la casilla 'sq' es atacada por el bando 'side'.
 * Devuelve el tipo de PIEZA atacante (el ataque de DAMA es devuelto como ALFIL o TORRE)
 */
#define				Attacked(sq, side)			(PAWN_ATTACK_BB(sq, side) || KNIGHT_ATTACK_BB(sq, side) || KING_ATTACK_BB(sq, side) || ((BISHOP_BB((side))| QUEEN_BB((side))) & BishopPseudoMoveBB[sq] & BISHOP_MOVES_BB(sq, ALL_PIECES_BB)) || ((ROOK_BB((side)) | QUEEN_BB((side))) & RookPseudoMoveBB[sq] & ROOK_MOVES_BB(sq, ALL_PIECES_BB)))
#define				IN_CHECK(side)				Attacked(KING_SQ(side), FLIP_TURN(side))			// Determina si el REY del bando 'side' esta en jaque

char*				LoadFEN(char* buffer);

int					RepeatPos(int n);							// Determina si la actual posición se ha repetido 'n' veces

void				QuitPiece(SQUARE sq, int upd_hash);
void				PutPiece(SQUARE sq, PIECE piece, int upd_hash);
void				MovePiece(SQUARE from, SQUARE to, int upd_hash);


#endif // BOARD_H


//	150
