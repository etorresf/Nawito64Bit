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

#include "hash.h"


TTYPE			*TThash;
ETYPE			*EThash;

unsigned		TTmask;
unsigned		ETmask;

unsigned		TTsize;
unsigned		ETsize;


// Borra las tablas hash y evaluación caché
void	ClearHash(void)			{
	memset(TThash,	0, sizeof(TTYPE) * (TTmask + 1));
	memset(EThash,	0, sizeof(ETYPE) * (ETmask + 1));
}


// Libera memoria de las tablas hash y de la evaluación caché
void	FreeHash(void)			{
	free(TThash);
	free(EThash);
}


// Asigna memoria para las tablas hash y evaluación caché
void	AllocHash(void)			{
	TThash		= (TTYPE*) malloc(sizeof(TTYPE) * (TTmask + 1));
	EThash		= (ETYPE*) malloc(sizeof(ETYPE) * (ETmask + 1));


	if (TThash == NULL)
		printf("# Out of memory allocating hash.\n");

	if (EThash == NULL)
		printf("# Out of memory allocating eval cache.\n");


	if (TThash == NULL || EThash == NULL)
		exit(1);
}


// Calcula la llave hash para la actual posición
HASH_KEY	PutHash(int* Board64, TURN Side, CASTTLE_RIGHT CasttleRight, SQUARE EnPassantSQ)	{
	ClearHash();

	SQUARE		SQ, From;
	HASH_KEY	Key			= HashTurn[Side] ^ HashCasttle[CasttleRight];


	// Calcula la llave hash para las piezas
	SQ_LOOP(SQ)
		Key ^= HashPiece[Board64[SQ]][SQ];


	// Se pone la hash para la captura al paso solo si un Peón ha movido 2 pasos y puede ser capturado
	if (EnPassantSQ)			{
		From = EnPassantSQ + Front[FLIP_TURN(Side)] + 1;
		if (abs(FILE(EnPassantSQ) - FILE(From)) < 2	&& Board64[From] == (PAWN | Side))
			return Key ^ HashEnPassant[EnPassantSQ];


		From = EnPassantSQ + Front[FLIP_TURN(Side)] - 1;
		if (abs(FILE(EnPassantSQ) - FILE(From)) < 2	&& Board64[From] == (PAWN | Side))
			return Key ^ HashEnPassant[EnPassantSQ];
	}


	return Key;
}


// Almacena información sobre la evaluación de la posición
void	StoreScore(int Score, HASH_KEY Key)		{
	ETYPE*		ETindex	= &EThash[Key & ETmask];

	ETindex->Hash	= Key;
	ETindex->Score	= (I16)Score;
}


// Comprueba si una posición ya se ha evaluado
int		ProbeScore(HASH_KEY Key){
	ETYPE*		ETindex		= &EThash[Key & ETmask];

	if (ETindex->Hash == Key)
		return ETindex->Score;

	return -VAL_UNKNOWN;
}


void	StoreHash(int Move, int Score, int Depth, U8 Flags, HASH_KEY Key, U8 CurrentAge)		{
	BUCKET		*TTindex,
				*TTindex2	= TThash[Key & TTmask].Bucket;
	int			Age_i		= -1,
				Hash_i		= -1,
				Depth_i		= -1,
				Depthdest	= -1,
				i;


	for (i = 0; i < NUM_BUCKETS - 1; i++)		{
		// Buscando llaves hash a la actual, estas no deben estar repetidas
		if (TTindex2[i].Hash == Key)			{
			Hash_i		= i;
			break;
		}

		// Buscando entrada vieja(al menos 8 movimientos de diferencia con el actual)
		if ((TTindex2[i].Age >> 2) ^ (CurrentAge >> 2))
			Age_i		= i;

		// Buscando entrada de menor profundidad
		if (Age_i == -1 && Depthdest < TTindex2[i].Depth)		{
			Depthdest	= TTindex2[i].Depth;
			Depth_i		= i;
		}
	}


	if (Hash_i != -1)	{
		if (Depth >= TTindex2[Hash_i].Depth)
			TTindex		= &TTindex2[Hash_i];					// Remplazo por llave hash, tener las entradas no repetidas es bueno

		else return;											// No aporta informacion
	}

	else if (Age_i != -1)
		TTindex		= &TTindex2[Age_i];							// Remplazo por edad, entradas recientes son buenas

	else if ((Depth_i != -1) && (Depth >= Depthdest))
		TTindex		= &TTindex2[Depth_i];						// Remplazo por profundidad, entradas con altas profundidad son buenas

	else TTindex	= &TTindex2[NUM_BUCKETS - 1];				// Remplazo siempre, entradas cerca de la raiz del arbol de busqueda son buenas


	// Actualiza los valore de la tabla hash
	TTindex->Hash	= Key;
	TTindex->Move	= (U16)Move;
	TTindex->Depth	= (I16)Depth;
	TTindex->Flags	= Flags;
	TTindex->Score	= (I16)Score;
	TTindex->Age	= CurrentAge;
}


// Prueba si la posición esta almacenada en las tablas hash
void	ProbeHash(int* Move, int Depth, U8* Flags, HASH_KEY Key, int* HashDepth, int* HashScore, U8 CurrentAge)	{
	int			i;
	BUCKET*		TTindex;

	*Flags		= HASF_NULL;
	*Move		= 0;


	for (i = 0; i < NUM_BUCKETS; i++)			{
		TTindex	= &TThash[Key & TTmask].Bucket[i];

		if (TTindex->Hash == Key)				{
			*Flags = HASF_HIT;

			if (TTindex->Age ^ CurrentAge)
				TTindex->Age = CurrentAge;						// Actualiza la edad del movimiento, es accesible desde la actual posicion

			if (TTindex->Move)
				*Move	= (int)TTindex->Move;


			if (TTindex->Depth >= Depth)		{
				*Flags		= TTindex->Flags | HASF_HIT | HASF_PRUNING;
				*HashDepth	= (int)TTindex->Depth;
				*HashScore	= (int)TTindex->Score;
				return;
			}
		}
	}
}


// Obtiene la potencia de 2 mas cercana a un numero 'Number' entre 'Inf' y 'Sup'
int		GetPow(int Number, int Inf, int Sup)		{
	int			Midd;


	// Básicamente se realiza una búsqueda binaria
	while(Inf <= Sup)			{
		Midd = ((Sup - Inf) >> 1) + Inf;

		if(Number == (1 << Midd))
			return Midd;

		if(Number < (1 << Midd))
			Sup = Midd - 1;

		else Inf = Midd + 1;
	}


	// Comprueba si la potencia de de anterior es mas cercana
	if (abs(Number - (1 << Inf)) < abs(Number - (1 << Sup)))
		return Inf;

	return Sup;
}


// 200
