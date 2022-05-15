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

#include "board.h"


//====================================================================================================================================
TURN				EngineTurn;

TURN				Turn;
CASTTLE_RIGHT		Casttle;
SQUARE				EnPassant;
int					Rule50;
HASH_KEY			PawnHash;
HASH_KEY			Hash;
HASH_MAT			HashMat;

int					PieceMat[2];
int					PawnMat[2];

int					MaxDepth;
int					Ply;

PIECE				Board[64];
SQUARE				KingSquare[2];

int					PieceNum[16];

BITBOARD			PieceBB[16];

HIST_MOVE			Moves[HIST_STACK];
int					Nmove;

int					History[2][HISTORY_SIZE];

CASTTLE_RIGHT		CasttleMask[64];


//====================================================================================================================================
// Inicializa los datos correspondientes a la representación del tablero
void	ClearBoard()			{
	memset(CasttleMask,		15,	sizeof(CasttleMask));
	memset(Moves,			0,	sizeof(Moves));
	memset(History,			0,	sizeof(History));
	memset(PieceMat,		0,	sizeof(PieceMat));
	memset(PawnMat,			0,	sizeof(PawnMat));
	memset(Board,			0,	sizeof(Board));
	memset(PieceBB,			0,	sizeof(PieceBB));
	memset(PieceNum,		0,	sizeof(PieceNum));

	// Se inicializa la mascara del enroque
	CasttleMask[E1] ^= WQ | WK;
	CasttleMask[E8] ^= BQ | BK;
	CasttleMask[A1] ^= WQ;
	CasttleMask[H1] ^= WK;
	CasttleMask[A8] ^= BQ;
	CasttleMask[H8] ^= BK;

	// Se inicializan algunas variables globales
	MaxDepth	= MAX_DEPHT;
	Nmove		= 0;
	Casttle		= 0;
	EnPassant	= Ply = Rule50 = 0;
	EngineTurn	= Turn = EMPTY_TURN;
	HashMat		= Hash = PawnHash = 0;
}


// Carga una posicion en formato FEN. No comprueba que esta sea legal
void	LoadFEN(char* buffer)	{
	SQUARE		sq, sq_fliped;
	char		*c;


	ClearBoard();
	ClearHash();

	// Poniendo piezas en el tablero
	for (sq = A1, c = buffer; sq <= H8 && *c != '\0'; c++)		{
		sq_fliped = FLIP_SQ(sq);

		switch (*c)				{
			case 'p':	PutPiece(sq_fliped, BLACK_PAWN,		1);		break;
			case 'P':	PutPiece(sq_fliped, WHITE_PAWN,		1);		break;
			case 'n':	PutPiece(sq_fliped, BLACK_KNIGHT,	1);		break;
			case 'N':	PutPiece(sq_fliped, WHITE_KNIGHT,	1);		break;
			case 'b':	PutPiece(sq_fliped, BLACK_BISHOP,	1);		break;
			case 'B':	PutPiece(sq_fliped, WHITE_BISHOP,	1);		break;
			case 'r':	PutPiece(sq_fliped, BLACK_ROOK,		1);		break;
			case 'R':	PutPiece(sq_fliped, WHITE_ROOK,		1);		break;
			case 'q':	PutPiece(sq_fliped, BLACK_QUEEN,	1);		break;
			case 'Q':	PutPiece(sq_fliped, WHITE_QUEEN,	1);		break;
			case 'k':	PutPiece(sq_fliped, BLACK_KING,		1);		break;
			case 'K':	PutPiece(sq_fliped, WHITE_KING,		1);		break;

			case '/': case ' ': case '\t': case '\n':			continue;
			case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':	sq += *c - '0'; continue;
			default:	return;
		}

		sq++;
	}

	// Poniendo lado que le toca jugar
	CONSUME_ESPACE(c);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		if (*c == 'w')
			Turn = WHITE;

		else if (*c == 'b')
			Turn = BLACK;

		else return;

		c++;
	}

	// Actualiza la posibilidad de realizar el enroque	
	CONSUME_ESPACE(c);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		for (; *c != ' '; c++)
			switch (*c)		{
				case 'K':	Casttle |= WK;	break;
				case 'Q':	Casttle |= WQ;	break;
				case 'k':	Casttle |= BK;	break;
				case 'q':	Casttle |= BQ;	break;
				case '-':	break;
				default:;	return;
			}
		c++;
	}

	// Actualiza la casilla de captura al paso
	CONSUME_ESPACE(c);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		if (*c != '-')	{
			if (*c >= 'a' && *c <= 'h' && *(c + 1) >= '1' && *(c + 1) <= '8')
				EnPassant = (*c - 'a') + 8 * (*(c + 1) - '1');

			c += 2;
		}
		c++;
	}

	// Actualiza la regla de 50 movimientos
	CONSUME_ESPACE(c);
	for (; '0' <= *c && *c <= '9'; c++)
		Rule50 = 10 * Rule50 + (int)(*c - '0');

	// actualizando hash (Captura al paso y enroque)
	Hash ^= HashTurn[Turn] ^ HashCasttle[Casttle];
	if (EnPassant && PAWN_ATTACK_BB(EnPassant, FLIP_TURN(Turn)))
		Hash ^= HashEnPassant[EnPassant];
}


