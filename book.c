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

#include "book.h"


int					OutOfBook;
BOOK				MyBook;

#define				LAST_MOVE				0x8000


// Extrae del fichero 'File' un entero de 'l' byte y lo pone en 'r'. Devuelve 0 si no ocurre error
int		IntFromFile(U64* r, FILE* File, int l)	{
	int			i, c;
	*r			= 0;

	for(i = 0; i < l; i++)		{
		c	= fgetc(File);

		if(c == EOF)
			return 1;

		(*r) = ((*r) << 8) + (unsigned long)c;
	}

	return 0;
}


// Extrae del fichero 'File' un movimiento de libro 'BookMove'. Devuelve 0 si no ocurre error
int		BookMoveFromFile(BOOK_MOVE* BookMove, FILE* File, unsigned* LastMove)	{
	U64			r;


	// Movimiento
	if (IntFromFile(&r, File, 2))
		return 1;
	BookMove->Move = r & 0x7FFF;


	// Determina si es el ultimo movimiento para esta posición del libro
	if (r & LAST_MOVE)
		*LastMove = 1;

	else *LastMove = 0;


	// Puntuacion polyglot para el movimiento
	if (IntFromFile(&r, File, 2))
		return 2;
	BookMove->Weight = (U16)r;


	if (IntFromFile(&r, File, 4))
		return 4;
	if (IntFromFile(&r, File, 2))
		return 5;

	return 0;
}


// Extrae del fichero 'File' los movimientos para la posicion del libro 'Position'. Devuelve 0 si no ocurre error
int		BookMoveListFromFile(BOOK_POS* Position, FILE* File)	{
	int			i,
				Size		= 0;
	U64			r			= 0;
	unsigned	LastMove	= 0;
	BOOK_MOVE	BookMove	= {0,0,},
				MovList[256];


	if (IntFromFile(&r, File, 8))
		return 1;
	Position->Key = r;


	for (; !LastMove;)			{
		if (BookMoveFromFile(&BookMove, File, &LastMove))
			return 2;

		MovList[Size++] = BookMove;
	}


	Position->Moves		= (U8)Size;
	Position->MoveList	= (BOOK_MOVE*) malloc(sizeof(BOOK_MOVE) * (unsigned)Size);


	for (i = 0; i < Size; i++)
		Position->MoveList[i] = MovList[i];

	return 0;
}


// Carga un libro. Devuelve 0 si no ocurre error
int		LoadBook(BOOK* Book, char* Name)		{
	strcpy(Book->Name, Name);
	FILE*		File	= fopen(Name, "rb");
	U32			i		= 0;
	U64			r;


	if (!File)
		return 1;

	if (IntFromFile(&r, File, 4))				{
		fclose(File);
		return 2;
	}
	Book->NumKey = (unsigned)r;


	Book->Position	 = (BOOK_POS*) malloc(sizeof(BOOK_POS) * (unsigned)Book->NumKey);

	for (i = 0; i < Book->NumKey; i++)
		if (BookMoveListFromFile(&(Book->Position[i]), File))
			break;


	fclose(File);


	if (i != Book->NumKey)
		return 3;

	return 0;
}


// Guarda al fichero 'File' un entero de 'l' byte
void	IntToFile(FILE* File, U64 r, int l)		{
	int			i, c;

	for(i = 0; i < l; i++)		{
		c = (r >> 8 * (l - i - 1)) & 0xFF;
		fputc(c, File);
	}
}


// Busca una posicion en el libro con llave 'HashKey'
BOOK_POS*		BinarySearch(BOOK_POS* Position, U64 HashKey, int Inf, int Sup)	{
	int			Midd;

	while(Inf <= Sup)			{
		Midd = ((Sup - Inf) >> 1) + Inf;

		if(HashKey == Position[Midd].Key)
			return &(Position[Midd]);

		if(HashKey < Position[Midd].Key)
			Sup = Midd - 1;

		else Inf = Midd + 1;
	}

	return NULL;
}


// Precalcula algunos datos para hacer mas rápida la búsqueda binaria
BOOK_POS*		ExpSearch(BOOK* Book, U64 HashKey)				{
	if (!Book->NumKey)
		return NULL;

	U32 Bound = 1;
	while (Bound < Book->NumKey && Book->Position[Bound].Key < HashKey)
		Bound <<= 1;

	return BinarySearch(Book->Position, HashKey, (int)Bound / 2, (int)MIN(Bound + 1, Book->NumKey));
}


