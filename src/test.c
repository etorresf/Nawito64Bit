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

#include "test.h"


// Comprueba el generador de movimientos, contando el número de nodos totales para una profundidad
U64		PerftAux(int Depth)		{
	if (!Depth)
		return 1;

	U64			NodesNum	= 0;
	STACK_MOVE	*Last		= FirstMove[Ply + 1] = MoveGen(FirstMove[Ply]),
				*First;

	for (First = FirstMove[Ply]; First != Last; First++)
		if (MakeMove(First->ID))				{
			NodesNum += PerftAux(Depth - 1);
			UnMakeMove();
		 }

	return NodesNum;
}


// Muestra los datos obtenidos por Perft_Aux
void	Perft(int Depth)		{
	int		i, Time;
	U64		NodesNum;

	LoadFEN(PERFT_POS);
	printf("  Performance Test in \"%s\"\n\n  Ply\tSeconds\tNodes\n", PERFT_POS);

	for (i = 0; i <= Depth; i++){
		TimeLeft	= ClockTime();
		NodesNum	= PerftAux(i);
		Time		= (ClockTime() - TimeLeft) / 1000;

		printf("  %i:\t%d\t%" PRIu64 "\n", i, Time, NodesNum);
	}
}


/*
 * Resuelve un mate, sirve para comprobar la velocidad relativa del ordenador respecto a uno
 * determinado, también para comprobar diferentes compilaciones del código con diferentes compiladores
 */
void	Bench(void)				{
	long		t;
	double		npst;

	printf("\nSearching Mate in \"%s\" \n\n", BENCH_POS);
	LoadFEN(BENCH_POS);

	MaxDepth		= 20;
	Noise			= 0;
	NoNewIteration	= NoNewMov = TimeLimit = TimeTotal = INF_TIME;
	EngineState		|= ANALYZING;

	EngineThink();
	t				= ClockTime() - TimeLeft;
	npst			= ((double)Nodes / ((double)t + 1)) * 1000.0;
	EngineState		^= ANALYZING;

	printf("# \n# Time: %d ms\n# Nodes: %d\n# \n", (int)t, Nodes);
	printf("# Best Nodes per second: %d (Score: %.4f)\n", (int)npst, (float)npst / 2188928.0);
	printf("# Score: 1.000 on Intel® Core™ i5-3470 CPU @ 3.20GHz × 4 \n\n");
}


// 64
