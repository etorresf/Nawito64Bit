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

#include "hash.h"


//====================================================================================================================================
TENTRY			*TThash;
EENTRY			*EThash;
PENTRY			*PThash;
MENTRY			*MThash;

unsigned		TTsize;
unsigned		ETsize;
unsigned		PTsize;
unsigned		MTsize;

unsigned		TTmask;
unsigned		ETmask;
unsigned		PTmask;
unsigned		MTmask;

unsigned		TTsizeMB;
unsigned		ETsizeMB;
unsigned		PTsizeMB;
unsigned		MTsizeMB;

U8				HashDate;

int				UsedHash;


//====================================================================================================================================
// Borra las tablas hash y evaluación caché
void	ClearHash()				{
	if (TThash)
		memset(TThash,	0, sizeof(TENTRY) * TTsize);

	if (EThash)
		memset(EThash,	0, sizeof(EENTRY) * ETsize);

	if (PThash)
		memset(PThash,	0, sizeof(PENTRY) * PTsize);

	if (MThash)
		memset(MThash,	0, sizeof(MENTRY) * MTsize);

	UsedHash		= 0;
}


// Libera memoria de las tablas hash y de la evaluación caché
void	FreeHash()				{
	free(TThash);
	free(EThash);
	free(PThash);
	free(MThash);
}


/*
 * Asigna memoria para las tablas hash.
 * Calcula el numero de elementos de las hash
 * Calcula las mascaras de acceso a las hash
 */
void	AllocHash(U32 tt_mb, U32 et_mb, U32 pt_mb, U32 mt_mb)	{
	// Almacenando el tamanho de las tablas hash
	TTsizeMB	= tt_mb;
	ETsizeMB	= et_mb;
	PTsizeMB	= pt_mb;
	MTsizeMB	= mt_mb;

	// Calculando el numero de elementos de las hash
	TTsize		= MAX(4, (TTsizeMB << 20) / sizeof(TENTRY));
	ETsize		= MAX(1, (ETsizeMB << 20) / sizeof(EENTRY));
	PTsize		= MAX(1, (PTsizeMB << 20) / sizeof(PENTRY));
	MTsize		= MAX(1, (MTsizeMB << 20) / sizeof(MENTRY));

	// Calculando las mascaras de acceso a las hash
	TTmask		= (int)TTsize - 4;
	ETmask		= (int)ETsize - 1;
	PTmask		= (int)PTsize - 1;
	MTmask		= (int)MTsize - 1;

	// Asignando memoria para las tablas hash
	TThash		= (TENTRY*) malloc(sizeof(TENTRY) * TTsize);
	EThash		= (EENTRY*) malloc(sizeof(EENTRY) * ETsize);
	PThash		= (PENTRY*) malloc(sizeof(PENTRY) * PTsize);
	MThash		= (MENTRY*) malloc(sizeof(MENTRY) * MTsize);


	// Detectando y mostrando errores al reservar memoria para las hash
	if (TThash == NULL)
		printf("# Out of memory allocating hash.\n");

	if (EThash == NULL)
		printf("# Out of memory allocating eval hash.\n");

	if (PThash == NULL)
		printf("# Out of memory allocating pawn hash.\n");

	if (MThash == NULL)
		printf("# Out of memory allocating material hash.\n");

	if (TThash == NULL || EThash == NULL || PThash == NULL || MThash == NULL)
		exit(1);
}


// Almacena información sobre la evaluación de la posición
void	StoreScore(int score, HASH_KEY key)		{
	EENTRY*	entry	= &EThash[key & ETmask];

	entry->Hash		= key;
	entry->Score	= (I16)score;
}


// Comprueba si una posición ya se ha evaluado
int		ProbeScore(HASH_KEY key){
	EENTRY*	entry	= &EThash[key & ETmask];

	if (entry->Hash == key)
		return entry->Score;

	return -MATE;
}


// Almacena información sobre la evaluación de la estructura de peones
void	StorePawnScore(int mid_score, int end_score, int black_casttle, int white_casttle, HASH_KEY key)		{
	PENTRY*	entry		= &PThash[key & PTmask];

	entry->Hash			= key;
	entry->MidScore		= (I16)mid_score;
	entry->EndScore		= (I16)end_score;
	entry->BlackCasttle	= (I16)black_casttle;
	entry->WhiteCasttle	= (I16)white_casttle;
}


// Comprueba si una posición ya se ha evaluado
int		ProbePawnScore(int* mid_score, int* end_score,  int* black_casttle, int* white_casttle, HASH_KEY key)	{
	PENTRY*	entry		= &PThash[key & PTmask];

	if (entry->Hash == key)	{
		*mid_score		= entry->MidScore;
		*end_score		= entry->EndScore;
		*black_casttle	= entry->BlackCasttle;
		*white_casttle	= entry->WhiteCasttle;
		return 1;
	}

	return -MATE;
}


// Almacena información sobre el material de la posición
void	StoreMatScore(int mid_score, int end_score, U32 key)	{
	MENTRY*	entry	= &MThash[key & MTmask];

	entry->Hash		= key;
	entry->MidScore	= (I16)mid_score;
	entry->EndScore	= (I16)end_score;
}


// Comprueba si una posición ya se ha evaluado el material
int		ProbeMatScore(int* mid_score, int* end_score, U32 Key)	{
	MENTRY*	entry	= &MThash[Key & MTmask];

	if (entry->Hash == Key)	{
		*mid_score	= entry->MidScore;
		*end_score	= entry->EndScore;
		return 1;
	}

	return -MATE;
}


