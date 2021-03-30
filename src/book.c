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


#include "book.h"



int					OutOfBook;
int					MaxNumMove	= 64;


BOOK				MyBook;


// Extrae del fichero 'File' un entero de 'l' byte y lo pone en 'r'. Devuelve 0 si no ocurre error
int		IntFromFile(U64* r, FILE* file, int l)	{
	int			i, c;
	*r			= 0;

	for(i = 0; i < l; i++)		{
		c	= fgetc(file);

		if(c == EOF)
			return 1;

		(*r) = ((*r) << 8) + (unsigned long)c;
	}

	return 0;
}


// Extrae del fichero 'File' un movimiento de libro 'BookMove'. Devuelve 0 si no ocurre error
int		BookMoveFromFile(BOOK_MOVE* move, FILE* file, unsigned* last_move)		{
	U64			r;


	// Movimiento
	if (IntFromFile(&r, file, 2))
		return 1;
	move->Move = r & 0x7FFF;

	*last_move = (r & LAST_MOVE) != 0;							// Ultimo movimiento para esta posición del libro


	// Puntuacion polyglot para el movimiento
	if (IntFromFile(&r, file, 2))
		return 2;
	move->Weight = (U16)r;


	// Valor del aprendizaje del motor
	if (IntFromFile(&r, file, 2))
		return 5;

	return 0;
}


// Extrae del fichero 'File' los movimientos para la posicion del libro 'Position'. Devuelve 0 si no ocurre error
int		BookMoveListFromFile(BOOK_POS* position, FILE* file)	{
	int			i,
				size		= 0;
	U64			r			= 0;
	unsigned	last_move	= 0;
	BOOK_MOVE	book_move	= {0,0,},
				move_list[256];


	if (IntFromFile(&r, file, 8))
		return 1;
	position->Key = r;


	for (; !last_move;)			{
		if (BookMoveFromFile(&book_move, file, &last_move))
			return 2;

		move_list[size++] = book_move;
	}


	position->Moves		= (U8)size;
	position->MoveList	= (BOOK_MOVE*) malloc(sizeof(BOOK_MOVE) * (unsigned)size);


	for (i = 0; i < size; i++)
		position->MoveList[i] = move_list[i];

	return 0;
}


// Carga un libro. Devuelve 0 si no ocurre error
int		LoadBook(BOOK* book, char* name)		{
	strcpy(book->Name, name);
	FILE*		file	= fopen(name, "rb");
	I64			i		= 0;
	U64			r;


	if (!file)
		return 1;

	if (IntFromFile(&r, file, 8))				{
		fclose(file);
		return 2;
	}
	book->NumKey = (I64)r;


	book->Position	 = (BOOK_POS*) malloc(sizeof(BOOK_POS) * (I64)(book->NumKey));

	for (i = 0; i < book->NumKey; i++)
		if (BookMoveListFromFile(&(book->Position[i]), file))
			break;


	fclose(file);


	if (i != book->NumKey)
		return 3;

	return 0;
}


// Guarda al fichero 'File' un entero de 'l' byte
void	IntToFile(FILE* file, U64 r, int l)		{
	int			i, c;

	for(i = 0; i < l; i++)		{
		c = (r >> 8 * (l - i - 1)) & 0xFF;
		fputc(c, file);
	}
}


// Busca una posicion en el libro con llave 'HashKey'
BOOK_POS*		BinarySearch(BOOK_POS* position, U64 hash_key, I64 inf, I64 sup){
	I64			midd;

	while(inf <= sup)			{
		midd		= ((sup - inf) >> 1) + inf;

		if(hash_key == position[midd].Key)
			return &(position[midd]);

		if(hash_key < position[midd].Key)
			sup		= midd - 1;

		else inf	= midd + 1;
	}

	return NULL;
}


// Precalcula algunos datos para hacer mas rápida la búsqueda binaria
BOOK_POS*		ExpSearch(BOOK* book, U64 hash_key)				{
	I64		bound	= 1;


	if (!book->NumKey)
		return NULL;

	while (bound < book->NumKey && book->Position[bound].Key < hash_key)
		bound <<= 1;

	return BinarySearch(book->Position, hash_key, (int)bound / 2, (int)MIN(bound + 1, book->NumKey));
}


// Cierra el libro
void	CloseBook(BOOK* book)	{
	I64			key_ind;

	if (book->State & USE_BOOK)	{
		for (key_ind = 0; key_ind < book->NumKey; key_ind++)
			free(book->Position[key_ind].MoveList);

		free(book->Position);
	}
}


int		GetMoveScore(BOOK_MOVE* move)			{	return (int)(move->Weight);	}


// Suma el valor de los movimientos de libro de la posición 'Position'
int		GetScoreSum(BOOK_POS* position)			{
	U8			i;
	int			score_sum	= 0;


	for (i = 0; i < position->Moves; i++)
		score_sum += GetMoveScore(&(position->MoveList[i]));

	return score_sum;
}


// Selecciona psuedo-aleatoriamente un movimiento de libro de la posición 'Position' según su valor 
BOOK_MOVE*		SelectBookMove(BOOK_POS* position, int score_sum)				{
	int			i, j;
	BOOK_MOVE*	book_move;


	if (!score_sum)
		return NULL;

	j			= rand() % score_sum + 1;
	book_move	= position->MoveList;

	for (i = 0, j -= GetMoveScore(book_move); (j > 0) && (i < position->Moves); j -= GetMoveScore(book_move), i++)
		book_move++;

	return book_move;
}


// Determina cual movimiento del libro realizar
BOOK_MOVE*		GetBookMove(BOOK* book, U64 move_key)			{
	BOOK_POS*	position;


	if (book->State & USE_BOOK)		{
		position	= ExpSearch(book, move_key);

		if (position)
			return SelectBookMove(position, GetScoreSum(position));
	}

	return NULL;
}


// Convierte a 'string' un movimiento del libro
void	PrintBookMove(char* buffer, MOVE move)	{
	char*		promotion_char	= " nbrq   ";
	SQUARE		from			= (move >> 6) & 0x3F,
				to				= (move & 0x3F);
	int			promotion		= (move >> 12) & 0x7;


	// Ajusta la casilla destino del rey en caso de enroque. En formato 'polyglot' estas casillas son A1, H1, A8, H8
	if (Board[from] >= KING)
		switch (to)			{
			case H1: case H8:	to--;		break;
			case A1: case A8:	to += 2;	break;
			default:;
		}


	buffer += sprintf(buffer, "%s%s", SquareChar[from], SquareChar[to]);

	if (promotion)
		buffer += sprintf(buffer, "%c", promotion_char[promotion]);
}


// Consulta al libro de aperturas
MOVE	CheckBook(BOOK* book, U64 key, int num_move)			{
	int			i;
	char		move_str[8];
	MOVE		move		= EMPTY_MOVE;
	BOOK_MOVE*	book_move	= GetBookMove(book, key);


	if (num_move > MaxNumMove)
		return EMPTY_MOVE;

	if (book_move)				{
		OutOfBook = 0;

		PrintBookMove(move_str, book_move->Move);
		move	= CheckMove(move_str, &i);

		printf("%d %d %d %d %s {%s}\n", 0, 0, 0, 0, move_str, book->Name);
	}
	else OutOfBook++;

	return move;
}


// 400