// Determina si la actual posición se ha repetido 'n' veces. Al menos se repite 1 vez
int		RepeatPos(int n)		{
	int		i, j	= Nmove - Rule50,
			count	= 1;

	for (i = Nmove - 2; i >= j; i -= 2)
		if ((Moves[i].Hash == Hash) && (++count >= n))
			return 1;

	return 0;
}


// Quita una pieza del tablero y actualiza sus datos
void	QuitPiece(SQUARE sq, int upd_hash)		{
	PIECE		piece		= Board[sq];
	TURN		side		= GET_COLOR(piece);
	BITBOARD	bb			= SQ_BB(sq);
	HASH_KEY	hash_piece	= HashPiece[piece][sq];


	// Quita la pieza del tablero y los bitboard
	Board[sq]		= EMPTY_PIECE;

	PieceBB[piece]	^= bb;
	PieceBB[side]	^= bb;

	// Actualiza las hash
	HashMat			-= HashMatKey[piece];
	if (upd_hash)
		Hash		^= hash_piece;

	// Actualiza datos referente al material y la hash de peones
	switch (piece ^ side)		{
		case PAWN:
			PawnHash			^= hash_piece;
			PawnMat[side]		-= PAWN_VALUE;
			break;

		case KING: PawnHash		^= hash_piece;		break;

		default: PieceMat[side]	-= PieceValue[piece];
	}

	PieceNum[piece]--;
}


// Pone una pieza en el tablero y actualiza sus datos
void	PutPiece(SQUARE sq, PIECE piece, int upd_hash)			{
	TURN		side		= GET_COLOR(piece);
	BITBOARD	bb			= SQ_BB(sq);
	HASH_KEY	hash_piece	= HashPiece[piece][sq];


	// Pone la pieza en el tablero y los bitboard
	Board[sq]		= piece;

	PieceBB[piece]	^= bb;
	PieceBB[side]	^= bb;

	// Actualiza las hash
	HashMat			+= HashMatKey[piece];
	if (upd_hash)
		Hash		^= hash_piece;

	// Actualiza datos referente al material y la hash de peones
	switch (piece ^ side)		{
		case PAWN:
			PawnHash			^= hash_piece;
			PawnMat[side]		+= PAWN_VALUE;
			break;

		case KING:
			PawnHash			^= hash_piece;
			KingSquare[side]	= sq;
			break;

		default: PieceMat[side]	+= PieceValue[piece];
	}

	PieceNum[piece]++;
}


// Mueve una pieza del tablero y actualiza sus datos
void	MovePiece(SQUARE from, SQUARE to, int upd_hash)			{
	PIECE		piece	= Board[from];
	TURN		side	= GET_COLOR(piece);
	BITBOARD	bb		= SQ_BB(from) ^ SQ_BB(to);
	HASH_KEY	hash_mov= HashPiece[piece][from] ^ HashPiece[piece][to];


	// Pone la pieza en el tablero y los bitboard
	Board[to]		= piece;
	Board[from]		= EMPTY_PIECE;

	PieceBB[piece]	^= bb;
	PieceBB[side]	^= bb;

	// Actualiza las hash
	if (upd_hash)
		Hash	^= hash_mov;

	// Actualiza datos referente a la hash de peones y posicion del rey
	switch (piece ^ side)		{
		case PAWN:
			PawnHash		^= hash_mov;
			break;

		case KING:
			PawnHash		^= hash_mov;
			KingSquare[side] = to;
			break;

		default:;
	}
}


//	300	310
