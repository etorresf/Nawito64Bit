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


#include "board.h"



STATE				EngineState;
TURN				EngineTurn;


TURN				Turn;
CASTTLE_RIGHT		Casttle;
SQUARE				EnPassant;
int					Rule50;
HASH_KEY			Hash;
//U8					CurrentAge;
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
STACK_MOVE			MoveStack[MAX_DEPHT_1][256];


int					History[2][HISTORY_SIZE];
int					MaxHistory[2];


MOVE				Killer1[MAX_DEPHT_1];
MOVE				Killer2[MAX_DEPHT_1];
MOVE				MateKiller[MAX_DEPHT_1];


CASTTLE_RIGHT		CasttleMask[64];


// Imprime la lista de movimientos realizados en la partida
void	PrintMoves(void)		{
	int			i		= NmoveInit;
	TURN		side	= WHITE;


	if (i == Nmove)
		return;

	if (i & 1)	{
		side = BLACK;
		printf(" %u. ....", i / 2);
	}


	for (; i < Nmove && Moves[i].Move; i++)		{
		printf(" ");

		if (side == WHITE)
			printf("%u. ", i / 2);


		PrintMove(Moves[i].Move);


		if (i % 10 == 1)
			printf("\n");

		SAWP_TURN(side);
	}

	printf("\n\n\n");
}


// Imprime el tablero. Útil para solucionar errores
void	PrintBoard(void)		{
	SQUARE		sq, sq_fliped;
	char		move_char[8];
	MOVE		last_move	= Moves[Nmove - 1].Move;


	printf("\n\n");

	// Muestra los nombres de los perfiles y el ultimo movimiento
	if (last_move)				{
		SprintMove(last_move, move_char);

		if (Turn == BLACK)
			printf("   (%s)", move_char);

		else if (Turn == WHITE)
			printf("\t\t\t      (%s)", move_char);
	}


	SQ_LOOP(sq)	{
		sq_fliped = FLIP_SQ(sq);

		if (FILE(sq_fliped) == FILE_A)
			printf("\n   +---+---+---+---+---+---+---+---+\n %u |", RANK(sq_fliped) + 1);

		printf(" %c |", PieceChar[Board[sq_fliped]]);
	}

	printf("\n   +---+---+---+---+---+---+---+---+\n     a   b   c   d   e   f   g   h\n\n");


	PrintMoves();												// Mostrando lista de movmientos
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
	EnPassant	= Ply = MatSum = Rule50 = 0;
	EngineTurn	= Turn = EMPTY_TURN;
	HashMat		= Hash = 0;
}


