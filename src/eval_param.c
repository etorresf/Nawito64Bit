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

#include "eval_param.h"
#include <stdio.h>
#include <string.h>


//====================================================================================================================================
int  PAWN_MAT_MID						 = 93;
int  PAWN_DIST_CENTER_FILE_MID			 = 1;
int  PAWN_DIST_LAST_RANK_MID			 = 2;
int  PAWN_CENTRAL_DEFAULT_POS_MID		 = 22;
int  PAWN_MAT_END						 = 180;
int  PAWN_DIST_CENTER_FILE_END			 = 1;
int  PAWN_DIST_LAST_RANK_END			 = 6;
int  PAWN_ADJUST						 = 48;
int  PAWN_ATTACK_KING					 = 2;		// se ha multiplicado * 4
int  PAWN_C2_BLOCKED_MID				 = 20;
int  PAWN_CENTRAL_BLOCKED_MID			 = 23;
int  PAWN_DOUBLED_MID					 = 17;
int  PAWN_DOUBLED_DIST_CENTER_FILE_MID	 = 4;
int  PAWN_DOUBLED_END					 = 21;
int  PAWN_DOUBLED_DIST_CENTER_FILE_END	 = 4;
int  PAWN_ISOLATED_MID					 = 13;
int  PAWN_ISOLATED_DIST_CENTER_FILE_MID	 = 0;
int  PAWN_ISOLATED_END					 = 27;
int  PAWN_ISOLATED_DIST_CENTER_FILE_END	 = 2;
int  PAWN_BACKWARD_MID					 = 12;
int  PAWN_BACKWARD_DIST_CENTER_FILE_MID	 = 0;
int  PAWN_BACKWARD_END					 = 30;
int  PAWN_BACKWARD_DIST_CENTER_FILE_END	 = 4;
int  PAWN_PASSED_MID					 = 102;
int  PAWN_PASSED_DIST_LAST_RANK_MID		 = 14;
int  PAWN_PASSED_END					 = 139;
int  PAWN_PASSED_DIST_LAST_RANK_END		 = 30;
int  PAWN_CANDIDATE_MID					 = 21;
int  PAWN_CANDIDATE_DIST_LAST_RANK_MID	 = 6;
int  PAWN_CANDIDATE_END					 = 51;
int  PAWN_CANDIDATE_DIST_LAST_RANK_END	 = 7;
int  PAWN_PASSED_MY_KING_TROPISM_END	 = 7;		// se ha multiplicado * 4
int  PAWN_PASSED_ENEMY_KING_TROPISM_END	 = 30;		// se ha multiplicado * 4
int  PAWN_PASSED_PROTECTED				 = 18;
int  KNIGHT_MAT_MID						 = 273;		// -16 por base de movilidad, -20 base del ajuste segun # peones
int  KNIGHT_DIST_CENTER_MID				 = 8;
int  KNIGHT_DIST_LAST_RANK_MID			 = 6;
int  KNIGHT_SLOW_DEVELOPMENT_MID		 = 10;
int  KNIGHT_MAT_END						 = 238;		// -16 por base de movilidad, -20 base del ajuste segun # peones
int  KNIGHT_DIST_CENTER_END				 = 7;
int  KNIGHT_ADJUST						 = 6;
int  KNIGHT_ATTACK_KING					 = 6;		// se ha multiplicado * 4
int  KNIGHT_PAIR						 = 7;
int  KNIGHT_TRAPPED						 = 40;
int  KNIGHT_OUTPOST_MID					 = 8;		// se ha multiplicado * 4
int  KNIGHT_PROTECTED_BY_PAWN_MID		 = 5;
int  KNIGHT_KING_TROPISM				 = 2;		// se ha multiplicado * 4
int  KNIGHT_MOB_MID						 = 7;		// se ha multiplicado * 4
int  KNIGHT_MOB_END						 = 7;		// se ha multiplicado * 4
int  BISHOP_MAT_MID						 = 248;		// -22 por base de movilidad
int  BISHOP_DIST_CENTER_MID				 = 4;
int  BISHOP_DIST_LAST_RANK_MID			 = 0;
int  BISHOP_SLOW_DEVELOPMENT_MID		 = 17;
int  BISHOP_MAT_END						 = 340;		// -22 por base de movilidad
int  BISHOP_DIST_CENTER_END				 = 7;
int  BISHOP_ATTACK_KING					 = 8;		// se ha multiplicado * 4
int  BISHOP_TRAPPED						 = 43;
int  BISHOP_PROTECTED_BY_PAWN_MID		 = 2;
int  BISHOP_KING_TROPISM				 = 6;		// se ha multiplicado * 4
int  BISHOP_MOB_MID						 = 4;		// se ha multiplicado * 4
int  BISHOP_MOB_END						 = 4;		// se ha multiplicado * 4
int  BISHOP_PAIR_BASE					 = 68;
int  BISHOP_PAIR						 = 0;
int  BISHOP_PAIR_WITH_MINOR_PIECE		 = 22;
int  BISHOP_PAIR_WITHOUT_MINOR_PIECE	 = 22;
int  BAD_BISHOP_MID						 = 17;		// se ha multiplicado * 4
int  BAD_BISHOP_END						 = 68;		// se ha multiplicado * 4
int  ROOK_MAT_MID						 = 383;		// -14 por base de movilidad, +16 por adjuste de torre segun # peones
int  ROOK_DIST_CENTER_FILE_MID			 = 4;
int  ROOK_MAT_END						 = 607;		// -28 por base de movilidad, +16 por adjuste de torre segun # peones
int  ROOK_DIST_CENTER_FILE_END			 = 6;
int  ROOK_DIST_LAST_RANK_END			 = 3;
int  ROOK_ADJUST						 = 6;
int  ROOK_IN_OPEN_FILE					 = 15;
int  ROOK_IN_HALF_OPEN_FILE				 = 12;
int  ROOK_ATTACK_KING					 = 15;		// se ha multiplicado * 4
int  ROOK_ATTACK_KING_HALF_OPEN_FILE	 = 10;		// se ha multiplicado * 4
int  ROOK_ATTACK_KING_OPEN_FILE			 = 3;		// se ha multiplicado * 4
int  ROOK_PAIR							 = 17;
int  ROOK_TRAPPED						 = 52;
int  ROOK_7_MID							 = 23;
int  ROOK_7_END							 = 30;
int  ROOK_SUPP_PASSED_END				 = 56;
int  ROOK_ATTACK_PASSED_END				 = 15;
int  ROOK_KING_TROPISM					 = 4;		// se ha multiplicado * 4
int  ROOK_SUPPORT_MID					 = 8;
int  ROOK_MOB_MID						 = 4;		// se ha multiplicado * 4
int  ROOK_MOB_END						 = 15;		// se ha multiplicado * 4
int  OWN_ROOK_BLOCK_PASSED_PAWN_END		 = 31;
int  QUEEN_MAT_MID						 = 766;		// -14 por base de movilidad
int  QUEEN_DIST_CENTER_MID				 = 4;
int  QUEEN_DIST_LAST_RANK_MID			 = 0;
int  QUEEN_MAT_END						 = 1192;	// -28 por base de movilidad
int  QUEEN_DIST_CENTER_END				 = 8;
int  QUEEN_ATTACK_KING					 = 22;		// se ha multiplicado * 4
int  QUEEN_7_MID						 = 9;
int  QUEEN_7_END						 = 20;
int  QUEEN_KING_TROPISM					 = 12;		// se ha multiplicado * 4
int  QUEEN_MOB_MID						 = 4;		// se ha multiplicado * 4
int  QUEEN_MOB_END						 = 9;		// se ha multiplicado * 4
int  KING_MAT_MID						 = 265;
int  KING_DIST_CASTTLE_MID				 = 12;
int  KING_DIST_LAST_RANK_MID			 = 6;
int  KING_MAT_END						 = 160;
int  KING_DIST_CENTER_END				 = 15;
int  KING_TROPISM_WEAK_PAWN_END			 = 12;
int  KING_FIANCHETTO_MID				 = 15;
int  KING_IN_CENTER_MID					 = 35;
int  KING_CASTTLE_DONE_MID				 = 37;
int  KING_NO_PAWN_IN_FILE				 = 90;		// se ha multiplicado * 4
int  KING_PAWN_IN_4						 = 89;		// se ha multiplicado * 4
int  KING_PAWN_IN_3						 = 43;		// se ha multiplicado * 4
int  KING_NO_PAWN_ENEMY_IN_FILE			 = 90;		// se ha multiplicado * 4
int  KING_PAWN_ENEMY_IN_3				 = 36;		// se ha multiplicado * 4
int  KING_PAWN_ENEMY_IN_4				 = 28;		// se ha multiplicado * 4
int  KING_IN_OPEN_FILE					 = 57;		// se ha multiplicado * 4
int  KING_ONLY_DEFEND					 = 6;		// se ha multiplicado * 4
int  KING_WITHOUT_FRIEND				 = 5;		// se ha multiplicado * 4
int  KING_WITH_ENEMY					 = 6;		// se ha multiplicado * 4
int  KING_WITH_ENEMY2					 = 6;		// se ha multiplicado * 4
int  TEMPO_MID							 = 16;
int  TEMPO_END							 = 6;
int  COMPELLING_END						 = 30;
int  PROTECTED_PIECES					 = 6;
int  KBKB_END							 = 320;
int  KBKB2_END							 = 96;
int  KBBKBB_END							 = 135;
int  KRKR_END							 = 13;
int  KRRKRR_END							 = 21;
int  KQKQ_END							 = 13;
int  KRBKRB_END							 = 23;
int  KRBKRB2_END						 = 15;
int  KQBKQB_END							 = 17;
int  KQBKQB2_END						 = 12;
int  KK_END								 = 17;
int  KNKN_END							 = 10;
int  ROOK_VS_MINOR_MID					 = 80;
int  ROOK_VS_MINOR_END					 = 35;
int  ROOK_VS_PAWN_MID					 = 153;
int  ROOK_VS_PAWN_END					 = 125;
int  MINOR_VS_PAWN_MID					 = 162;
int  MINOR_VS_PAWN_END					 = 103;
int  TWO_MINOR_VS_ROOK_MID				 = 68;
int  TWO_MINOR_VS_ROOK_END				 = 29;