// Cierra el libro
void	CloseBook(BOOK* Book)	{
	U32			KeyInd;

	if (Book->State & USE)		{
		for (KeyInd = 0; KeyInd < Book->NumKey; KeyInd++)
			free(Book->Position[KeyInd].MoveList);

		free(Book->Position);
	}
}


// Suma el valor de los movimientos de libro de la posición 'Position'
int		GetScoreSum(BOOK_POS* Position)			{
	U8		i;
	int		ScoreSum	= 0;

	for (i = 0; i < Position->Moves; i++)
		ScoreSum += (int)(Position->MoveList[i].Weight);

	return ScoreSum;
}


// Selecciona psuedo-aleatoriamente un movimiento de libro de la posición 'Position' según su valor 
BOOK_MOVE*		SelectBookMove(BOOK_POS* Position, int ScoreSum){
	if (!ScoreSum )
		return NULL;

	int			i, j	= rand() % ScoreSum + 1;
	BOOK_MOVE*	BMove	= Position->MoveList;

	for (i = 0, j -= (int)(BMove->Weight); (j > 0) && (i < Position->Moves); j -= (int)(BMove->Weight), i++)
		BMove++;

	return BMove;
}


// Determina cual movimiento del libro realizar
BOOK_MOVE*		GetBookMove(BOOK* Book, U64 MoveKey)			{
	if (Book->State & USE)		{
		BOOK_POS* Position = ExpSearch(Book, MoveKey);

		if (Position)
			return SelectBookMove(Position, (int)GetScoreSum(Position));
	}

	return NULL;
}


// Convierte a 'string' un movimiento del libro
void	PrintBookMove(char* Buffer, MOVE IDmove)				{
	char*		PromoteChar = " nbrq   ";
	SQUARE		From		= (IDmove >> 6) & 0x3F,
				To			= (IDmove & 0x3F);
	int			Promote		= (IDmove >> 12) & 0x7;


	// Ajusta la casilla destino del rey en caso de enroque. En formato 'polyglot' estas casillas son A1, H1, A8, H8
	if ((From == E8 && MATCH(BLACK_KING, From)) || (From == E1 && MATCH(WHITE_KING, From)))
		switch (To)			{
			case H1: case H8:	To--;		break;
			case A1: case A8:	To += 2;	break;
			default:;
		}


	Buffer += sprintf(Buffer, "%s%s", SquareChar[From], SquareChar[To]);

	if (Promote)
		Buffer += sprintf(Buffer, "%c", PromoteChar[Promote]);
}


// Consulta al libro de aperturas
MOVE	CheckBook(BOOK* Book, U64 Key)			{
	int			i;
	char		MoveStr[8];
	MOVE		Move;
	BOOK_MOVE*	BookMove	= GetBookMove(Book, Key);


	if (BookMove)				{
		PrintBookMove(MoveStr, BookMove->Move);
		Move = CheckOneMove(MoveStr, &i).ID;

		printf("%d %d %d %d %s {Book Move}\n", 0, 0, 0, BookMove->Weight, MoveStr);

		OutOfBook = 0;
		return Move;
	}


	OutOfBook++;
	return 0;
}


// Convierte a movimiento de libro otro en formato 'string'
MOVE	ScanBookMove(char* c)	{
	int			From	= (c[0] - 'a') + ((c[1] - '1') << 3),
				To		= (c[2] - 'a') + ((c[3] - '1') << 3);
	MOVE		Move	= To + (From << 6);


	// Ajusta la casilla destino del rey en caso de enroque. En formato 'polyglot' estas casillas son A1, H1, A8, H8
	if ((From == E8 && MATCH(BLACK_KING, From)) || (From == E1 && MATCH(WHITE_KING, From)))
		switch (To)			{
			case G1: case G8:	To++;		break;
			case C1: case C8:	To -= 2;	break;
			default:;
		}


	// Agrega el tipo de promoción al movimiento, si esta existe
	switch (c[4])				{
		case 'n':	return Move | (1 << 12);
		case 'b':	return Move | (2 << 12);
		case 'r':	return Move | (3 << 12);
		case 'q':	return Move | (4 << 12);
		default:	return Move;
	}
}


void	PrintBookConfig(CONFIG Config)			{
	printf("\n#\n# Book\t\t= ");
	MyBook.State & USE ? printf("%s", MyBook.Name) : printf(" ");

	if (Config & BOOK1)
		printf("\t\t[Not found. Using default value]");
}


// 500
