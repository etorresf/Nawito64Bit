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


#include "test.h"



// Comprueba el generador de movimientos, contando el número de nodos totales para una profundidad
U64		PerftAux(int depth)		{
	U64			nodes_num	= 0;
	STACK_MOVE*	move;


	if (!depth)
		return 1;


	MoveGen(Turn, MoveStack[Ply], 0);

	for (move = MoveStack[Ply]; move->ID; move++)
		if (MakeMove(move->ID))	{
			nodes_num	+= PerftAux(depth - 1);
			UnMakeMove();
		 }


	return nodes_num;
}


// Muestra los datos obtenidos por Perft_Aux
void	Perft(int depth)		{
	int		i, time;
	U64		nodes_num;


	LoadFEN(PERFT_POS);
	PrintBoard();

	printf("  Performance Test\n  Ply\tSeconds\tNodes\n");
	for (i = 0; i <= depth; i++){
		TimeLeft	= ClockTime();
		nodes_num	= PerftAux(i);
		time		= (ClockTime() - TimeLeft) / 1000;

		printf("  %i:\t%d\t%" PRIu64 "\n", i, time, nodes_num);
	}
}


/*
 * Resuelve un mate, sirve para comprobar la velocidad relativa del ordenador respecto a uno
 * determinado, también para comprobar diferentes compilaciones del código con diferentes compiladores
 */
void	Bench(void)				{
	long		t;
	double		npst;


	printf("\n   Searching Mate in 11... \n");
	LoadFEN(BENCH_POS);
	PrintBoard();


	MaxDepth		= 20;
	Noise			= 0;
	NoNewIteration	= NoNewMov = TimeLimit = TimeTotal = INF_TIME;
	EngineState		|= ANALYZING;


	EngineThink();
	t				= ClockTime() - TimeLeft;
	npst			= ((double)Nodes / ((double)t + 1)) * 1000.0;
	EngineState		^= ANALYZING;


	printf("# \n# Time: %d ms\n# Nodes: %d\n# \n", (int)t, Nodes);
	printf("# Best Nodes per second: %d (Score: %.4f)\n", (int)npst, (float)npst / 2179549.6875);
	printf("# Score: 1.000 on Intel® Core™ i5-3470 CPU @ 3.20GHz × 4 \n\n");
}


// 64
