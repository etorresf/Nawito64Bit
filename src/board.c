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

#include "board.h"


STATE				EngineState;
TURN				EngineTurn;


TURN				Turn;
CASTTLE_RIGHT		Casttle;
SQUARE				EnPassant;
int					Rule50;
HASH_KEY			Hash;
U8					CurrentAge;
HASH_MAT			HashMat;


int					MatSum;
int					PieceMat[2];
int					PawnMat[2];


int					MaxDepth;
int					Li;
int					Ply;
int					Nodes;


PIECE				Board[64];
SQUARE				KingSquare[2];


int					PieceNum[16];


BITBOARD			PieceBB[16];
BITBOARD			AllPieceBB[2];


HIST_MOVE			Moves[HIST_STACK];
int					Nmove;
int					NmoveInit;
STACK_MOVE			MoveStack[GEN_STACK];
STACK_MOVE*			FirstMove[MAX_DEPHT_1];


int					History[2][HISTORY_SIZE];
int					MaxHistory[2];


MOVE				Killer1[MAX_DEPHT_1];
MOVE				Killer2[MAX_DEPHT_1];
MOVE				MateKiller[MAX_DEPHT_1];


CASTTLE_RIGHT		CasttleMask[64];


// Actualiza los datos del tablero despues de cargar una posicion
void	UpdateBoard(void)		{
	PIECE		Piece;
	SQUARE		Square;
	TURN		Side;


	// Recorrer tablero para actualizar datos
	SQ_LOOP(Square)
		if (Board[Square])		{
			Piece	= Board[Square];
			Side	= GET_COLOR(Piece);


			PieceNum[Piece]++;

			// Actualiza el hash material
			HashMat += HashMatKey[Piece];

			// Actualiza el tablero Bitboard
			PieceBB[Piece]		|= SquareBB[Square];
			AllPieceBB[Side]	|= SquareBB[Square];

			// Actualiza el material(no del Rey) y posicion del Rey
			if (NOT_PAWN(Piece)){
				if (Piece >= KING)
					KingSquare[Side]	= Square;

				else PieceMat[Side]		+= PieceValue[Piece];
			}

			else PawnMat[Side]	+= PAWN_VALUE;
		}


	// Actualiza la suma del material
	MatSum		= PieceMat[BLACK] + PawnMat[BLACK] + PieceMat[WHITE] + PawnMat[WHITE];

	// Actualiza las Hash, primero es necesario limpiarlas
	Hash		= PutHash(Board, Turn, Casttle, EnPassant);

	MaxDepth	= MAX_DEPHT;
	EngineTurn	= FLIP_TURN(Turn);
}


// Inicializa los datos correspondientes a la representación del tablero
void	ClearBoard(void)		{
	memset(CasttleMask,		15,	sizeof(CasttleMask));
	memset(Moves,			0,	sizeof(Moves));
	memset(History,			0,	sizeof(History));
	memset(MaxHistory,		0,	sizeof(MaxHistory));
	memset(PieceMat,		0,	sizeof(PieceMat));
	memset(PawnMat,			0,	sizeof(PawnMat));
	memset(Board,			0,	sizeof(Board));
	memset(PieceBB,			0,	sizeof(PieceBB));
	memset(AllPieceBB,		0,	sizeof(AllPieceBB));
	memset(PieceNum,		0,	sizeof(PieceNum));


	// Se inicializa la mascara del enroque
	CasttleMask[E1] = 12;
	CasttleMask[E8] = 3;
	CasttleMask[A1] = 13;
	CasttleMask[H1] = 14;
	CasttleMask[A8] = 7;
	CasttleMask[H8] = 11;


	// Se inicializan algunas variables globales
	MaxDepth	= MAX_DEPHT;
	Nmove		= NmoveInit = 2;
	Casttle		= 0;
	EnPassant	= Ply = MatSum = Rule50 = /*EngineState =*/ 0;
	EngineTurn	= Turn = EMPTY_TURN;
	HashMat		= Hash = 0;

	FirstMove[Ply]	= MoveStack;
}


long	LoadFEN(char* Buffer)	{
	SQUARE		Square, RelSQ;
	char		*c;


	// Limpiar tablero	y datos del mismo
	ClearBoard();

	// Recorre la cadena buffer, rellenando el tablero
	for (Square = A1, c = Buffer; Square <= H8 && *c != '\0' && *c != '\n'; c++){
		RelSQ = FLIP_SQ(Square);

		switch (*c)				{
			case 'p':	Board[RelSQ] = BLACK_PAWN;		break;
			case 'P':	Board[RelSQ] = WHITE_PAWN;		break;
			case 'n':	Board[RelSQ] = BLACK_KNIGHT;	break;
			case 'N':	Board[RelSQ] = WHITE_KNIGHT;	break;
			case 'b':	Board[RelSQ] = BLACK_BISHOP;	break;
			case 'B':	Board[RelSQ] = WHITE_BISHOP;	break;
			case 'r':	Board[RelSQ] = BLACK_ROOK;		break;
			case 'R':	Board[RelSQ] = WHITE_ROOK;		break;
			case 'q':	Board[RelSQ] = BLACK_QUEEN;		break;
			case 'Q':	Board[RelSQ] = WHITE_QUEEN;		break;
			case 'k':	Board[RelSQ] = BLACK_KING;		break;
			case 'K':	Board[RelSQ] = WHITE_KING;		break;

			case '/':	continue;
			case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':	Square += *c - '0'; continue;
			default:;
		}

		Square++;
	}


	// Actualiza el lado que le toca jugar
	for (; *c == ' '; c++);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		if (*c == 'w')
			Turn = WHITE;

		else if (*c == 'b')
			Turn = BLACK;

		c++;
	}


	// Actualiza la posibilidad de realizar el enroque	
	for (; *c == ' '; c++);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		for (; *c != ' '; c++)
			switch (*c)		{
				case 'K':	Casttle |= WK;	break;
				case 'Q':	Casttle |= WQ;	break;
				case 'k':	Casttle |= BK;	break;
				case 'q':	Casttle |= BQ;	break;
				case '-':	break;
				default:;	// ERROR, caracter no esperado
			}
		c++;
	}


	// Actualiza la casilla de captura al paso
	for (; *c == ' '; c++);
	if (*c != '\0' && *c != '\n' && *c != ';')	{
		if (*c != '-')	{
			if (*c >= 'a' && *c <= 'h' && *(c + 1) >= '1' && *(c + 1) <= '8')
				EnPassant = (*c - 'a') + 8 * (*(c + 1) - '1');

			c += 2;
		}
		c++;
	}


	// Actualiza la regla de 50 movimientos
	for (; *c == ' '; c++);
	for (; '0' <= *c && *c <= '9'; c++)
		Rule50 = 10 * Rule50 + (int)(*c - '0');


	// Actualiza el numero de movimientos
	for (; *c == ' '; c++);
	for (Nmove = 0; '0' <= *c && *c <= '9'; c++)
		Nmove = 10 * Nmove + (int)(*c - '0');

	NmoveInit = Nmove = MAX(2, 2 * Nmove) + (Turn == BLACK);
	c++;


	UpdateBoard();
	return c - Buffer + 1;
}


// Determina si la actual posición se ha repetido 'n' veces. Al menos se repite 1 vez
U8		RepeatPos(U8 n)			{
	int		i, j	= Nmove - Rule50;
	U8		Count	= 1;

	for (i = Nmove - 2; i >= j; i -= 2)
		if ((Moves[i].Hash == Hash) && (++Count >= n))
			return 1;

	return 0;
}


// 300
