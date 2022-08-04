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

#include "uci.h"


//====================================================================================================================================
UCI_TYPE	UCIoptions	= ALL_UCI_OPTION;
int			OwnBook		= 0;									// Por defecto el motor no carga un libro de aperturas
char*		SavePointer;


//====================================================================================================================================
// Enviando movimiento al GUI
void	SendMoveUCI(MOVE bestmove)				{
	if (bestmove)				{
		printf("bestmove ");
		PrintMove(bestmove);
		printf("\n");
	}

	else printf("bestmove 0000\n");
}


// Extrae tokens de una cadena
char*	MyStrtok(char* s, const char* sep, char** p)			{
	if (!s && !(s = *p))
		return NULL;

	s += strspn(s, sep);
	if (!*s)
		return *p = 0;
	*p = s + strcspn(s, sep);

	if (**p)
		*(*p)++ = 0;
	else
		*p = 0;

	return s;
}


// Comprueba las opciones recibidas del comando setoption si ha habido algun cambio de la GUI o el usuario
void	ParseSetOptionCommand(char* Line)		{
	unsigned hash_MB		= 0;
	char	*name			= strstr(Line, "name"),
			*value			= strstr(Line, "value");


	if (name == NULL || value == NULL || (name + 2 >= value))
		return;

	name	+= strlen("name ");
	--value;
	*value	= '\0';
	value	+= strlen(" value ");


	// Cambia el tamaño de la hash si el usuario o GUI lo ordena
	if (!strcmp(name, "Hash"))	{
		FreeHash();												// Devolver la memoria usada por las tablas hash
		hash_MB	= MAX(16, 1 << GET_POW(atoi(value)));			// Calcula el tamanho de las tablas hash

		AllocHash(hash_MB >> 1, hash_MB >> 2, hash_MB >> 3, hash_MB >> 4);			// Reserva tamanho de tablas hash
		ClearHash();

		UCIoptions	&= ALL_UCI_OPTION ^ CHANGE_HASH;
	}

	// Borra las tablas hash si el usuario o GUI lo ordena
	else if (!strcmp(name, "Clear Hash"))
		ClearHash();

	// Activa los libros por defecto del motor y los inicia
	else if (!strcmp(name, "OwnBook"))			{
		OwnBook		=  STRING_TO_BOOL(value);
		UCIoptions	|= CHANGE_BOOK;
	}

	// Cambia el libro de torneo, para ello previamente cierra los libros y los inicia de nuevo si el motor gestiona libros propios
	else if(!strcmp(name, "Book File"))			{
		strcpy(BookPath, value);
		UCIoptions	|= CHANGE_BOOK;
	}
}


// Comprueba las opciones del comando position
void	ParsePositionCommand()	{
	char	*Opc		= MyStrtok(NULL, " ", &SavePointer);
	MOVE	best_move	= EMPTY_MOVE;
	int		NumMoves	= -1;

	if (Opc == NULL)
		return;


	/*
	 * UCI envia en cada movimiento al motor el comando postion startpos y luego la lista de jugadas realizadas desde la 1ra. 
	 * Se puede tratar de evitar de realizar en cada movimiento la configuracion inicial y toda la lista de jugadas si sabemos 
	 * que este startpos no corresponde a un nuevo juego ya que en este caso valdria solamente introducir en la lista de 
	 * jugadas el ultimo realizado por el adversario. Por supuesto si se sabe que esta recibiendo un comando ucinewgame o si 
	 * la GUI no envia nunca este comando entonces no es posible evitarlo y se tiene que realizar toda la secuencia.
	 */
	if (!strcmp(Opc, "startpos"))				{
		LoadFEN(START_POS);
		EngineTurn	= BLACK;

		Opc = MyStrtok(NULL, " ", &SavePointer);
		if (Opc == NULL || strcmp(Opc, "moves"))
			return;
	}

	/*
	 * La GUI podría enviar también con el comando fen una posicion que no es la inicial del ajedrez normal y despues 
	 * una lista de movimientos, en este caso en cada movimiento el motor configurara la posicion fen enviada por la GUI
	 */
	else if (!strcmp(Opc, "fen"))				{
		char FEN[1000];
		FEN[0] = '\0';

		while((Opc = MyStrtok(NULL, " ", &SavePointer)) != NULL){
			if (!strcmp(Opc, "moves"))
				break;

			strcat(FEN, Opc);
			strcat(FEN, " ");
		}

		LoadFEN(FEN);
	}

	else return;												// si despues del comando position no recibimos el comamndo startpos o fen anulamos el comando


	// Ahora tenemos que gestionar la lista de movimientos recibidos después de las opciones startpos o fen.
	while((Opc = MyStrtok(NULL, " ", &SavePointer)) != NULL)
		if (Nmove <= ++NumMoves)
			switch ((best_move = CheckMove(Opc, 1)))			{
				case ILLEGAL_MOVE:	printf("Illegal move: %s\n", Opc);				break;
				case UNKNOWN_MOVE:	printf("Error (unknown command): %s\n", Opc);	break;
				default:;
			}
}


