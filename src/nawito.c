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
// Inicializa algunos datos del motor
void	Init()	{
	// Inicializando variables globales al programa
	srand((unsigned)ClockTime());
	BitboardInitBB();
	DistanceIni();
	IniSearch();
	EvalIni();

	// Cargando configuracion de tablas hash desde fichero
	HASH_CONFIG hash_config	= LoadHashConfig(ENGINE_CONFIG_FILE, HASH | EHASH | PHASH | MHASH);
	CompleteHashConfig(hash_config);

	// Cargando configuracion del libro desde fichero
	LoadBookConfig(ENGINE_CONFIG_FILE);

	// Cargando parametros de evaluacion
	LoadEvalConfig(ENGINE_CONFIG_EV);
}


// Se devuelve la memoria utilizada por el motor
void	Finalize()				{
	BookClose();
	FreeHash();
}


// Muestra el menu o la ayuda del motor
void	PrintHelp()				{
	printf("\n#");
	printf("\n# perft $1\t\t\tTotal nodes account for a given depth $1 from the perft position");
	printf("\n# bench\t\t\t\tRun the built in benchmark");

	printf("\n# version\t\t\tPrint the engine version");
	printf("\n# arch\t\t\t\tPrint the engine architecture");
	printf("\n# protocol\t\t\tPrint the engine protocol support");
	printf("\n# uci\t\t\tSwitch to uci protocol");

	printf("\n# help\t\t\t\tPrint a list of commands");
	printf("\n# quit\t\t\t\tExit the program");
	printf("\n#\n");
}


// Funcion principal del programa
int		main()					{
	char	command[256],
			Line[256];

	Init();
	setbuf(stdout, NULL);										// En windows o linux podemos intentar eliminar el problema de buffering al enviar algo a xboard


	for (; fgets(Line, 256, stdin);)			{				// bucle esperando recibir un comando del GUI
		if (Line[0] == '\n')
			continue;

		sscanf(Line, "%s", command);							// Almacena lo que llega y lo guarda en la variable Command


		// Comandos relacionadas con el paso a otros protocolos de comunicacion
		if (!strcmp(command, "quit"))							// Comando para cerrar el motor
			break;

		if (!strcmp(command, "uci"))			{				// Si llega uci continua a ese protocolo de comunicacion
			UCI();
			break;
		}
		else if (!strcmp(command, "version"))
			printf("%s", ENGINE_VERSION);

		else if (!strcmp(command, "protocol"))
			printf("%s", ENGINE_PROTOCOL);

		else if (!strcmp(command, "arch"))
			printf("%s", ENGINE_ARCH);

		else if (!strcmp(command, "help"))
			PrintHelp();


		// Comandos relacionados con las pruebas y utilidades
		else if (!strcmp(command, "perft"))		{				// Perft: comprueba el correcto funcionamiento del generador de movimientos
			int i = 5;
			sscanf(Line,"%*s %i", &i);
			Perft(PERFT_POS, i % (MAX_DEPHT + 1));
		}
		else if (!strcmp(command, "bench"))						// Bench: comprueba la velocidad del equipo
			Bench(BENCH_POS, 20);
	}


	Finalize();
	return 0;
}


//	100
