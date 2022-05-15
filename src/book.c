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

#include "book.h"


//====================================================================================================================================
char			BookName[2048];

static FILE*	BookFile;
static int		BookSize;
int				OutOfBook	= 0;
int				OpenBook	= 0;


//====================================================================================================================================
// Extrae del fichero 'file' un entero de 'l' byte y lo pone en 'r'. Devuelve 0 si no ocurre error
int		IntFromFile(U64* r, FILE* file, int l)	{
	int			i, c;
	*r			= 0;

	for(i = 0; i < l; i++)		{
		c	= fgetc(file);

		if(c == EOF)
			return 1;

		(*r) = ((*r) << 8) + (U64)c;
	}

	return 0;
}


// Guarda al fichero 'file' un entero de 'l' byte
void	IntToFile(FILE* file, U64 r, int l)		{
	int			i, c;

	for(i = 0; i < l; i++)		{
		c = (r >> 8 * (l - i - 1)) & 0xFF;
		fputc(c, file);
	}
}


// Inicializa el libro
void	BookIni()				{
	BookFile	= NULL;
	BookSize	= 0;
}


// Abre el libro
int		BookOpen()				{
	OpenBook	= 0;

	// No existe el libro
	BookFile = fopen(BookName,"rb");
	if (BookFile == NULL)
		return -2;

	// No posible colocarse al final del libro
	if (fseek(BookFile, 0, SEEK_END) == -1)
		return -3;

	// Libro vacio
	BookSize = ftell(BookFile) / 16;
	if (BookSize == -1)
		return -4;

	OpenBook = 1;
	return 0;
}


// Cierra el libro
int		BookClose()				{
	// Libro no abierto
	if (BookFile != NULL)
		return -5;

	// No es posible cerrarlo
	if (fclose(BookFile) == EOF)
		return -6;

	OpenBook = 0;
	return 0;
}


// Leyendo movimiento del libro, se retorna valor negativo en caso de error
int		ReadBookEntry(BOOK_ENTRY* book_entry, int n)			{
	U64		r;

	// Entrada vacia
	if (book_entry == NULL)
		return -7;

	// entrada del libro fuera de los limites de este
	if (n < 0 || n >= BookSize)
		return -8;

	// No existe libro
	if (BookFile == NULL)
		return -9;

	// No es posible colocarse en la entrada de libro
	if (fseek(BookFile, n * 16, SEEK_SET) == -1)
		return -10;


	// No es posible cargar llave del movimiento de libro
	if (IntFromFile(&r, BookFile, 8))
		return -11;
	book_entry->Key		= r;

	// No es posible cargar movimiento de libro
	if (IntFromFile(&r, BookFile, 2))
		return -12;
	book_entry->Move	= (U16)r;

	// No es posible cargar puntuacion del movimiento de libro
	if (IntFromFile(&r, BookFile, 2))
		return -13;
	book_entry->Weight	= (U16)r;

	// No es posible cargar aprendizaje del movimiento de libro
	if (IntFromFile(&r, BookFile, 4))
		return -14;
	book_entry->Learn	= (U32)r;

	return 0;
}


// Buscando si existe una posicion en el libro
int		FindBookPos(U64 key)	{
	BOOK_ENTRY	book_entry;
	int			left	= 0,
				right	= BookSize - 1,
				mid;


	// Libro vacio
	if (right < 0)
		return -15;

	// Busca binaria en los movimientos del libro
	while (left < right)		{
		mid = (left + right) / 2;

		// No hay movimientos para la posicion
		if (mid < left && mid < right)
			return -16;

		ReadBookEntry(&book_entry, mid);

		if (key <= book_entry.Key)
			right = mid;

		else left = mid + 1;
	}

	// No hay movimientos para la posicion
	if (left != right)
		return -17;

	ReadBookEntry(&book_entry, left);
	return (book_entry.Key == key) ? left : -1;
}


// Obtiene la puntuacion para un movimiento de libro
#define		GET_MOVE_SCORE(book_move)	(int)((book_move).Weight)


// Obteniendo movimiento de libro para la posicion con llave 'key'
int		GetBookMove(U64 key)	{
	int			pos,
				first_pos,
				score_sum	= 0;
	BOOK_ENTRY	book_entry;


	// No existe el ibro 
	if (BookFile == NULL)
		return -18;

	// libro vacio
	if (BookSize == 0)
		return -19;


	// Movimiento de libro no encontrado o error al buscarlo
	first_pos	= FindBookPos(key);
	if (first_pos < 0)
		return first_pos;


	// Almacenando suma de pesos del libro para la posicion
	for (pos = first_pos; pos < BookSize; pos++){
		ReadBookEntry(&book_entry, pos);
		if (book_entry.Key != key)
			break;

		score_sum	+= GET_MOVE_SCORE(book_entry);
	}


	// Seleccionando movimiento de libro probabilisticamente de acuerdo al peso de cada movimiento
	if (score_sum > 0)			{
		score_sum = rand() % score_sum + 1;

		for (pos = first_pos; pos < BookSize; pos++)			{
			ReadBookEntry(&book_entry, pos);
			if (book_entry.Key != key)
				break;

			score_sum	-= GET_MOVE_SCORE(book_entry);
			if (score_sum <= 0)
				return book_entry.Move;
		}
	}

	return 0;
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


	buffer += sprintf(buffer, "%c%u%c%u", 'a' + FILE(from), 1 + RANK(from), 'a' + FILE(to), 1 + RANK(to));

	if (promotion)
		buffer += sprintf(buffer, "%c", promotion_char[promotion]);
}


// Consulta al libro de aperturas
MOVE	CheckBook(U64 key)		{
	char	move_str[8];


	// Demasiados movimientos fuera del libro
	if (OutOfBook > 8)
		return EMPTY_MOVE;

	// Chequea si se tiene movimiento de libro para la posicion
	MOVE book_move	= GetBookMove(key);
	if (book_move <= 0)
		return EMPTY_MOVE;

	// Convirtiendo a 'string' y luego al formato interno del motor el movimiento del libro
	PrintBookMove(move_str, book_move);
	book_move = CheckMove(move_str, 0);

	if (book_move == UNKNOWN_MOVE || book_move == ILLEGAL_MOVE)
		return EMPTY_MOVE;

	return book_move;
}


// Carga el nombre del libro a usar y su configuracion
void	LoadBookConfig(char* file)				{
	char	command[256], line[256];
	FILE*	config_file		= fopen(file, "r");


	if (config_file != NULL)	{
		fseek(config_file, 0, SEEK_SET);

		while (fgets(line, 256, config_file))	{
			if (line[0] == '#' || line[0] == ';' || (int)line[0] < 32)
				continue;

			sscanf(line, "%s", command);
			if (!strcmp(command, "Book"))		{
				sscanf(line, "Book = %s", BookName);
				break;
			}
		}

		fclose(config_file);
	}
}


//	300