// Almacena la evaluacion de la posicion en las tablas hash
void	StoreHash(int move, int score, int depth, U8 type, HASH_KEY key, int ply)				{
	int		i;
	TENTRY	*entry			= TThash + (key & TTmask),
			*depth_entry	= entry,
			*age_entry		= NULL;


	for (i = 0; i < 3; i++)	{
		// Busca llaves hash igual a la actual, estas no deben estar repetidas
		if (entry->Hash == key)				{
			if (entry->Depth <= depth)
				break;

			entry->Age	= HashDate;							// No aporta informacion
			return;
		}


		if (age_entry == NULL)				{
			// Busca entrada vieja
			if (HashDate ^ entry->Age)
				age_entry	= entry;

			// Busca entrada de menor profundidad
			else if (entry->Depth < depth_entry->Depth)
				depth_entry	= entry;
		}

		entry++;
	}


	// 1. remplazo por hash repetida
	if (i < 3);

	// 2. remplazo por edad
	else if (age_entry != NULL)
		entry	= age_entry;

	// 3. remplazo por profundidad
	else if (depth_entry->Depth <= depth)
		entry	= depth_entry;

	// 4. remplazo siempre


	// Ajusta valor de evaluacion en caso de MATE
	if (score >= MATE_SCORE)
		score	+= ply;

	else if (score <= -MATE_SCORE)
		score	-= ply;


	// Contando entradas usuadas de la hash
	if (entry->Age == 0)
		UsedHash++;

	// Actualiza los valore de la tabla hash
	entry->Hash		= key;
	entry->Move		= (U16)move;
	entry->Depth	= (I16)depth;
	entry->Type		= type;
	entry->Score	= (I16)score;
	entry->Age		= HashDate;
}


// Prueba si la posición esta almacenada en las tablas hash
void	ProbeHash(int* move, int depth, U8* type, HASH_KEY key, int* hash_depth, int* hash_score, int ply)		{
	int			i;
	TENTRY		*entry	= TThash + (key & TTmask);

	*type		= HASF_NULL;
	*move		= 0;


	for (i = 0; i < 4; i++)	{
		if (entry->Hash == key)				{
			*type			= HASF_HIT;
			*move			= (int)entry->Move;
			entry->Age		= HashDate;


			if (entry->Depth >= depth)		{
				*type		|= entry->Type | HASF_PRUNING;
				*hash_depth	= (int)entry->Depth;
				*hash_score	= (int)entry->Score;


				// Ajusta valor de evaluacion en caso de MATE
				if (*hash_score >= MATE_SCORE)
					*hash_score	-= ply;

				else if (*hash_score <= -MATE_SCORE)
					*hash_score	+= ply;
			}
			return;
		}

		entry++;
	}
}


// Obtiene la potencia de 2 mas cercana a un numero 'Number' entre 'Inf' y 'Sup'
int		GetPow(int num, int inf, int sup)		{
	int			midd;


	// Básicamente se realiza una búsqueda binaria
	while(inf <= sup)			{
		midd	= ((sup - inf) >> 1) + inf;

		if(num == (1 << midd))
			return midd;

		if(num < (1 << midd))
			sup		= midd - 1;

		else inf	= midd + 1;
	}


	// Comprueba si la potencia de de anterior es mas cercana
	if (abs(num - (1 << inf)) < abs(num - (1 << sup)))
		return inf;

	return sup;
}


// Calcula la edad de las tablas hash
void	CalcHashDate(int half_moves)			{	HashDate = (half_moves / 6) & 255;	}


// Carga desde el fichero de configuracion el tamanho de las tablas hash
// De no aparecer configuracion para alguna hash, los retorna como desaparecido
HASH_CONFIG	LoadHashConfig(char* file, HASH_CONFIG hash_config)	{
	int			size;
	char		command[256], line[256];
	FILE*		config_file		= fopen(file, "r");
	HASH_CONFIG	missing_config	= hash_config;


	if (config_file != NULL)	{
		fseek(config_file, 0, SEEK_SET);

		while (fgets(line, 256, config_file) && hash_config)	{
			if (line[0] == '#' || line[0] == ';' || (int)line[0] < 32)
				continue;

			sscanf(line, "%s", command);

			if (!strcmp(command, "Hash") && (hash_config & HASH))				{
				sscanf(line, "Hash = %u", &size);
				TTsizeMB		= 1 << GET_POW(size);
				missing_config	^= HASH;
			}

			else if (!strcmp(command, "Ehash") && (hash_config & EHASH))		{
				sscanf(line, "Ehash = %u", &size);
				ETsizeMB		= 1 << GET_POW(size);
				missing_config	^= EHASH;
			}

			else if (!strcmp(command, "Phash") && (hash_config & PHASH))		{
				sscanf(line, "Phash = %u", &size);
				PTsizeMB		= 1 << GET_POW(size);
				missing_config	^= PHASH;
			}

			else if (!strcmp(command, "Mhash") && (hash_config & MHASH))		{
				sscanf(line, "Mhash = %u", &size);
				MTsizeMB		= 1 << GET_POW(size);
				missing_config	^= MHASH;
			}
		}

		fclose(config_file);
	}

	return hash_config & missing_config;
}


// Si falta algún dato en la configuración de las hash, pone su valor por defecto
void	CompleteHashConfig(HASH_CONFIG hash_config)				{
	if (hash_config & HASH)
		TTsizeMB	= DEFAULT_HASH;

	if (hash_config & EHASH)
		ETsizeMB	= DEFAULT_EHASH;

	if (hash_config & PHASH)
		PTsizeMB	= DEFAULT_PHASH;

	if (hash_config & MHASH)
		MTsizeMB	= DEFAULT_MHASH;
}


//	200	250	300	350