long	LoadFEN(char* buffer)	{
	SQUARE		sq, sq_fliped;
	char		*c;


	ClearBoard();
	ClearHash();

	for (sq = A1, c = buffer; sq <= H8 && *c != '\0' && *c != '\n'; c++)		{
		sq_fliped = FLIP_SQ(sq);

		switch (*c)				{
			case 'p':	PutPiece(sq_fliped, BLACK_PAWN, 1);		break;
			case 'P':	PutPiece(sq_fliped, WHITE_PAWN, 1);		break;
			case 'n':	PutPiece(sq_fliped, BLACK_KNIGHT, 1);	break;
			case 'N':	PutPiece(sq_fliped, WHITE_KNIGHT, 1);	break;
			case 'b':	PutPiece(sq_fliped, BLACK_BISHOP, 1);	break;
			case 'B':	PutPiece(sq_fliped, WHITE_BISHOP, 1);	break;
			case 'r':	PutPiece(sq_fliped, BLACK_ROOK, 1);		break;
			case 'R':	PutPiece(sq_fliped, WHITE_ROOK, 1);		break;
			case 'q':	PutPiece(sq_fliped, BLACK_QUEEN, 1);	break;
			case 'Q':	PutPiece(sq_fliped, WHITE_QUEEN, 1);	break;
			case 'k':	PutPiece(sq_fliped, BLACK_KING, 1);		break;
			case 'K':	PutPiece(sq_fliped, WHITE_KING, 1);		break;

			case '/':	continue;
			case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':	sq += *c - '0'; continue;
			default:;
		}

		sq++;
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
				default:;										// ERROR, caracter no esperado
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


	Hash ^= HashTurn[Turn] ^ HashCasttle[Casttle];
	if (EnPassant)				{
		sq = EnPassant + Front[FLIP_TURN(Turn)] + 1;
		if (abs(FILE(EnPassant) - FILE(sq)) < 2	&& Board[sq] == (PAWN | Turn))
			Hash ^= HashEnPassant[EnPassant];

		else	{
			sq = EnPassant + Front[FLIP_TURN(Turn)] - 1;
			if (abs(FILE(EnPassant) - FILE(sq)) < 2	&& Board[sq] == (PAWN | Turn))
				Hash ^= HashEnPassant[EnPassant];
		}
	}

	EngineTurn = FLIP_TURN(Turn);
	return c - buffer + 1;
}


// Determina si la actual posición se ha repetido 'n' veces. Al menos se repite 1 vez
U8		RepeatPos(U8 n)			{
	int		i, j	= Nmove - Rule50;
	U8		count	= 1;

	for (i = Nmove - 2; i >= j; i -= 2)
		if ((Moves[i].Hash == Hash) && (++count >= n))
			return 1;

	return 0;
}


void	QuitPiece(SQUARE sq, U8 upd_hash)		{
	PIECE		piece		= Board[sq];
	TURN		side		= GET_COLOR(piece);


	// Quita la pieza del tablero y los bitboard
	Board[sq]				= EMPTY_PIECE;

	PieceBB[piece]			^= SquareBB[sq];
	AllPieceBB[side]		^= SquareBB[sq];


	// Actualiza las hash
	HashMat					-= HashMatKey[piece];
	if (upd_hash)
		Hash				^= HashPiece[piece][sq];


	// Actualiza datos referente al material
	if (piece < KING)			{
		MatSum				-= PieceValue[piece];

		if (NOT_PAWN(piece))
			PieceMat[side]	-= PieceValue[piece];

		else PawnMat[side]	-= PAWN_VALUE;
	}


	PieceNum[piece]--;											// Actualiza la cantidad de cada pieza
}


void	PutPiece(SQUARE sq, PIECE piece, U8 upd_hash)			{
	TURN		side		= GET_COLOR(piece);


	// Pone la pieza en el tablero y los bitboard
	Board[sq]				= piece;

	PieceBB[piece]			^= SquareBB[sq];
	AllPieceBB[side]		^= SquareBB[sq];


	// Actualiza las hash
	HashMat					+= HashMatKey[piece];
	if (upd_hash)
		Hash				^= HashPiece[piece][sq];


	// Actualiza datos referente al material
	if (piece < KING)			{
		MatSum				+= PieceValue[piece];

		if (NOT_PAWN(piece))
			PieceMat[side]	+= PieceValue[piece];

		else PawnMat[side]	+= PAWN_VALUE;
	}

	// Actualiza la posicion del Rey
	else KingSquare[side]	= sq;


	PieceNum[piece]++;											// Actualiza la cantidad de cada pieza
}


void	MovePiece(SQUARE from, SQUARE to, U8 upd_hash)			{
	PIECE		piece	= Board[from];
	TURN		side	= GET_COLOR(piece);


	// Pone la pieza en el tablero y los bitboard
	Board[to]		= piece;
	Board[from]		= EMPTY_PIECE;


	PieceBB[piece]	^= SquareBB[from] ^ SquareBB[to];
	AllPieceBB[side]^= SquareBB[from] ^ SquareBB[to];


	// Actualiza las hash
	if (upd_hash)
		Hash		^= HashPiece[piece][from] ^ HashPiece[piece][to];


	// Actualiza la posicion del Rey
	if (piece >= KING)
		KingSquare[side] = to;
}


// 400
