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

#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <string.h>

#include "square.h"
#include "casttle.h"
#include "random.h"


//====================================================================================================================================
typedef	U64		HASH_KEY;


// Valores de tamanho por defecto para las tablas hash(en MB)
#define			DEFAULT_HASH			128
#define			DEFAULT_EHASH			64
#define			DEFAULT_PHASH			32
#define			DEFAULT_MHASH			16


// Configuración de las hash
typedef	enum	{
	HASH		= 1<<0,
	EHASH		= 1<<1,
	PHASH		= 1<<2,
	MHASH		= 1<<3,
}	HASH_CONFIG;


// Tipos de movimientos de las TTables, de momento solo se usan 4 bits
typedef	enum	{
	HASF_NULL		= 0<<0,
	HASF_EXACT		= 1<<0,
	HASF_ALPHA		= 1<<1,
	HASF_BETA		= 1<<2,
	HASF_HIT		= 1<<3,
	HASF_PRUNING	= 1<<4,
}	TTABLE_TYPE;


// Guarda las posiciones previamente analizadas
typedef struct	{
	HASH_KEY	Hash;
	U16			Move;
	I16			Score,
				Depth;
	U8			Type,											// Solo se usan [4, 5] bits, puede almacenar otros datos, como si esta en jaque, etc
				Age;
}	TENTRY;


// Evaluación de cache: guarda las posiciones previamente analizadas
typedef struct	{
	HASH_KEY	Hash;
	I16			Score;
}	EENTRY;


// Evaluación de cache: guarda las posiciones previamente analizadas de la estructura de peones
// incluye la posicion y peones alfrente de los reyes
typedef struct	{
	HASH_KEY	Hash;
	I16			MidScore,
				EndScore,
				BlackCasttle,
				WhiteCasttle;
}	PENTRY;


// Evaluación de cache: guarda las posiciones previamente analizadas
typedef struct	{
	U32			Hash;
	I16			MidScore,
				EndScore;
}	MENTRY;


//====================================================================================================================================
extern unsigned		TTsize;										// Tamano de las tablas hash en numero de 'buckets'
extern unsigned		ETsize;										// Tamano de las tablas hash evaluacion en numero de 'buckets'
extern unsigned		PTsize;										// Tamano de las tablas hash evaluacion de peones en numero de 'buckets'
extern unsigned		MTsize;										// Tamano de las tablas hash evaluacion de material en numero de 'buckets'

extern unsigned		TTmask;										// Máscara para acceder a la hash
extern unsigned		ETmask;										// Máscara para acceder a la hash evaluacion
extern unsigned		PTmask;										// Máscara para acceder a la hash evaluacion de peones
extern unsigned		MTmask;										// Máscara para acceder a la hash evaluacion de material

extern unsigned		TTsizeMB;									// Tamaño de las tablas hash en MB
extern unsigned		ETsizeMB;									// Tamaño de la hash de evaluación(evaluación cache) en MB
extern unsigned		PTsizeMB;									// Tamaño de la hash de evaluación(evaluación cache) de peones en MB
extern unsigned		MTsizeMB;									// Tamaño de la hash de evaluación(evaluación cache) del material en MB

extern TENTRY		*TThash;									// Tablas Hash
extern EENTRY		*EThash;									// Tablas Hash Evaluacion
extern PENTRY		*PThash;									// Tablas Hash Evaluacion de Peones
extern MENTRY		*MThash;									// Tablas Hash Evaluacion de Material

extern U8			HashDate;									// Edad actual de las tablas hash

extern int			UsedHash;									// Cuenta los slot usados en la tablas hash


//====================================================================================================================================
void			ClearHash();								// Borra las tablas hash y evaluación caché
void			FreeHash();									// Libera memoria de las tablas hash y de la evaluación caché
void			AllocHash(U32 tt_mb, U32 et_mb, U32 pt_mb, U32 mt_mb);	// Asigna memoria para las tablas hash y evaluación caché

void			StoreScore(int Score, HASH_KEY key);		// Almacena información sobre la evaluación de una posición
int				ProbeScore(HASH_KEY key);					// Comprueba si una posición ya se ha evaluado

void			StorePawnScore(int mid_score, int end_score, int black_casttle, int white_casttle, HASH_KEY key);
int				ProbePawnScore(int* mid_score, int* end_score, int* black_casttle, int* white_casttle, HASH_KEY key);

void			StoreMatScore(int mid_score, int end_score, U32 key);
int				ProbeMatScore(int* mid_score, int* end_score, U32 Key);

void			StoreHash(int move, int score, int depth, U8 type, HASH_KEY key, int ply);	// Se almacena información en las tablas hash
void			ProbeHash(int* move, int depth, U8* type, HASH_KEY key, int* hash_depth, int* hash_score, int ply);	// Prueba si la posición esta almacenada en las tablas hash

// Obtiene la potencia de 2 mas cercana a un numero 'Number' entre 'Inf' y 'Sup'
int				GetPow(int num, int inf, int sup);
#define			GET_POW(n)		GetPow(n, 1, 32)

void			CalcHashDate(int half_moves);					// Calcula la edad de las tablas hash

// Carga y completamiento de los perfiles de las distintas hash
HASH_CONFIG		LoadHashConfig(char* file, HASH_CONFIG hash_config);
void			CompleteHashConfig(HASH_CONFIG hash_config);


#endif // HASH_H


//	100	150
