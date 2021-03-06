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


#ifndef CONFIG_H
#define CONFIG_H

#include "hash.h"



typedef	U32			CONFIG;


#define				ENGINE_CONFIG_FILE		"Nawito2103.ini"


#define				DEFAULT_NOISE			200000
#define				DEFAULT_HASH			128
#define				DEFAULT_ECACHE			64
#define				DEFAULT_MCACHE			2


// Configuración del motor
typedef	enum		ENGINE_CONFIG_TYPE		{
	MAKE_PONDER		= 1<<2,
	POST			= 1<<3,
	SHOW_BOARD		= 1<<4,
	NOISE			= 1<<5,
	HASH			= 1<<6,
	ECACHE			= 1<<7,
	MCACHE			= 1<<8,
	PROFILE			= 1<<9,
	USE_BOOK		= 1<<10,
	LOCK_BOOK		= 1<<11,
}	ENGINE_CONFIG;



extern char			ProfileName[128];
extern char			BookName[128];

extern int			Noise;										// Cantidad mínima de nodos para imprimir información del PV

extern CONFIG		EngineConfig;								// configuración del motor



void				PrintConfig(CONFIG config);					// Muestra la configuración del motor

U8					FindProfile(char* profile);					// Busca el nombre del perfil que se usara en la configuración del motor

void				CompleteProfile(CONFIG config);				// Si falta algún dato en la configuración pone su valor por defecto

CONFIG				LoadProfile(char* profile);					// Carga la configuración de un perfil


#endif // CONFIG_H
