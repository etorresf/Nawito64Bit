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


#include "config.h"



char			ProfileName[128];
char			BookName[128];
int				Noise;
CONFIG			EngineConfig;


// Muestra la configuración del motor
void	PrintConfig(CONFIG config)				{
	printf("\n#\n#\n# Profile\t= %s", ProfileName);
	if (config & PROFILE)
		printf("\t\t[Not found. Using default profile]");


	printf("\n#\n# Hash\t\t= %u Mb.", TTsize);
	if (config & HASH)
		printf("\t[Not found. Using default value]");


	printf("\n# Eval cache\t= %u Mb.", ETsize);
	if (config & ECACHE)
		printf("\t[Not found. Using default value]");


	printf("\n# Material cache= %u Mb.", MTsize);
	if (config & MCACHE)
		printf("\t\t[Not found. Using default value]");


	printf("\n#\n# Noise\t\t= %i", Noise);
	if (config & NOISE)
		printf("\t[Not found. Using default value]");


	printf("\n#\n# ShowBoard\t= ");
	config & SHOW_BOARD ? printf("off") : printf("on");
	if (config & SHOW_BOARD)
		printf("\t\t[Not found. Using default value]");


	printf("\n#\n# Book\t\t= ");
	config & USE_BOOK ? printf(" ") : printf("%s", BookName);
}


// Busca el nombre del perfil que se usara en la configuración del motor
U8		FindProfile(char* profile)				{
	char		command[256],
				line[256];
	FILE*		config_file		= fopen(ENGINE_CONFIG_FILE, "r");


	if (config_file == NULL)	{
		printf("\n#");
		return 1;
	}


	fseek(config_file, 0, SEEK_SET);
	while (fgets(line, 256, config_file))
		if (line[0] != '#' && line[0] != ';' && (int)line[0] > 31)				{
			sscanf(line, "%s", command);


			if (!strcmp(command, "Profile"))	{
				sscanf(line, "Profile = %s", profile);
				fclose(config_file);
				return 0;
			}
		}


	printf("\n# 'Profile =' keyword no found in %s file.\n", ENGINE_CONFIG_FILE);
	fclose(config_file);
	return 2;
}


// Si falta algún dato en la configuración pone su valor por defecto
void	CompleteProfile(CONFIG config)			{
	if (config & NOISE)
		Noise	= DEFAULT_NOISE;

	if (config & HASH)
		TTsize	= DEFAULT_HASH;

	if (config & ECACHE)
		ETsize	= DEFAULT_ECACHE;

	if (config & MCACHE)
		MTsize	= DEFAULT_MCACHE;


	TTmask		= MAX(1, (TTsize << 20) / sizeof(TTYPE) - 1);
	ETmask		= MAX(1, (ETsize << 20) / sizeof(ETYPE) - 1);
	MTmask		= MAX(1, (MTsize << 20) / sizeof(MTYPE) - 1);
}


// Carga la configuración de un perfil
CONFIG	LoadProfile(char* profile)				{
	char		command[256],
				line[256],
				temp[256];
	int			size;
	FILE*		config_file	= fopen(ENGINE_CONFIG_FILE, "r");
	CONFIG		config		= 0xFFFFFFFF;
	EngineConfig			= POST;


	if (config_file == NULL)
		return config;


	fseek(config_file, 0, SEEK_SET);
	sprintf(temp, "[%s]", profile);


	while (fgets(line, 256, config_file))
		if (line[0] == '[')		{
			sscanf(line, "%s", command);


			if (!strcmp(temp, command))			{
				config ^= PROFILE;


				while (fgets(line, 256, config_file) && (line[0] != '['))		{
					if (line[0] == '#' || line[0] == ';' || (int)line[0] < 32)
						continue;

					sscanf(line, "%s", command);


					if (!strcmp(command, "Noise"))				{
						config ^= NOISE;
						sscanf(line, "Noise = %d", &Noise);
					}

					else if (!strcmp(command, "ShowBoard"))		{
						sscanf(line, "ShowBoard = %s", temp);

						if (!strcmp(temp, "on"))
							config ^= SHOW_BOARD;
					}

					else if (!strcmp(command, "Hash"))			{
						config ^= HASH;
						sscanf(line, "Hash = %u", &size);
						TTsize = 1 << GET_POW(size);
					}

					else if (!strcmp(command, "Ecache"))		{
						config ^= ECACHE;
						sscanf(line, "Ecache = %u", &size);
						ETsize = 1 << GET_POW(size);
					}

					else if (!strcmp(command, "Mcache"))		{
						config ^= MCACHE;
						sscanf(line, "Mcache = %u", &size);
						MTsize = 1 << GET_POW(size);
					}

					else if (!strcmp(command, "Book"))			{
						config ^= USE_BOOK;
						sscanf(line, "Book = %s", BookName);
					}
				}


				fclose(config_file);
				strcpy(ProfileName, profile);
				return config;
			}
		}


	fclose(config_file);
	return config;
}


// 200
