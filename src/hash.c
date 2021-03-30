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


#include "hash.h"



TTYPE			*TThash;
ETYPE			*EThash;
MTYPE			*MThash;

unsigned		TTmask;
unsigned		ETmask;
unsigned		MTmask;

unsigned		TTsize;
unsigned		ETsize;
unsigned		MTsize;


// Borra las tablas hash y evaluación caché
void	ClearHash(void)			{
	memset(TThash,	0, sizeof(TTYPE) * (TTmask + 1));
	memset(EThash,	0, sizeof(ETYPE) * (ETmask + 1));
	memset(MThash,	0, sizeof(MTYPE) * (MTmask + 1));
}


// Libera memoria de las tablas hash y de la evaluación caché
void	FreeHash(void)			{
	free(TThash);
	free(EThash);
	free(MThash);
}


// Asigna memoria para las tablas hash y evaluación caché
void	AllocHash(void)			{
	TThash		= (TTYPE*) malloc(sizeof(TTYPE) * (TTmask + 1));
	EThash		= (ETYPE*) malloc(sizeof(ETYPE) * (ETmask + 1));
	MThash		= (MTYPE*) malloc(sizeof(MTYPE) * (MTmask + 1));


	if (TThash == NULL)
		printf("# Out of memory allocating hash.\n");

	if (EThash == NULL)
		printf("# Out of memory allocating eval cache.\n");

	if (MThash == NULL)
		printf("# Out of memory allocating material cache.\n");


	if (TThash == NULL || EThash == NULL || MThash == NULL)
		exit(1);
}


// Almacena información sobre la evaluación de la posición
void	StoreScore(int score, HASH_KEY key)		{
	ETYPE*		index_ET	= &EThash[key & ETmask];

	index_ET->Hash	= key;
	index_ET->Score	= (I16)score;
}


// Comprueba si una posición ya se ha evaluado
int		ProbeScore(HASH_KEY key){
	ETYPE*		index_ET	= &EThash[key & ETmask];

	if (index_ET->Hash ^ key)
		return -VAL_UNKNOWN;

	return index_ET->Score;
}


// Almacena información sobre el material de la posición
void	StoreMatScore(int mid_score, int end_score, U32 key)	{
	MTYPE*		index_MT	= &MThash[key & MTmask];

	index_MT->Hash		= key;
	index_MT->MidScore	= (I16)mid_score;
	index_MT->EndScore	= (I16)end_score;
}


// Comprueba si una posición ya se ha evaluado el material
U8		ProbeMatScore(int* mid_score, int* end_score, U32 Key)	{
	MTYPE*		index_MT	= &MThash[Key & MTmask];

	if (index_MT->Hash ^ Key)
		return 0;

	*mid_score	= index_MT->MidScore;
	*end_score	= index_MT->EndScore;
	return 1;
}


void	StoreHash(int move, int score, int depth, U8 flags, HASH_KEY key, U8 current_age)		{
	BUCKET		*index_TT,
				*index2_TT	= TThash[key & TTmask].Bucket;
	int			age_i		= -1,
				depth_i		= -1,
				depth_dest	= MATE,
				i;


	for (i = 0; i < NUM_BUCKETS - 1; i++)		{
		// Buscando llaves hash a la actual, estas no deben estar repetidas
		if (index2_TT[i].Hash == key)
			break;

		// Buscando entrada vieja(al menos 4 movimientos de diferencia con el actual)
		if (index2_TT[i].Age ^ current_age)
			age_i		= i;

		// Buscando entrada de menor profundidad
		if (age_i == -1 && depth_i != -1 && depth_dest > index2_TT[i].Depth)	{
			depth_dest	= index2_TT[i].Depth;
			depth_i		= i;
		}
	}


	if (i != NUM_BUCKETS - 1)	{
		index_TT	= &index2_TT[i];							// Remplazo por llave hash, tener las entradas no repetidas es bueno

		// Actualiza la edad, si es accesible
		if (index_TT->Age ^ current_age)
			index_TT->Age	= current_age;

		if (depth < index_TT->Depth)
			return;												// No aporta informacion
	}

	else if (age_i != -1)
		index_TT	= &index2_TT[age_i];						// Remplazo por edad, entradas recientes son buenas

	else if (depth >= depth_dest)
		index_TT	= &index2_TT[depth_i];						// Remplazo por profundidad, entradas con altas profundidad son buenas

	else index_TT	= &index2_TT[i];							// Remplazo siempre, entradas cerca de la raiz del arbol de busqueda son buenas


	// Actualiza los valore de la tabla hash
	index_TT->Hash	= key;
	index_TT->Move	= (U16)move;
	index_TT->Depth	= (I16)depth;
	index_TT->Flags	= flags;
	index_TT->Score	= (I16)score;
	index_TT->Age	= current_age;
}


// Prueba si la posición esta almacenada en las tablas hash
void	ProbeHash(int* move, int depth, U8* flags, HASH_KEY key, int* hash_depth, int* hash_score, U8 current_age)				{
	int			i;
	BUCKET*		index_TT;

	*flags		= HASF_NULL;
	*move		= 0;


	for (i = 0; i < NUM_BUCKETS; i++)			{
		index_TT	= &TThash[key & TTmask].Bucket[i];

		if (index_TT->Hash == key)				{
			*flags = HASF_HIT;

			if (index_TT->Move)	{
				*move		= (int)index_TT->Move;

				// Actualiza la edad, si es accesible
				if (index_TT->Age ^ current_age)
					index_TT->Age = current_age;
			}


			if (index_TT->Depth >= depth)		{
				*flags		|= index_TT->Flags | HASF_PRUNING;
				*hash_depth	= (int)index_TT->Depth;
				*hash_score	= (int)index_TT->Score;
				return;
			}
			return;
		}
	}
}


// Obtiene la potencia de 2 mas cercana a un numero 'Number' entre 'Inf' y 'Sup'
int		GetPow(int num, int inf, int sup)		{
	int			midd;


	// Básicamente se realiza una búsqueda binaria
	while(inf <= sup)			{
		midd = ((sup - inf) >> 1) + inf;

		if(num == (1 << midd))
			return midd;

		if(num < (1 << midd))
			sup = midd - 1;

		else inf = midd + 1;
	}


	// Comprueba si la potencia de de anterior es mas cercana
	if (abs(num - (1 << inf)) < abs(num - (1 << sup)))
		return inf;

	return sup;
}


// 200