//====================================================================================================================================
// Cargando configuracion de la evaluacion
void	LoadEvalConfig(char* file)				{
	int		value;
	char	command[256], line[256];
	FILE*	eval_file	= fopen(file, "r");


	if (eval_file != NULL)	{
		fseek(eval_file, 0, SEEK_SET);

		while (fgets(line, 256, eval_file))	{
			if (line[0] == '#' || line[0] == ';' || (int)line[0] < 32)
				continue;

			sscanf(line, "%s", command);

			if (!strcmp(command, "PAWN_MAT_MID"))				{
				sscanf(line, "PAWN_MAT_MID = %u", &value);
				PAWN_MAT_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DIST_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_DIST_CENTER_FILE_MID = %u", &value);
				PAWN_DIST_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DIST_LAST_RANK_MID"))	{
				sscanf(line, "PAWN_DIST_LAST_RANK_MID = %u", &value);
				PAWN_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "PAWN_CENTRAL_DEFAULT_POS_MID"))	{
				sscanf(line, "PAWN_CENTRAL_DEFAULT_POS_MID = %u", &value);
				PAWN_CENTRAL_DEFAULT_POS_MID	= value;
			}
			else if (!strcmp(command, "PAWN_MAT_END"))			{
				sscanf(line, "PAWN_MAT_END = %u", &value);
				PAWN_MAT_END	= value;
			}

			else if (!strcmp(command, "PAWN_DIST_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_DIST_CENTER_FILE_END = %u", &value);
				PAWN_DIST_CENTER_FILE_END	= value;
			}
			else if (!strcmp(command, "PAWN_DIST_LAST_RANK_END"))	{
				sscanf(line, "PAWN_DIST_LAST_RANK_END = %u", &value);
				PAWN_DIST_LAST_RANK_END	= value;
			}
			else if (!strcmp(command, "PAWN_ADJUST"))	{
				sscanf(line, "PAWN_ADJUST = %u", &value);
				PAWN_ADJUST	= value;
			}
			else if (!strcmp(command, "PAWN_ATTACK_KING"))	{
				sscanf(line, "PAWN_ATTACK_KING = %u", &value);
				PAWN_ATTACK_KING	= value;
			}
			else if (!strcmp(command, "PAWN_C2_BLOCKED_MID"))	{
				sscanf(line, "PAWN_C2_BLOCKED_MID = %u", &value);
				PAWN_C2_BLOCKED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_CENTRAL_BLOCKED_MID"))	{
				sscanf(line, "PAWN_CENTRAL_BLOCKED_MID = %u", &value);
				PAWN_CENTRAL_BLOCKED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DOUBLED_MID"))	{
				sscanf(line, "PAWN_DOUBLED_MID = %u", &value);
				PAWN_DOUBLED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DOUBLED_DIST_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_DOUBLED_DIST_CENTER_FILE_MID = %u", &value);
				PAWN_DOUBLED_DIST_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DOUBLED_END"))	{
				sscanf(line, "PAWN_DOUBLED_END = %u", &value);
				PAWN_DOUBLED_END	= value;
			}
			else if (!strcmp(command, "PAWN_DOUBLED_DIST_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_DOUBLED_DIST_CENTER_FILE_END = %u", &value);
				PAWN_DOUBLED_DIST_CENTER_FILE_END	= value;
			}
			else if (!strcmp(command, "PAWN_ISOLATED_MID"))	{
				sscanf(line, "PAWN_ISOLATED_MID = %u", &value);
				PAWN_ISOLATED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_ISOLATED_DIST_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_ISOLATED_DIST_CENTER_FILE_MID = %u", &value);
				PAWN_ISOLATED_DIST_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_ISOLATED_END"))	{
				sscanf(line, "PAWN_ISOLATED_END = %u", &value);
				PAWN_ISOLATED_END	= value;
			}
			else if (!strcmp(command, "PAWN_ISOLATED_DIST_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_ISOLATED_DIST_CENTER_FILE_END = %u", &value);
				PAWN_ISOLATED_DIST_CENTER_FILE_END	= value;
			}
			else if (!strcmp(command, "PAWN_BACKWARD_MID"))	{
				sscanf(line, "PAWN_BACKWARD_MID = %u", &value);
				PAWN_BACKWARD_MID	= value;
			}
			else if (!strcmp(command, "PAWN_BACKWARD_DIST_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_BACKWARD_DIST_CENTER_FILE_MID = %u", &value);
				PAWN_BACKWARD_DIST_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_BACKWARD_END"))	{
				sscanf(line, "PAWN_BACKWARD_END = %u", &value);
				PAWN_BACKWARD_END	= value;
			}
			else if (!strcmp(command, "PAWN_BACKWARD_DIST_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_BACKWARD_DIST_CENTER_FILE_END = %u", &value);
				PAWN_BACKWARD_DIST_CENTER_FILE_END	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_MID"))	{
				sscanf(line, "PAWN_PASSED_MID = %u", &value);
				PAWN_PASSED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_DIST_LAST_RANK_MID"))	{
				sscanf(line, "PAWN_PASSED_DIST_LAST_RANK_MID = %u", &value);
				PAWN_PASSED_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_END"))	{
				sscanf(line, "PAWN_PASSED_END = %u", &value);
				PAWN_PASSED_END	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_DIST_LAST_RANK_END"))	{
				sscanf(line, "PAWN_PASSED_DIST_LAST_RANK_END = %u", &value);
				PAWN_PASSED_DIST_LAST_RANK_END	= value;
			}
			else if (!strcmp(command, "PAWN_CANDIDATE_MID"))	{
				sscanf(line, "PAWN_CANDIDATE_MID = %u", &value);
				PAWN_CANDIDATE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_CANDIDATE_DIST_LAST_RANK_MID"))	{
				sscanf(line, "PAWN_CANDIDATE_DIST_LAST_RANK_MID = %u", &value);
				PAWN_CANDIDATE_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "PAWN_CANDIDATE_END"))	{
				sscanf(line, "PAWN_CANDIDATE_END = %u", &value);
				PAWN_CANDIDATE_END	= value;
			}
			else if (!strcmp(command, "PAWN_CANDIDATE_DIST_LAST_RANK_END"))	{
				sscanf(line, "PAWN_CANDIDATE_DIST_LAST_RANK_END = %u", &value);
				PAWN_CANDIDATE_DIST_LAST_RANK_END	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_MY_KING_TROPISM_END"))	{
				sscanf(line, "PAWN_PASSED_MY_KING_TROPISM_END = %u", &value);
				PAWN_PASSED_MY_KING_TROPISM_END	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_ENEMY_KING_TROPISM_END"))	{
				sscanf(line, "PAWN_PASSED_ENEMY_KING_TROPISM_END = %u", &value);
				PAWN_PASSED_ENEMY_KING_TROPISM_END	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_PROTECTED"))	{
				sscanf(line, "PAWN_PASSED_PROTECTED = %u", &value);
				PAWN_PASSED_PROTECTED	= value;
			}
			else if (!strcmp(command, "KNIGHT_MAT_MID"))	{
				sscanf(line, "KNIGHT_MAT_MID = %u", &value);
				KNIGHT_MAT_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_DIST_CENTER_MID"))	{
				sscanf(line, "KNIGHT_DIST_CENTER_MID = %u", &value);
				KNIGHT_DIST_CENTER_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_DIST_LAST_RANK_MID"))	{
				sscanf(line, "KNIGHT_DIST_LAST_RANK_MID = %u", &value);
				KNIGHT_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_SLOW_DEVELOPMENT_MID"))	{
				sscanf(line, "KNIGHT_SLOW_DEVELOPMENT_MID = %u", &value);
				KNIGHT_SLOW_DEVELOPMENT_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_MAT_END"))	{
				sscanf(line, "KNIGHT_MAT_END = %u", &value);
				KNIGHT_MAT_END	= value;
			}
			else if (!strcmp(command, "KNIGHT_DIST_CENTER_END"))	{
				sscanf(line, "KNIGHT_DIST_CENTER_END = %u", &value);
				KNIGHT_DIST_CENTER_END	= value;
			}
			else if (!strcmp(command, "KNIGHT_ADJUST"))	{
				sscanf(line, "KNIGHT_ADJUST = %u", &value);
				KNIGHT_ADJUST	= value;
			}
			else if (!strcmp(command, "KNIGHT_ATTACK_KING"))	{
				sscanf(line, "KNIGHT_ATTACK_KING = %u", &value);
				KNIGHT_ATTACK_KING	= value;
			}
			else if (!strcmp(command, "KNIGHT_PAIR"))	{
				sscanf(line, "KNIGHT_PAIR = %u", &value);
				KNIGHT_PAIR	= value;
			}
			else if (!strcmp(command, "KNIGHT_TRAPPED"))	{
				sscanf(line, "KNIGHT_TRAPPED = %u", &value);
				KNIGHT_TRAPPED	= value;
			}
			else if (!strcmp(command, "KNIGHT_OUTPOST_MID"))	{
				sscanf(line, "KNIGHT_OUTPOST_MID = %u", &value);
				KNIGHT_OUTPOST_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_PROTECTED_BY_PAWN_MID"))	{
				sscanf(line, "KNIGHT_PROTECTED_BY_PAWN_MID = %u", &value);
				KNIGHT_PROTECTED_BY_PAWN_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_KING_TROPISM"))	{
				sscanf(line, "KNIGHT_KING_TROPISM = %u", &value);
				KNIGHT_KING_TROPISM	= value;
			}
			else if (!strcmp(command, "KNIGHT_MOB_MID"))	{
				sscanf(line, "KNIGHT_MOB_MID = %u", &value);
				KNIGHT_MOB_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_MOB_END"))	{
				sscanf(line, "KNIGHT_MOB_END = %u", &value);
				KNIGHT_MOB_END	= value;
			}
			else if (!strcmp(command, "BISHOP_MAT_MID"))	{
				sscanf(line, "BISHOP_MAT_MID = %u", &value);
				BISHOP_MAT_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_DIST_CENTER_MID"))	{
				sscanf(line, "BISHOP_DIST_CENTER_MID = %u", &value);
				BISHOP_DIST_CENTER_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_DIST_LAST_RANK_MID"))	{
				sscanf(line, "BISHOP_DIST_LAST_RANK_MID = %u", &value);
				BISHOP_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_SLOW_DEVELOPMENT_MID"))	{
				sscanf(line, "BISHOP_SLOW_DEVELOPMENT_MID = %u", &value);
				BISHOP_SLOW_DEVELOPMENT_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_MAT_END"))	{
				sscanf(line, "BISHOP_MAT_END = %u", &value);
				BISHOP_MAT_END	= value;
			}
			else if (!strcmp(command, "BISHOP_DIST_CENTER_END"))	{
				sscanf(line, "BISHOP_DIST_CENTER_END = %u", &value);
				BISHOP_DIST_CENTER_END	= value;
			}
			else if (!strcmp(command, "BISHOP_ATTACK_KING"))	{
				sscanf(line, "BISHOP_ATTACK_KING = %u", &value);
				BISHOP_ATTACK_KING	= value;
			}
			else if (!strcmp(command, "BISHOP_TRAPPED"))	{
				sscanf(line, "BISHOP_TRAPPED = %u", &value);
				BISHOP_TRAPPED	= value;
			}
			else if (!strcmp(command, "BISHOP_PROTECTED_BY_PAWN_MID"))	{
				sscanf(line, "BISHOP_PROTECTED_BY_PAWN_MID = %u", &value);
				BISHOP_PROTECTED_BY_PAWN_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_KING_TROPISM"))	{
				sscanf(line, "BISHOP_KING_TROPISM = %u", &value);
				BISHOP_KING_TROPISM	= value;
			}
			else if (!strcmp(command, "BISHOP_MOB_MID"))	{
				sscanf(line, "BISHOP_MOB_MID = %u", &value);
				BISHOP_MOB_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_MOB_END"))	{
				sscanf(line, "BISHOP_MOB_END = %u", &value);
				BISHOP_MOB_END	= value;
			}
			else if (!strcmp(command, "BISHOP_PAIR_BASE"))	{
				sscanf(line, "BISHOP_PAIR_BASE = %u", &value);
				BISHOP_PAIR_BASE	= value;
			}
			else if (!strcmp(command, "BISHOP_PAIR"))	{
				sscanf(line, "BISHOP_PAIR = %u", &value);
				BISHOP_PAIR	= value;
			}
			else if (!strcmp(command, "BISHOP_PAIR_WITH_MINOR_PIECE"))	{
				sscanf(line, "BISHOP_PAIR_WITH_MINOR_PIECE = %u", &value);
				BISHOP_PAIR_WITH_MINOR_PIECE	= value;
			}
			else if (!strcmp(command, "BISHOP_PAIR_WITHOUT_MINOR_PIECE"))	{
				sscanf(line, "BISHOP_PAIR_WITHOUT_MINOR_PIECE = %u", &value);
				BISHOP_PAIR_WITHOUT_MINOR_PIECE	= value;
			}
			else if (!strcmp(command, "BAD_BISHOP_MID"))	{
				sscanf(line, "BAD_BISHOP_MID = %u", &value);
				BAD_BISHOP_MID	= value;
			}
			else if (!strcmp(command, "BAD_BISHOP_END"))	{
				sscanf(line, "BAD_BISHOP_END = %u", &value);
				BAD_BISHOP_END	= value;
			}
			else if (!strcmp(command, "ROOK_MAT_MID"))	{
				sscanf(line, "ROOK_MAT_MID = %u", &value);
				ROOK_MAT_MID	= value;
			}
			else if (!strcmp(command, "ROOK_DIST_CENTER_FILE_MID"))	{
				sscanf(line, "ROOK_DIST_CENTER_FILE_MID = %u", &value);
				ROOK_DIST_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "ROOK_MAT_END"))	{
				sscanf(line, "ROOK_MAT_END = %u", &value);
				ROOK_MAT_END	= value;
			}
			else if (!strcmp(command, "ROOK_DIST_CENTER_FILE_END"))	{
				sscanf(line, "ROOK_DIST_CENTER_FILE_END = %u", &value);
				ROOK_DIST_CENTER_FILE_END	= value;
			}
			else if (!strcmp(command, "ROOK_DIST_LAST_RANK_END"))	{
				sscanf(line, "ROOK_DIST_LAST_RANK_END = %u", &value);
				ROOK_DIST_LAST_RANK_END	= value;
			}
			else if (!strcmp(command, "ROOK_ADJUST"))	{
				sscanf(line, "ROOK_ADJUST = %u", &value);
				ROOK_ADJUST	= value;
			}
			else if (!strcmp(command, "ROOK_IN_OPEN_FILE"))	{
				sscanf(line, "ROOK_IN_OPEN_FILE = %u", &value);
				ROOK_IN_OPEN_FILE	= value;
			}
			else if (!strcmp(command, "ROOK_IN_HALF_OPEN_FILE"))	{
				sscanf(line, "ROOK_IN_HALF_OPEN_FILE = %u", &value);
				ROOK_IN_HALF_OPEN_FILE	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_KING"))	{
				sscanf(line, "ROOK_ATTACK_KING = %u", &value);
				ROOK_ATTACK_KING	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_KING_HALF_OPEN_FILE"))	{
				sscanf(line, "ROOK_ATTACK_KING_HALF_OPEN_FILE = %u", &value);
				ROOK_ATTACK_KING_HALF_OPEN_FILE	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_KING_OPEN_FILE"))	{
				sscanf(line, "ROOK_ATTACK_KING_OPEN_FILE = %u", &value);
				ROOK_ATTACK_KING_OPEN_FILE	= value;
			}
			else if (!strcmp(command, "ROOK_PAIR"))	{
				sscanf(line, "ROOK_PAIR = %u", &value);
				ROOK_PAIR	= value;
			}
			else if (!strcmp(command, "ROOK_TRAPPED"))	{
				sscanf(line, "ROOK_TRAPPED = %u", &value);
				ROOK_TRAPPED	= value;
			}
			else if (!strcmp(command, "ROOK_7_MID"))	{
				sscanf(line, "ROOK_7_MID = %u", &value);
				ROOK_7_MID	= value;
			}
			else if (!strcmp(command, "ROOK_7_END"))	{
				sscanf(line, "ROOK_7_END = %u", &value);
				ROOK_7_END	= value;
			}
			else if (!strcmp(command, "ROOK_SUPP_PASSED_END"))	{
				sscanf(line, "ROOK_SUPP_PASSED_END = %u", &value);
				ROOK_SUPP_PASSED_END	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_PASSED_END"))	{
				sscanf(line, "ROOK_ATTACK_PASSED_END = %u", &value);
				ROOK_ATTACK_PASSED_END	= value;
			}
			else if (!strcmp(command, "ROOK_KING_TROPISM"))	{
				sscanf(line, "ROOK_KING_TROPISM = %u", &value);
				ROOK_KING_TROPISM	= value;
			}
			else if (!strcmp(command, "ROOK_SUPPORT_MID"))	{
				sscanf(line, "ROOK_SUPPORT_MID = %u", &value);
				ROOK_SUPPORT_MID	= value;
			}
			else if (!strcmp(command, "ROOK_MOB_MID"))	{
				sscanf(line, "ROOK_MOB_MID = %u", &value);
				ROOK_MOB_MID	= value;
			}
			else if (!strcmp(command, "ROOK_MOB_END"))	{
				sscanf(line, "ROOK_MOB_END = %u", &value);
				ROOK_MOB_END	= value;
			}
			else if (!strcmp(command, "OWN_ROOK_BLOCK_PASSED_PAWN_END"))	{
				sscanf(line, "OWN_ROOK_BLOCK_PASSED_PAWN_END = %u", &value);
				OWN_ROOK_BLOCK_PASSED_PAWN_END	= value;
			}
			else if (!strcmp(command, "QUEEN_MAT_MID"))	{
				sscanf(line, "QUEEN_MAT_MID = %u", &value);
				QUEEN_MAT_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_DIST_CENTER_MID"))	{
				sscanf(line, "QUEEN_DIST_CENTER_MID = %u", &value);
				QUEEN_DIST_CENTER_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_DIST_LAST_RANK_MID"))	{
				sscanf(line, "QUEEN_DIST_LAST_RANK_MID = %u", &value);
				QUEEN_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_MAT_END"))	{
				sscanf(line, "QUEEN_MAT_END = %u", &value);
				QUEEN_MAT_END	= value;
			}
			else if (!strcmp(command, "QUEEN_DIST_CENTER_END"))	{
				sscanf(line, "QUEEN_DIST_CENTER_END = %u", &value);
				QUEEN_DIST_CENTER_END	= value;
			}
			else if (!strcmp(command, "QUEEN_ATTACK_KING"))	{
				sscanf(line, "QUEEN_ATTACK_KING = %u", &value);
				QUEEN_ATTACK_KING	= value;
			}
			else if (!strcmp(command, "QUEEN_7_MID"))	{
				sscanf(line, "QUEEN_7_MID = %u", &value);
				QUEEN_7_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_7_END"))	{
				sscanf(line, "QUEEN_7_END = %u", &value);
				QUEEN_7_END	= value;
			}
			else if (!strcmp(command, "QUEEN_KING_TROPISM"))	{
				sscanf(line, "QUEEN_KING_TROPISM = %u", &value);
				QUEEN_KING_TROPISM	= value;
			}
			else if (!strcmp(command, "QUEEN_MOB_MID"))	{
				sscanf(line, "QUEEN_MOB_MID = %u", &value);
				QUEEN_MOB_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_MOB_END"))	{
				sscanf(line, "QUEEN_MOB_END = %u", &value);
				QUEEN_MOB_END	= value;
			}
			else if (!strcmp(command, "KING_MAT_MID"))	{
				sscanf(line, "KING_MAT_MID = %u", &value);
				KING_MAT_MID	= value;
			}
			else if (!strcmp(command, "KING_DIST_CASTTLE_MID"))	{
				sscanf(line, "KING_DIST_CASTTLE_MID = %u", &value);
				KING_DIST_CASTTLE_MID	= value;
			}
			else if (!strcmp(command, "KING_DIST_LAST_RANK_MID"))	{
				sscanf(line, "KING_DIST_LAST_RANK_MID = %u", &value);
				KING_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "KING_MAT_END"))	{
				sscanf(line, "KING_MAT_END = %u", &value);
				KING_MAT_END	= value;
			}
			else if (!strcmp(command, "KING_DIST_CENTER_END"))	{
				sscanf(line, "KING_DIST_CENTER_END = %u", &value);
				KING_DIST_CENTER_END	= value;
			}
			else if (!strcmp(command, "KING_TROPISM_WEAK_PAWN_END"))	{
				sscanf(line, "KING_TROPISM_WEAK_PAWN_END = %u", &value);
				KING_TROPISM_WEAK_PAWN_END	= value;
			}
			else if (!strcmp(command, "KING_FIANCHETTO_MID"))	{
				sscanf(line, "KING_FIANCHETTO_MID = %u", &value);
				KING_FIANCHETTO_MID	= value;
			}
			else if (!strcmp(command, "KING_IN_CENTER_MID"))	{
				sscanf(line, "KING_IN_CENTER_MID = %u", &value);
				KING_IN_CENTER_MID	= value;
			}
			else if (!strcmp(command, "KING_CASTTLE_DONE_MID"))	{
				sscanf(line, "KING_CASTTLE_DONE_MID = %u", &value);
				KING_CASTTLE_DONE_MID	= value;
			}
			else if (!strcmp(command, "KING_NO_PAWN_IN_FILE"))	{
				sscanf(line, "KING_NO_PAWN_IN_FILE = %u", &value);
				KING_NO_PAWN_IN_FILE	= value;
			}
			else if (!strcmp(command, "KING_PAWN_IN_4"))	{
				sscanf(line, "KING_PAWN_IN_4 = %u", &value);
				KING_PAWN_IN_4	= value;
			}
			else if (!strcmp(command, "KING_PAWN_IN_3"))	{
				sscanf(line, "KING_PAWN_IN_3 = %u", &value);
				KING_PAWN_IN_3	= value;
			}
			else if (!strcmp(command, "KING_NO_PAWN_ENEMY_IN_FILE"))	{
				sscanf(line, "KING_NO_PAWN_ENEMY_IN_FILE = %u", &value);
				KING_NO_PAWN_ENEMY_IN_FILE	= value;
			}
			else if (!strcmp(command, "KING_PAWN_ENEMY_IN_3"))	{
				sscanf(line, "KING_PAWN_ENEMY_IN_3 = %u", &value);
				KING_PAWN_ENEMY_IN_3	= value;
			}
			else if (!strcmp(command, "KING_PAWN_ENEMY_IN_4"))	{
				sscanf(line, "KING_PAWN_ENEMY_IN_4 = %u", &value);
				KING_PAWN_ENEMY_IN_4	= value;
			}
			else if (!strcmp(command, "KING_IN_OPEN_FILE"))	{
				sscanf(line, "KING_IN_OPEN_FILE = %u", &value);
				KING_IN_OPEN_FILE	= value;
			}
			else if (!strcmp(command, "KING_ONLY_DEFEND"))	{
				sscanf(line, "KING_ONLY_DEFEND = %u", &value);
				KING_ONLY_DEFEND	= value;
			}
			else if (!strcmp(command, "KING_WITHOUT_FRIEND"))	{
				sscanf(line, "KING_WITHOUT_FRIEND = %u", &value);
				KING_WITHOUT_FRIEND	= value;
			}
			else if (!strcmp(command, "KING_WITH_ENEMY"))	{
				sscanf(line, "KING_WITH_ENEMY = %u", &value);
				KING_WITH_ENEMY	= value;
			}
			else if (!strcmp(command, "KING_WITH_ENEMY2"))	{
				sscanf(line, "KING_WITH_ENEMY2 = %u", &value);
				KING_WITH_ENEMY2	= value;
			}
			else if (!strcmp(command, "TEMPO_MID"))	{
				sscanf(line, "TEMPO_MID = %u", &value);
				TEMPO_MID	= value;
			}
			else if (!strcmp(command, "TEMPO_END"))	{
				sscanf(line, "TEMPO_END = %u", &value);
				TEMPO_END	= value;
			}
			else if (!strcmp(command, "COMPELLING_END"))	{
				sscanf(line, "COMPELLING_END = %u", &value);
				COMPELLING_END	= value;
			}
			else if (!strcmp(command, "PROTECTED_PIECES"))	{
				sscanf(line, "PROTECTED_PIECES = %u", &value);
				PROTECTED_PIECES	= value;
			}
			else if (!strcmp(command, "KBKB_END"))	{
				sscanf(line, "KBKB_END = %u", &value);
				KBKB_END	= value;
			}
			else if (!strcmp(command, "KBKB2_END"))	{
				sscanf(line, "KBKB2_END = %u", &value);
				KBKB2_END	= value;
			}
			else if (!strcmp(command, "KBBKBB_END"))	{
				sscanf(line, "KBBKBB_END = %u", &value);
				KBBKBB_END	= value;
			}
			else if (!strcmp(command, "KRKR_END"))	{
				sscanf(line, "KRKR_END = %u", &value);
				KRKR_END	= value;
			}
			else if (!strcmp(command, "KRRKRR_END"))	{
				sscanf(line, "KRRKRR_END = %u", &value);
				KRRKRR_END	= value;
			}
			else if (!strcmp(command, "KQKQ_END"))	{
				sscanf(line, "KQKQ_END = %u", &value);
				KQKQ_END	= value;
			}
			else if (!strcmp(command, "KRBKRB_END"))	{
				sscanf(line, "KRBKRB_END = %u", &value);
				KRBKRB_END	= value;
			}
			else if (!strcmp(command, "KRBKRB2_END"))	{
				sscanf(line, "KRBKRB2_END = %u", &value);
				KRBKRB2_END	= value;
			}
			else if (!strcmp(command, "KQBKQB_END"))	{
				sscanf(line, "KQBKQB_END = %u", &value);
				KQBKQB_END	= value;
			}
			else if (!strcmp(command, "KQBKQB2_END"))	{
				sscanf(line, "KQBKQB2_END = %u", &value);
				KQBKQB2_END	= value;
			}
			else if (!strcmp(command, "KK_END"))	{
				sscanf(line, "KK_END = %u", &value);
				KK_END	= value;
			}
			else if (!strcmp(command, "KNKN_END"))	{
				sscanf(line, "KNKN_END = %u", &value);
				KNKN_END	= value;
			}
			else if (!strcmp(command, "ROOK_VS_MINOR_MID"))	{
				sscanf(line, "ROOK_VS_MINOR_MID = %u", &value);
				ROOK_VS_MINOR_MID	= value;
			}
			else if (!strcmp(command, "ROOK_VS_MINOR_END"))	{
				sscanf(line, "ROOK_VS_MINOR_END = %u", &value);
				ROOK_VS_MINOR_END	= value;
			}
			else if (!strcmp(command, "ROOK_VS_PAWN_MID"))	{
				sscanf(line, "ROOK_VS_PAWN_MID = %u", &value);
				ROOK_VS_PAWN_MID	= value;
			}
			else if (!strcmp(command, "ROOK_VS_PAWN_END"))	{
				sscanf(line, "ROOK_VS_PAWN_END = %u", &value);
				ROOK_VS_PAWN_END	= value;
			}
			else if (!strcmp(command, "MINOR_VS_PAWN_MID"))	{
				sscanf(line, "MINOR_VS_PAWN_MID = %u", &value);
				MINOR_VS_PAWN_MID	= value;
			}
			else if (!strcmp(command, "MINOR_VS_PAWN_END"))	{
				sscanf(line, "MINOR_VS_PAWN_END = %u", &value);
				MINOR_VS_PAWN_END	= value;
			}
			else if (!strcmp(command, "TWO_MINOR_VS_ROOK_MID"))	{
				sscanf(line, "TWO_MINOR_VS_ROOK_MID = %u", &value);
				TWO_MINOR_VS_ROOK_MID	= value;
			}
			else if (!strcmp(command, "TWO_MINOR_VS_ROOK_END"))	{
				sscanf(line, "TWO_MINOR_VS_ROOK_END = %u", &value);
				TWO_MINOR_VS_ROOK_END	= value;
			}
		}

		fclose(eval_file);
	}
}


//	750
