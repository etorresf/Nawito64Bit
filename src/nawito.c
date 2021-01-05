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


// Inicializa algunos datos del motor
void	Init(void)				{
	srand((unsigned)ClockTime());
	MyBook.State	= 0;

	FindProfile(ProfileName);
	CONFIG Profile	= LoadProfile(ProfileName);

	MyBook.State |= USE;
	CompleteProfile(Profile);

	AllocHash();
	BitboardInitBB();

	DistanceIni();
	IniLMRtable();
	EvalIni();

	if (LoadBook(&MyBook, BookName))
		MyBook.State ^= USE;


	PrintConfig(Profile);
	PrintBookConfig(Profile);
}


// Se devuelve la memoria utilizada por el motor
void	Finalize(void)			{
	CloseBook(&MyBook);
	FreeHash();
}


int		main()		{
	printf("#\n# %s is a chess engine with %s protocol.", ENGINE_NAME, ENGINE_PROTOCOL);
	printf("\n# Copyright (C) <%s> by <%s>", ENGINE_DATE, AUTOR_NAME);
	Init();
	printf("\n#\n#\n# \"--help\" o \"-h\" print a list of commands.\n#\n");

	Xboard();
	Finalize();

	return 0;
}


// 64
