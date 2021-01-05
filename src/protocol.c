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

#include "protocol.h"


// Muestra el menu o la ayuda del motor
void	PrintMenu(void)			{
	printf("\n#");
	printf("\n# --perft $1, -p $1\t\t\tTotal nodes account for a given depth $1 from the perft position");
	printf("\n# --bench, -b\t\t\t\tRun the built in benchmark");

	printf("\n# --config, -c\t\t\t\tPrint the engine config");
	printf("\n# --version, -v\t\t\t\tPrint the engine version");
	printf("\n# --arch, -a\t\t\t\tPrint the engine architecture");
	printf("\n# --protocol, -l\t\t\tPrint the engine protocol support");

	printf("\n# --help, -h\t\t\t\tPrint a list of commands");
	printf("\n# --quit, -q\t\t\t\tExit the program");

	printf("\n#");
}


// Protocolo xboard, mas otros commandos
void	Xboard(void)			{
	char		Minutes[4],												// Recibe el número de minutos base para el control de tiempo
				Args[6][64],											// Para el comando setboard
				FEN[256];
	int			n,
				PonderFlag	= 1;
	MOVE		BestMove;


	setbuf(stdout, NULL);												// En windows o linux podemos intentar eliminar el problema de buffering al enviar algo a xboard
	LoadFEN(START_POS);
	NoNewIteration = NoNewMov = TimeLimit = TimeTotal = DEFAULT_TIME;	// Por defecto 8 seg para pensar el motor


	for (;;)	{														// bucle esperando recibir un comando del GUI
		if (Turn == EngineTurn)	{										// Turno del motor
			// Comprobando si existe movimiento de libro
			BestMove = CheckBook(&MyBook, Hash);


			// Efectuar busqueda
			if (!BestMove)		{
				EngineState	|= SEARCHING;
				BestMove	 = EngineThink();
			}


			MakeMove(BestMove);											// Guarda el movimiento en la lista de jugadas

			// Enviando movimiento al GUI
			printf("move "); PrintMove(BestMove); printf("\n");


			PrintResult();												// Si es final de partida imprime el resultado
			EngineState	^= SEARCHING;
			PonderFlag	^= 1;											// Indica que puede ponderar
			continue;
		}

		else if (EngineTurn != EMPTY_TURN)		{
			if ((PonderFlag & 1) && (EngineConfig & MAKE_PONDER))	{	// Pondera si el GUI le envía una señal
				if (OutOfBook)	{
					EngineState	|= PONDERING;
					BestMove	 = EngineThink();
					EngineState	^= PONDERING;
				}

				PonderFlag	^= 1;										// Deja de ponderar
				continue;
			}
		}

		if (!fgets(Line, 256, stdin))
			return;
		if (Line[0] == '\n')
			continue;

		sscanf(Line, "%s", Command);									// Almacena lo que llega y lo guarda en la variable Command

		if (!strcmp(Command, "xboard"));								// Si llega xboard simplemente continua
		else if (!strcmp(Command, "name"));								// Nombre del oponente
		else if (!strcmp(Command, "rating"));							// Rating del oponente cuando estamos en modo ICS
		else if (!strcmp(Command, "ics"));								// Nombre del servidor de Internet donde jugamos
		else if (!strcmp(Command, "computer"));							// El oponente es un motor
		else if (!strcmp(Command, "variant"));							// Nos indica si jugamos una variante que no es la normal
		else if (!strcmp(Command, "random"));							// Comando que en GNUChess añadía un pequeño valor aleatorio a la evaluación
		else if (!strcmp(Command, "otim"));								// Tiempo del oponente, no utilizado de momento
		else if (!strcmp(Command, "edit"));								// Viejo comando edit de winboard para configurar una posición
		else if (!strcmp(Command, "."));
		else if (!strcmp(Command, "exit"));
		else if (!strcmp(Command, "bk"));								// Si el usuario selecciona Book en el menú
		else if (!strcmp(Command, "accepted"));							// Indica si las características definidas son aceptadas
		else if (!strcmp(Command, "draw"));

		else if (!strcmp(Command, "quit"))								// Comando para cerrar el motor
			return;

		else if (!strcmp(Command, "rejected"))
			printf("feature rejected\n");

		else if (!strcmp(Command, "force"))								// El motor deja de jugar
			EngineTurn		= EMPTY_TURN;

		else if (!strcmp(Command, "?"))
			EngineTurn		= EMPTY_TURN;

		else if (!strcmp(Command, "hard"))								// Ponder, pensar con el tiempo del contrario
			EngineConfig	|= MAKE_PONDER;

		else if (!strcmp(Command, "easy"))								// No ponder
			EngineConfig	^= (MAKE_PONDER & EngineConfig);

		else if (!strcmp(Command, "post"))								// Imprimir el pensamiento
			EngineConfig	|= POST;

		else if (!strcmp(Command, "nopost"))							// No imprimir el pensamiento
			EngineConfig	^= (POST & EngineConfig);

		else if (!strcmp(Command,"protover"))							// Comprueba si tiene protocolo winboard 2 y define algunas características
			printf("feature colors=0 draw=0 ics=1 myname=\"%s\" ping=1 setboard=1 sigint=0 sigterm=0 variants=\"normal\"\nfeature done=1\n", ENGINE_NAME);

		else if (!strcmp(Command, "result"))							// Resultado de un juego el motor debe parar
			EngineTurn = EMPTY_TURN;

		else if (!strcmp(Command, "new"))		{						// Comprueba si hay que empezar nueva partida
			LoadFEN(START_POS);
			NoNewIteration = NoNewMov = TimeLimit = TimeTotal = DEFAULT_TIME;	// Por defecto 8 seg para pensar el motor
		}

		else if (!strcmp(Command, "go"))								// Fuerza al motor a jugar
			EngineTurn	= Turn;

		else if (!strcmp(Command, "white"))		{						// Turno para las blancas, el motor juega negras
			Turn		= WHITE;
			EngineTurn	= BLACK;
		}

		else if (!strcmp(Command, "black"))		{						// Turno para las negras, el motor juega blancas
			Turn		= BLACK;
			EngineTurn	= WHITE;
		}

		else if (!strcmp(Command, "level"))		{						// Parámetros para configurar el nivel de juego del programa level 40 5 0 --> 40 movimientos 5 minutos incremento 0 si el número de movimientos es 0 la partida es a finish
			sscanf(Line, "level %d %s %d", &Mps, Minutes, &Inc);
			Inc *= 1000;
		}

		else if (!strcmp(Command, "st"))		{						// Configura el nivel de juego indicando cuantos segundos por movimiento	st 10 --> 10 segundos por jugada
			sscanf(Line, "st %d", &MaxTime);
			NoNewIteration = NoNewMov = TimeLimit = TimeTotal = MaxTime *= 1000;// Convierte a milisegundos y no gestiona el tiempo para cambios de iteración o nuevos movimientos
			MaxDepth = MAX_DEPHT;
		}

		else if (!strcmp(Command, "sd"))		{						// Configura el nivel de juego indicando la profundidad a jugar sd 8 --> juega hasta profundidad principal 8
			sscanf(Line, "sd %d", &n);

			MaxDepth		= n & MAX_DEPHT;
			NoNewIteration	= NoNewMov = TimeLimit = TimeTotal = INF_TIME;	// Es como poner tiempo infinito y no gestionamos el tiempo
		}

		else if (!strcmp(Command, "time"))		{						// Recibe del GUI el tiempo que nos queda
			MaxTime	= DEFAULT_TIME / 10;
			sscanf(Line, "time %d", &MaxTime);
			CalcTimeForMove(10 * MaxTime, Nmove, EngineTurn, OutOfBook, MatSum / 100);	// Pasa a milisegundos que es como trabajamos internamente
		}

		else if (!strcmp(Command, "ping"))		{						// Control para winboard, recibe un ping y envía un pong
			n = 0;
			sscanf(Line, "ping %d", &n);
			printf("pong %d\n", n);
		}

		else if (!strcmp(Command, "setboard"))	{						// Configura el tablero con una posición en formato FEN
			strcpy(FEN, "");
			sscanf(Line, "setboard %s %s %s %s %s %s", Args[0],Args[1],Args[2],Args[3],Args[4],Args[5]);

			strcat(FEN, Args[0]); strcat(FEN, " ");
			strcat(FEN, Args[1]); strcat(FEN, " ");
			strcat(FEN, Args[2]); strcat(FEN, " ");
			strcat(FEN, Args[3]); strcat(FEN, " ");
			strcat(FEN, Args[4]); strcat(FEN, " ");
			strcat(FEN, Args[5]);

			LoadFEN(FEN);
			NoNewIteration = NoNewMov = TimeLimit = TimeTotal = DEFAULT_TIME;
		}

		else if (!strcmp(Command, "hint"))		{						// Da una pista del mejor movimiento al usuario si nos lo pide
			NoNewIteration	 = NoNewMov = TimeLimit = TimeTotal = DEFAULT_TIME;
			MaxDepth		 = MAX_DEPHT;

			EngineState		|= SEARCHING;
			BestMove		 = EngineThink();
			EngineState		^= SEARCHING;


			if (BestMove)
			{	printf("Hint: "); PrintMove(BestMove); printf("\n");	}
		}

		else if (!strcmp(Command, "undo"))		{						// Deshace un movimiento
			if (Nmove > 1)
				UnMakeMove();
		}

		else if (!strcmp(Command, "remove"))	{						// Deshace los 2 últimos movimientos y continua el mismo color
			if (Nmove > 2)		{
				UnMakeMove();
				UnMakeMove();
			}
		}

		else if (!strcmp(Command, "analyze"))	{						// Modo de análisis con GUI
			NoNewIteration	= NoNewMov = TimeLimit = TimeTotal = INF_TIME;
			MaxDepth		= MAX_DEPHT;
			EngineState		|= ANALYZING;
			BestMove		= EngineThink();
			EngineState		^= ANALYZING;
			EngineTurn		= EMPTY_TURN;
		}



		// LOS SIGUIENTES COMANDOS NO PERTENECEN A XBOARD. SON PARA USARSE SOLO CON NAWITO
		else if (!(strcmp(Command, "--perft") && strcmp(Command, "-p")))		{// Perft: comprueba el correcto funcionamiento del generador de movimientos
			n = 5;
			sscanf(Line,"%*s %i", &n);
			Perft(n & MAX_DEPHT);
		}

		else if (!(strcmp(Command, "--bench") && strcmp(Command, "-b")))		// Bench: comprueba la velocidad del equipo
			Bench();

		else if (!(strcmp(Command, "--config") && strcmp(Command, "-c")))	{	// Se muestra la configuración actual
			PrintConfig(EngineConfig);
			PrintBookConfig(MyBook.State);
			printf("\n#\n");
		}

		else if (!(strcmp(Command, "--version") && strcmp(Command, "-v")))
			printf("%s", ENGINE_VERSION);

		else if (!(strcmp(Command, "--protocol") && strcmp(Command, "-l")))
			printf("%s", ENGINE_PROTOCOL);

		else if (!(strcmp(Command, "--arch") && strcmp(Command, "-a")))
			printf("%s", ENGINE_ARCH);

		else if (!(strcmp(Command, "--help") && strcmp(Command, "-h")))			// Ayuda en pantalla
			PrintMenu();

		else if (!(strcmp(Command, "--quit") && strcmp(Command, "-q")))			// Sale del programa
			break;

		else	{
			BestMove = CheckOneMove(Command, &n).ID;

			if (BestMove)		{												// Comprueba si llega un movimiento del usuario y es legal
				MakeMove(BestMove);
				PrintResult();													// Si es final de partida imprime el resultado
			}
			else printf("Error (unknown command): %s\n", Command);
		}
	}
}


// 300