// Comprobamos las opciones del comando Go
void	ParseGoCommand()		{
	char*	Opc;												// guarda el comando recibido despues de go
	MOVE	best_move		= EMPTY_MOVE;
	int		time[3]			= {DEFAULT_TIME,	  DEFAULT_TIME,		 DEFAULT_TIME,},
			inc[3]			= {DEFAULT_INC_TIME,  DEFAULT_INC_TIME,	 DEFAULT_INC_TIME,},
			max_depth[3]	= {MAX_DEPHT,		  MAX_DEPHT,		 MAX_DEPHT,},
			move_time[3]	= {DEFAULT_MOVE_TIME, DEFAULT_MOVE_TIME, DEFAULT_MOVE_TIME,},
			moves_to_go[3]	= {DEFAULT_MOVE_TO_GO,DEFAULT_MOVE_TO_GO,DEFAULT_MOVE_TO_GO,};


	while((Opc = MyStrtok(NULL, " ", &SavePointer)) != NULL)	{
		// modo tiempo infinito o modo análisis, no se hace diferencia entre ambos
		if (!strcmp(Opc, "infinite"))			{
			best_move		= EngineThink(ANALYZING, INF_TIME, DEFAULT_INC_TIME, DEFAULT_MOVE_TO_GO, MAX_DEPHT, DEFAULT_MOVE_TIME, OutOfBook);
			SendMoveUCI(best_move);
			EngineTurn		= EMPTY_TURN;
			return;
		}

		// opciones de tiempo e incremento para blancas y negras
		if (!strcmp(Opc, "wtime"))
			time[WHITE]		= atoi(MyStrtok(NULL, " ", &SavePointer));

		else if (!strcmp(Opc, "btime"))
			time[BLACK]		= atoi(MyStrtok(NULL, " ", &SavePointer));

		else if (!strcmp(Opc, "winc"))
			inc[WHITE]		= atoi(MyStrtok(NULL, " ", &SavePointer));

		else if (!strcmp(Opc, "binc"))
			inc[BLACK]		= atoi(MyStrtok(NULL, " ", &SavePointer));

		// numero de movimentos que faltan para el control de tiempo
		else if (!strcmp(Opc, "movestogo"))
			moves_to_go[Turn]	= atoi(MyStrtok(NULL, " ", &SavePointer));

		// modo profundidad fija
		else if (!strcmp(Opc, "depth"))				{
			max_depth[Turn]	= atoi(MyStrtok(NULL, " ", &SavePointer));
			max_depth[Turn]	= MIN(MAX_DEPHT, max_depth[Turn]);
			time[Turn]		= INF_TIME;
		}

		// modo tiempo fijo por movimiento
		else if (!strcmp(Opc, "movetime"))			{
			move_time[Turn]	= atoi(MyStrtok(NULL, " ", &SavePointer));
			max_depth[Turn]	= MAX_DEPHT;
		}
	}


	// si hacemos una búsqueda con las opciones de tiempo del comando go
	best_move	= (OwnBook && OpenBook) ? CheckBook(Hash) : EMPTY_MOVE;
	OutOfBook	= best_move ? 0 : (OutOfBook + 1);

	// Indica que se tiene un movimiento de libro
	if (best_move)
		printf("info string BOOK MOVE\n");

	// Calcula tiempo para pensar si es necesario y busca el mejor movimiento
	else	best_move	= EngineThink(SEARCHING, time[Turn], inc[Turn], moves_to_go[Turn], max_depth[Turn], move_time[Turn], OutOfBook);

	// Envia y realiza el movimiento
	SendMoveUCI(best_move);
	MakeMove(best_move);
	EngineTurn	= EMPTY_TURN;
}


// si el motor juega con el protocolo uci entramos en esta funcion
void	UCI()	{
	char	line[10000];										// definimos el tamaño de la linea que vamos a recibir para los comandos que envía la GUI

	setbuf(stdout, NULL);										// En windows, linux o Android podemos intentar eliminar el problema de buffering


	// Identificamos el nombre del motor, autor y si se carga informacion de algun agente
	printf("\n");
	printf("id name %s\n",   ENGINE_NAME);
	printf("id author %s\n", AUTOR_NAME);

	// envía a la GUI las opciones que el motor maneja
	printf("option name OwnBook type check default false\n");
	printf("option name Book File type string default %s\n", BookPath);

	printf("option name Hash type spin default 256 min 16 max 2048\n");
	printf("option name Clear Hash type button\n");

	printf("option name UCI_AnalyseMode type check default false\n");

	printf("uciok\n");


	// ahora entramos en el ciclo en el que el motor comprueba los comando que llegan de la GUI
	while (fgets(line, 10000, stdin))			{
		// Remueve el caracter de nueva linea
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = '\0';

		// Comprueba el comando recibido
		char*	command	= MyStrtok(line, " ", &SavePointer);
		if (command == NULL)
			return;

		if (!strcmp(command, "uci"));

		else if (!strcmp(command, "go"))
			ParseGoCommand();

		else if (!strcmp(command, "setoption"))
			ParseSetOptionCommand(line + strlen(command) + 1);

		else if (!strcmp(command, "isready"))	{
			if (UCIoptions & CHANGE_HASH)		{
				FreeHash();
				AllocHash(TTsizeMB, ETsizeMB, PTsizeMB, MTsizeMB);
				ClearHash();
				UCIoptions	^= CHANGE_HASH;
			}

			if (UCIoptions & CHANGE_BOOK)		{
				if (OpenBook)
					BookClose();

				if (OwnBook)	{
					BookIni();
					BookOpen();
				}

				UCIoptions	^= CHANGE_BOOK;
			}

			printf("readyok\n");
		}

		else if (!strcmp(command, "quit"))		{
			printf("info Bye\n");
			exit(0);
		}

		else if (!strcmp(command, "position"))
			ParsePositionCommand();
	}
}


//	300
