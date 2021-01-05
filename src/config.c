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

#include "config.h"


char			ProfileName[128];
char			BookName[128];
int				Noise;
CONFIG			EngineConfig;


// Muestra la configuración del motor
void	PrintConfig(CONFIG Config)				{
	printf("\n#\n#\n# Profile\t= %s", ProfileName);
	if (Config & PROFILE)
		printf("\t\t[Not found. Using default profile]");


	printf("\n#\n# Hash\t\t= %u Mb.", TTsize);
	if (Config & HASH)
		printf("\t[Not found. Using default value]");


	printf("\n# Eval cache\t= %u Mb.", ETsize);
	if (Config & ECACHE)
		printf("\t[Not found. Using default value]");


	printf("\n#\n# Noise\t\t= %i", Noise);
	if (Config & NOISE)
		printf("\t[Not found. Using default value]");
}


// Busca el nombre del perfil que se usara en la configuración del motor
U8		FindProfile(char* Profile)		{
	char		Command[256],
				Line[256];
	FILE*		IniFile		= fopen(ENGINE_CONFIG_FILE, "r");


	if (IniFile == NULL)		{
		printf("\n#");
		return 1;
	}


	fseek(IniFile, 0, SEEK_SET);
	while (fgets(Line, 256, IniFile))
		if (Line[0] != '#' && Line[0] != ';' && (int)Line[0] > 31)				{
			sscanf(Line, "%s", Command);


			if (!strcmp(Command, "Profile"))	{
				sscanf(Line, "Profile = %s", Profile);
				fclose(IniFile);
				return 0;
			}
		}


	printf("\n# 'Profile =' keyword no found in %s file.\n", ENGINE_CONFIG_FILE);
	fclose(IniFile);
	return 2;
}


// Si falta algún dato en la configuración pone su valor por defecto
void	CompleteProfile(CONFIG Config)			{
	if (Config & NOISE)
		Noise	= DEFAULT_NOISE;

	if (Config & HASH)
		TTsize	= DEFAULT_HASH;

	if (Config & ECACHE)
		ETsize	= DEFAULT_ECACHE;


	TTmask		= MAX(1, (TTsize << 20) / sizeof(TTYPE) - 1);
	ETmask		= MAX(1, (ETsize << 20) / sizeof(ETYPE) - 1);
}


// Carga la configuración de un perfil
CONFIG	LoadProfile(char* Profile)				{
	char		Command[256],
				Line[256],
				Temp[256];
	int			Size;
	FILE		*IniFile	= fopen(ENGINE_CONFIG_FILE, "r");
	CONFIG		Config		= 0xFFFFFFFF;
	EngineConfig			= POST;


	if (IniFile == NULL)
		return Config;


	fseek(IniFile, 0, SEEK_SET);
	sprintf(Temp, "[%s]", Profile);


	while (fgets(Line, 256, IniFile))
		if (Line[0] == '[')		{
			sscanf(Line, "%s", Command);


			if (!strcmp(Temp, Command))			{
				Config ^= PROFILE;


				while (fgets(Line, 256, IniFile) && (Line[0] != '['))			{
					if (Line[0] == '#' || Line[0] == ';' || (int)Line[0] < 32)
						continue;

					sscanf(Line, "%s", Command);


					if (!strcmp(Command, "Noise"))				{
						Config ^= NOISE;
						sscanf(Line, "Noise = %d", &Noise);
					}

					else if (!strcmp(Command, "Hash"))			{
						Config ^= HASH;
						sscanf(Line, "Hash = %u", &Size);
						TTsize = 1 << GET_POW(Size);
					}

					else if (!strcmp(Command, "Ecache"))		{
						Config ^= ECACHE;
						sscanf(Line, "Ecache = %u", &Size);
						ETsize = 1 << GET_POW(Size);
					}

					else if (!strcmp(Command, "Book"))			{
						Config ^= BOOK1;
						sscanf(Line, "Book = %s", BookName);
					}
				}


				fclose(IniFile);
				strcpy(ProfileName, Profile);
				return Config;
			}
		}


	fclose(IniFile);
	return Config;
}


// 128
