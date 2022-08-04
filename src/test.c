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

#include "test.h"


//====================================================================================================================================
// Comprueba el generador de movimientos, contando el número de nodos totales para una profundidad
U64		PerftAux(int depth)		{
	U64			nodes_num	= 0;
	STACK_MOVE*	move;


	if (depth)	{
		MoveGen(Turn, STACK_INDEX(Ply), 0);

		for (move = STACK_INDEX(Ply); move->ID; move++)
			if (MakeMove(move->ID))				{
				nodes_num	+= PerftAux(depth - 1);
				UnMakeMove();
			}

		return nodes_num;
	}

	return 1;
}


// Muestra los datos obtenidos por "PerftAux"
void	Perft(char* perft_pos, int depth)		{
	int		i;
	float	time;
	U64		nodes_num;


	LoadFEN(perft_pos);
	printf("\n  Performance Test(perft) off:\t%s\n\n", perft_pos);


	for (i = 0; i <= depth; i++){
		StartTime	= ClockTime();
		nodes_num	= PerftAux(i);
		time		= (float)(ELAPSED_TIME()) / 1000.0;

		printf("  perft(%i) =\t[%.3f sec]\t[%"PRIu64" nodes]\n", i, time, nodes_num);
	}
}


/*
 * Resuelve un mate, sirve para comprobar la velocidad relativa del ordenador respecto a uno
 * determinado, también para comprobar diferentes compilaciones del código con diferentes compiladores
 */
void	Bench(char* bench_pos, int depth)		{
	long		t;
	double		npst;

	// Es necesario, ya que inicialmente no se configuran las hash
	FreeHash();
	AllocHash(TTsizeMB, ETsizeMB, PTsizeMB, MTsizeMB);

	// Cargando posicion y configurando busqueda
	LoadFEN(bench_pos);
	printf("  Searching on (bench):\t%s\n\n", bench_pos);

	// Resolviendo MATE
	EngineThink(ANALYZING, INF_TIME, DEFAULT_INC_TIME, DEFAULT_MOVE_TO_GO, depth, DEFAULT_MOVE_TIME, 0);

	t			= ELAPSED_TIME();
	npst		= ((double)Nodes / ((double)t + 1)) * 1000.0;
	Engine		^= ANALYZING;


	printf("# \n# Time: %d ms\n# Nodes: %d\n# \n", (int)t, Nodes);
	printf("# Nodes per second: %d (Score: %.4f)\n", (int)npst, npst / 2844607.492225);
	printf("# Score: 1.000 on Intel Core i5-3470 CPU @ 3.20GHz x 4 \n\n");
}


//	100
