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

#ifndef BOARD_H
#define BOARD_H

#include "hash.h"
#include "hash_mat.h"
#include "bitboard.h"
#include "move.h"
#include "time.h"


// Posición inicial del juego por defecto
#define				START_POS			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ; ;"


extern STATE		EngineState;								// Estado del motor
extern TURN			EngineTurn;									// Color que tiene el programa (BLANCO o NEGRO)


extern TURN			Turn;										// Lado que mueve (BLANCO o NEGRO)
extern CASTTLE_RIGHT Casttle;									// Posibilidad para el enroque
extern SQUARE		EnPassant;									// Posibilidad para comer al paso
extern int			Rule50;										// Contador para la regla de los 50 movimientos
extern HASH_KEY		Hash;										// Llave hash para la actual posición
extern U8			CurrentAge;
extern HASH_MAT		HashMat;									// Asigna un número(no necesariamente único) a la posición según el material existente


extern int			MatSum;										// Suma del material acumulado por ambos bandos
extern int			PieceMat[2];								// Material acumulado por las piezas(excepto peones)
extern int			PawnMat[2];									// Material acumulado por los peones


extern int			MaxDepth;									// Nivel de búsqueda en el árbol
extern int			Li;											// Profundidad desde la que se inicia
extern int			Ply;										// Nivel de profundidad que estamos en el alpha_beta
extern int			Nodes;										// Contador de todas las visitas a nodos


extern PIECE		Board[64];									// Posición actual
extern SQUARE		KingSquare[2];

#define				MATCH(Piece, Square)	(Board[Square] == (Piece))
#define				NO_MATCH(Piece, Square)	(Board[Square] ^ (Piece))
#define				IS_EMPTY(Square)		!Board[Square]
#define				NO_EMPTY(Square)		Board[Square]
#define				KING_SQ(Side)			KingSquare[Side]


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

#define				ALL_PAWNS				(BLACK_PAWNS	+ WHITE_PAWNS)
#define				ALL_KNIGHTS				(BLACK_KNIGHTS	+ WHITE_KNIGHTS)
#define				ALL_BISHOPS				(BLACK_BISHOPS	+ WHITE_BISHOPS)
#define				ALL_ROOKS				(BLACK_ROOKS	+ WHITE_ROOKS)
#define				ALL_QUEENS				(BLACK_QUEENS	+ WHITE_QUEENS)

#define				PAWNS(Side)				PieceNum[PAWN	| (Side)]
#define				KNIGHTS(Side)			PieceNum[KNIGHT	| (Side)]
#define				BISHOPS(Side)			PieceNum[BISHOP	| (Side)]
#define				ROOKS(Side)				PieceNum[ROOK	| (Side)]
#define				QUEENS(Side)			PieceNum[QUEEN	| (Side)]


extern BITBOARD		PieceBB[16];
extern BITBOARD		AllPieceBB[2];

#define				ALL_PIECES_BB			(AllPieceBB[WHITE] | AllPieceBB[BLACK])

#define				PAWN_BB(Side)			PieceBB[PAWN	| (Side)]
#define				KNIGHT_BB(Side)			PieceBB[KNIGHT	| (Side)]
#define				BISHOP_BB(Side)			PieceBB[BISHOP	| (Side)]
#define				ROOK_BB(Side)			PieceBB[ROOK	| (Side)]
#define				QUEEN_BB(Side)			PieceBB[QUEEN	| (Side)]
#define				KING_BB(Side)			PieceBB[KING	| (Side)]

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

#define				PAWN_ATTACK_BB(SQ, Side) (PAWN_BB(Side) & PawnAttackBB[FLIP_TURN(Side)][SQ])


extern HIST_MOVE	Moves[HIST_STACK];							// Donde se almacena las jugadas que se van produciendo
extern int			Nmove;										// Número de movimientos en la partida (blancos + negros)
extern int			NmoveInit;									// Número de movimientos al comenzar la partida
extern STACK_MOVE	MoveStack[GEN_STACK];						// Memoria para los movimientos generados por el generador de movimientos
extern STACK_MOVE*	FirstMove[MAX_DEPHT_1];						// Indica donde comienza la primera jugada de la lista de movimientos en el nivel n (primer_mov[n])


#define HISTORY_SIZE 4096
extern int			History[2][HISTORY_SIZE];					// Utilizado para la ordenación movimientos
extern int			MaxHistory[2];								// Limita la historia para que no sobrepase el valor de otros movimientos


extern MOVE			Killer1[MAX_DEPHT_1];						// Guarda los movimientos que producen un corte
extern MOVE			Killer2[MAX_DEPHT_1];						// Guarda los movimientos que producen un corte
extern MOVE			MateKiller[MAX_DEPHT_1];					// Movimiento que amenaza mate


extern CASTTLE_RIGHT CasttleMask[64];							// Si se mueve pieza que no sea el rey o torre estarán intactas las posibilidades de enroque(15)


long				LoadFEN(char* Buffer);

U8					RepeatPos(U8 n);							// Determina si la actual posición se ha repetido 'n' veces



#endif // BOARD_H
