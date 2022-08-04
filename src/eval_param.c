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
int  PAWN_MAT_MID = 149;
int  PAWN_MAT_END = 160;
int  PAWN_ATTACK_KING_MID = 4;
int  PAWN_ATTACK_KING_END = -2;
int  PAWN_DIST_CENTER_FILE_MID = -20;
int  PAWN_DIST_CENTER_FILE_END = 0;
int  PAWN_DIST_LAST_RANK_MID = -5;
int  PAWN_DIST_LAST_RANK_END = -6;
int  PAWN_CENTRAL_DEFAULT_POS_MID = -21;
int  PAWN_ADJUST_MID = -49;
int  PAWN_ADJUST_END = -60;
int  PAWN_C2_BLOCKED_MID = -30;
int  PAWN_CENTRAL_BLOCKED_MID = -63;
int  PAWN_DOUBLED_MID = -12;
int  PAWN_DOUBLED_END = -34;
int  PAWN_DOUBLED_CENTER_FILE_MID = 0;
int  PAWN_DOUBLED_CENTER_FILE_END = -7;
int  PAWN_DOUBLED_DIST_LAST_RANK_MID = 4;
int  PAWN_DOUBLED_DIST_LAST_RANK_END = 7;
int  PAWN_DOUBLED_PERMANENCY_MID = -1;
int  PAWN_DOUBLED_PERMANENCY_END = -5;
int  PAWN_DOUBLED_RESOURCE_ECONOMY_MID = 23;
int  PAWN_DOUBLED_RESOURCE_ECONOMY_END = -22;
int  PAWN_ISOLATED_MID = -12;
int  PAWN_ISOLATED_END = -27;
int  PAWN_ISOLATED_DIST_CENTER_FILE_MID = 1;
int  PAWN_ISOLATED_DIST_CENTER_FILE_END = 3;
int  PAWN_ISOLATED_DIST_LAST_RANK_MID = -1;
int  PAWN_ISOLATED_DIST_LAST_RANK_END = 2;
int  PAWN_BACKWARD_MID = -25;
int  PAWN_BACKWARD_END = -32;
int  PAWN_BACKWARD_DIST_CENTER_FILE_MID = 4;
int  PAWN_BACKWARD_DIST_CENTER_FILE_END = -2;
int  PAWN_BACKWARD_DIST_LAST_RANK_MID = 2;
int  PAWN_BACKWARD_DIST_LAST_RANK_END = 7;
int  PAWN_BACKWARD_PERMANENCY_MID = -10;
int  PAWN_BACKWARD_PERMANENCY_END = -13;
int  PAWN_BACKWARD_RESOURCE_ECONOMY_MID = -8;
int  PAWN_BACKWARD_RESOURCE_ECONOMY_END = -4;
int  PAWN_PASSED_MID = 89;
int  PAWN_PASSED_END = 144;
int  PAWN_PASSED_DIST_LAST_RANK_MID = -20;
int  PAWN_PASSED_DIST_LAST_RANK_END = -41;
int  PAWN_PASSED_CENTER_FILE_MID = 8;
int  PAWN_PASSED_CENTER_FILE_END = 5;
int  PAWN_CANDIDATE_MID = 16;
int  PAWN_CANDIDATE_END = 41;
int  PAWN_CANDIDATE_DIST_LAST_RANK_MID = -3;
int  PAWN_CANDIDATE_DIST_LAST_RANK_END = -7;
int  PAWN_CANDIDATE_CENTER_FILE_MID = -2;
int  PAWN_CANDIDATE_CENTER_FILE_END = -3;
int  PAWN_PASSED_MY_KING_TROPISM_END = -11;
int  PAWN_PASSED_ENEMY_KING_TROPISM_END = 20;
int  PAWN_PASSED_PROTECTED_MID = 23;
int  PAWN_PASSED_PROTECTED_END = -10;
int  PAWN_PASSED_UNSTOPPABLE_END = 794;
int  PAWN_PASSED_DANGEROUS_END = 47;
int  PAWN_PASSED_BLOCKED_BY_BISHOP_MID = 6;
int  PAWN_PASSED_BLOCKED_BY_BISHOP_END = 7;
int  PAWN_PASSED_BLOCKED_BY_KNIGHT_MID = 10;
int  PAWN_PASSED_BLOCKED_BY_KNIGHT_END = 2;
int  PAWN_PASSED_BLOCKED_BY_KING = -8;
int  PAWN_UNITED_MID = 19;
int  PAWN_UNITED_END = 2;
int  PAWN_ADVANCED_MID = 22;
int  PAWN_ADVANCED_END = -32;
int  PAWN_ADVANCED_DIST_CENTER_FILE_MID = -5;
int  PAWN_ADVANCED_DIST_CENTER_FILE_END = 14;
int  PAWN_ADVANCED_DIST_LAST_RANK_MID = -5;
int  PAWN_ADVANCED_DIST_LAST_RANK_END = -4;
int  KNIGHT_MAT_MID = 352;
int  KNIGHT_MAT_END = 330;
int  KNIGHT_KING_TROPISM_MID = -4;
int  KNIGHT_KING_TROPISM_END = 0;
int  KNIGHT_DIST_CENTER_MID = -17;
int  KNIGHT_DIST_CENTER_END = -15;
int  KNIGHT_DIST_LAST_RANK_MID = 8;
int  KNIGHT_DIST_LAST_RANK_END = -4;
int  KNIGHT_SLOW_DEVELOPMENT_MID = 2;
int  KNIGHT_ADJUST_MID = 12;
int  KNIGHT_ADJUST_END = 8;
int  KNIGHT_ATTACK_KING_MID = 2;
int  KNIGHT_ATTACK_KING_END = 8;
int  KNIGHT_PAIR_MID = 2;
int  KNIGHT_PAIR_END = -10;
int  KNIGHT_TRAPPED_A1 = -77;
int  KNIGHT_TRAPPED_A2 = -39;
int  KNIGHT_OUTPOST1_MID = 29;
int  KNIGHT_OUTPOST2_MID = 41;
int  KNIGHT_OUTPOST3_MID = 4;
int  KNIGHT_OUTPOST4_MID = 45;
int  KNIGHT_PROTECTED_BY_PAWN_MID = 7;
int  KNIGHT_PROTECTED_BY_PAWN_END = -9;
int  KNIGHT_MOB1_MID = -2;
int  KNIGHT_MOB1_END = 4;
int  KNIGHT_MOB2_MID = 4;
int  KNIGHT_MOB2_END = -3;
int  KNIGHT_ATTACK_WEAK_PIECE_MID = 73;
int  KNIGHT_ATTACK_WEAK_PIECE_END = 5;
int  KNIGHT_ATTACK_WEAK_PAWN_MID = 6;
int  KNIGHT_ATTACK_WEAK_PAWN_END = 22;
int  BISHOP_MAT_MID = 348;
int  BISHOP_MAT_END = 340;
int  BISHOP_KING_TROPISM_MID = 2;
int  BISHOP_KING_TROPISM_END = 3;
int  BISHOP_DIST_CENTER_MID = -1;
int  BISHOP_DIST_CENTER_END = -9;
int  BISHOP_DIST_LAST_RANK_MID = 7;
int  BISHOP_DIST_LAST_RANK_END = -5;
int  BISHOP_SLOW_DEVELOPMENT_MID = -29;
int  BISHOP_ATTACK_KING_MID = 0;
int  BISHOP_ATTACK_KING_END = 6;
int  BISHOP_TRAPPED_A2 = -30;
int  BISHOP_TRAPPED_A3 = 5;
int  BISHOP_PROTECTED_BY_PAWN_MID = 4;
int  BISHOP_PROTECTED_BY_PAWN_END = 1;
int  BISHOP_MOB1_MID = 12;
int  BISHOP_MOB1_END = 6;
int  BISHOP_MOB2_MID = 3;
int  BISHOP_MOB2_END = 3;
int  BISHOP_MOB3_MID = 2;
int  BISHOP_MOB3_END = 0;
int  BISHOP_PAIR_MID = 35;
int  BISHOP_PAIR_END = 68;
int  BISHOP_ATTACK_WEAK_PIECE_MID = 42;
int  BISHOP_ATTACK_WEAK_PIECE_END = 27;
int  BISHOP_ATTACK_WEAK_PAWN_MID = 2;
int  BISHOP_ATTACK_WEAK_PAWN_END = 12;
int  BAD_BISHOP1_MID = 4;
int  BAD_BISHOP1_END = -63;
int  BAD_BISHOP2_MID = -29;
int  BAD_BISHOP2_END = -24;
int  BAD_BISHOP3_MID = -23;
int  BAD_BISHOP3_END = -19;
int  BAD_BISHOP4_MID = -10;
int  BAD_BISHOP4_END = -3;
int  ROOK_MAT_MID = 460;
int  ROOK_MAT_END = 554;
int  ROOK_KING_TROPISM_MID = -2;
int  ROOK_KING_TROPISM_END = 3;
int  ROOK_DIST_CENTER_FILE_MID = -3;
int  ROOK_DIST_CENTER_FILE_END = -3;
int  ROOK_DIST_LAST_RANK_MID = 9;
int  ROOK_DIST_LAST_RANK_END = -7;
int  ROOK_ADJUST_MID = -1;
int  ROOK_ADJUST_END = 4;
int  ROOK_IN_OPEN_FILE_MID = 61;
int  ROOK_IN_HALF_OPEN_FILE_MID = 27;
int  ROOK_IN_HALF_OPEN_FILE_END = -6;
int  ROOK_ATTACK_KING_MID = 2;
int  ROOK_ATTACK_KING_END = -5;
int  ROOK_ATTACK_KING_HALF_OPEN_FILE_MID = -1;
int  ROOK_ATTACK_KING_HALF_OPEN_FILE_END = 11;
int  ROOK_ATTACK_KING_OPEN_FILE_MID = 4;
int  ROOK_ATTACK_KING_OPEN_FILE_END = 20;
int  ROOK_PAIR_MID = -36;
int  ROOK_PAIR_END = -23;
int  ROOK_TRAPPED_MID = -33;
int  ROOK_TRAPPED_END = -68;
int  ROOK_7_MID = 26;
int  ROOK_7_END = 15;
int  ROOK_SUPP_PASSED = 14;
int  ROOK_ATTACK_PASSED = -69;
int  ROOK_ROOK_SUPPORT_MID = 26;
int  ROOK_ROOK_SUPPORT_END = 1;
int  ROOK_QUEEN_SUPPORT_MID = 0;
int  ROOK_QUEEN_SUPPORT_END = 30;
int  ROOK_MOB1_MID = 1;
int  ROOK_MOB1_END = 7;
int  ROOK_MOB2_MID = 6;
int  ROOK_MOB2_END = 5;
int  ROOK_ATTACK_WEAK_PIECE_MID = 51;
int  ROOK_ATTACK_WEAK_PIECE_END = 30;
int  ROOK_ATTACK_WEAK_PAWN_MID = -1;
int  ROOK_ATTACK_WEAK_PAWN_END = 23;
int  OWN_ROOK_BLOCK_PASSED_PAWN = 20;
int  QUEEN_MAT_MID = 1083;
int  QUEEN_MAT_END = 1018;
int  QUEEN_KING_TROPISM_MID = -1;
int  QUEEN_KING_TROPISM_END = -14;
int  QUEEN_DIST_CENTER_MID = 2;
int  QUEEN_DIST_CENTER_END = 3;
int  QUEEN_DIST_LAST_RANK_MID = 13;
int  QUEEN_DIST_LAST_RANK_END = -9;
int  QUEEN_ATTACK_KING_MID = 2;
int  QUEEN_ATTACK_KING_END = 1;
int  QUEEN_7_MID = 2;
int  QUEEN_7_END = 12;
int  QUEEN_MOB_MID = 4;
int  QUEEN_MOB_END = 13;
int  KING_DIST_CASTTLE_MID = -26;
int  KING_DIST_CENTER_END = -6;
int  KING_DIST_LAST_RANK_MID = -4;
int  KING_DIST_LAST_RANK_END = -10;
int  KING_TROPISM_WEAK_PAWN_END = 48;
int  KING_FIANCHETTO_MID = 14;
int  KING_NO_FIANCHETTO_MID = -43;
int  KING_IN_CENTER_MID = -73;
int  KING_CASTTLE_DONE_MID = 25;
int  KING_NO_PAWN_IN_FILE = -52;
int  KING_PAWN_IN_4 = -22;
int  KING_PAWN_IN_3 = 5;
int  KING_NO_PAWN_ENEMY_IN_FILE = 6;
int  KING_PAWN_ENEMY_IN_3 = -46;
int  KING_PAWN_ENEMY_IN_4 = 10;
int  KING_IN_OPEN_FILE = -25;
int  KING_ONLY_DEFEND_MID = 5;
int  KING_WITHOUT_FRIEND_MID = 3;
int  KING_WITH_ENEMY_MID = 7;
int  KING_WITH_ENEMY2_MID = 3;
int  TEMPO_MID = 45;
int  ROOK_COMPELLING_MID = 43;
int  ROOK_COMPELLING_END = 37;
int  QUEEN_COMPELLING_MID = 49;
int  QUEEN_COMPELLING_END = 103;
int  PROTECTED_PIECES_MID = 4;
int  PROTECTED_PIECES_END = 12;
int  KBKB_END = 253;
int  KBKB2_END = 127;
int  KBBKBB_END = 127;
int  KRKR_END = 19;
int  KQKQ_END = 23;
int  KRBKRB_END = 20;
int  KRBKRB2_END = 14;
int  KQBKQB_END = 19;
int  KK_END = 11;
int  KNKN_END = 15;
int  KBNKBN2_END = 13;
int  KBNKBN_END = 21;
int  KNNKNN_END = 17;
int  ROOK_VS_MINOR_MID = 108;
int  ROOK_VS_MINOR_END = 84;
int  ROOK_VS_PAWN_MID = 160;
int  ROOK_VS_PAWN_END = 211;
int  MINOR_VS_PAWN_MID = 167;
int  MINOR_VS_PAWN_END = 144;
int  TWO_MINOR_VS_ROOK_MID = 93;
int  TWO_MINOR_VS_ROOK_END = 51;


//====================================================================================================================================
char  EvalParamPath[2048];


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
				sscanf(line, "PAWN_MAT_MID = %i", &value);
				PAWN_MAT_MID	= value;
			}
			else if (!strcmp(command, "PAWN_MAT_END"))			{
				sscanf(line, "PAWN_MAT_END = %i", &value);
				PAWN_MAT_END	= value;
			}

			else if (!strcmp(command, "PAWN_ATTACK_KING_MID"))	{
				sscanf(line, "PAWN_ATTACK_KING_MID = %i", &value);
				PAWN_ATTACK_KING_MID	= value;
			}
			else if (!strcmp(command, "PAWN_ATTACK_KING_END"))	{
				sscanf(line, "PAWN_ATTACK_KING_END = %i", &value);
				PAWN_ATTACK_KING_END	= value;
			}

			else if (!strcmp(command, "PAWN_DIST_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_DIST_CENTER_FILE_MID = %i", &value);
				PAWN_DIST_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DIST_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_DIST_CENTER_FILE_END = %i", &value);
				PAWN_DIST_CENTER_FILE_END	= value;
			}

			else if (!strcmp(command, "PAWN_DIST_LAST_RANK_MID"))	{
				sscanf(line, "PAWN_DIST_LAST_RANK_MID = %i", &value);
				PAWN_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DIST_LAST_RANK_END"))	{
				sscanf(line, "PAWN_DIST_LAST_RANK_END = %i", &value);
				PAWN_DIST_LAST_RANK_END	= value;
			}
			
			else if (!strcmp(command, "PAWN_CENTRAL_DEFAULT_POS_MID"))	{
				sscanf(line, "PAWN_CENTRAL_DEFAULT_POS_MID = %i", &value);
				PAWN_CENTRAL_DEFAULT_POS_MID	= value;
			}

			else if (!strcmp(command, "PAWN_ADJUST_MID"))	{
				sscanf(line, "PAWN_ADJUST_MID = %i", &value);
				PAWN_ADJUST_MID	= value;
			}
			else if (!strcmp(command, "PAWN_ADJUST_END"))	{
				sscanf(line, "PAWN_ADJUST_END = %i", &value);
				PAWN_ADJUST_END	= value;
			}

			else if (!strcmp(command, "PAWN_C2_BLOCKED_MID"))	{
				sscanf(line, "PAWN_C2_BLOCKED_MID = %i", &value);
				PAWN_C2_BLOCKED_MID	= value;
			}

			else if (!strcmp(command, "PAWN_CENTRAL_BLOCKED_MID"))	{
				sscanf(line, "PAWN_CENTRAL_BLOCKED_MID = %i", &value);
				PAWN_CENTRAL_BLOCKED_MID	= value;
			}

			else if (!strcmp(command, "PAWN_DOUBLED_MID"))	{
				sscanf(line, "PAWN_DOUBLED_MID = %i", &value);
				PAWN_DOUBLED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DOUBLED_END"))	{
				sscanf(line, "PAWN_DOUBLED_END = %i", &value);
				PAWN_DOUBLED_END	= value;
			}

			else if (!strcmp(command, "PAWN_DOUBLED_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_DOUBLED_CENTER_FILE_MID = %i", &value);
				PAWN_DOUBLED_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DOUBLED_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_DOUBLED_CENTER_FILE_END = %i", &value);
				PAWN_DOUBLED_CENTER_FILE_END	= value;
			}

			else if (!strcmp(command, "PAWN_DOUBLED_DIST_LAST_RANK_MID"))	{
				sscanf(line, "PAWN_DOUBLED_DIST_LAST_RANK_MID = %i", &value);
				PAWN_DOUBLED_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DOUBLED_DIST_LAST_RANK_END"))	{
				sscanf(line, "PAWN_DOUBLED_DIST_LAST_RANK_END = %i", &value);
				PAWN_DOUBLED_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "PAWN_DOUBLED_PERMANENCY_MID"))	{
				sscanf(line, "PAWN_DOUBLED_PERMANENCY_MID = %i", &value);
				PAWN_DOUBLED_PERMANENCY_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DOUBLED_PERMANENCY_END"))	{
				sscanf(line, "PAWN_DOUBLED_PERMANENCY_END = %i", &value);
				PAWN_DOUBLED_PERMANENCY_END	= value;
			}

			else if (!strcmp(command, "PAWN_DOUBLED_RESOURCE_ECONOMY_MID"))	{
				sscanf(line, "PAWN_DOUBLED_RESOURCE_ECONOMY_MID = %i", &value);
				PAWN_DOUBLED_RESOURCE_ECONOMY_MID	= value;
			}
			else if (!strcmp(command, "PAWN_DOUBLED_RESOURCE_ECONOMY_END"))	{
				sscanf(line, "PAWN_DOUBLED_RESOURCE_ECONOMY_END = %i", &value);
				PAWN_DOUBLED_RESOURCE_ECONOMY_END	= value;
			}

			else if (!strcmp(command, "PAWN_ISOLATED_MID"))	{
				sscanf(line, "PAWN_ISOLATED_MID = %i", &value);
				PAWN_ISOLATED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_ISOLATED_END"))	{
				sscanf(line, "PAWN_ISOLATED_END = %i", &value);
				PAWN_ISOLATED_END	= value;
			}

			else if (!strcmp(command, "PAWN_ISOLATED_DIST_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_ISOLATED_DIST_CENTER_FILE_MID = %i", &value);
				PAWN_ISOLATED_DIST_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_ISOLATED_DIST_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_ISOLATED_DIST_CENTER_FILE_END = %i", &value);
				PAWN_ISOLATED_DIST_CENTER_FILE_END	= value;
			}

			else if (!strcmp(command, "PAWN_ISOLATED_DIST_LAST_RANK_MID"))	{
				sscanf(line, "PAWN_ISOLATED_DIST_LAST_RANK_MID = %i", &value);
				PAWN_ISOLATED_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "PAWN_ISOLATED_DIST_LAST_RANK_END"))	{
				sscanf(line, "PAWN_ISOLATED_DIST_LAST_RANK_END = %i", &value);
				PAWN_ISOLATED_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "PAWN_BACKWARD_MID"))	{
				sscanf(line, "PAWN_BACKWARD_MID = %i", &value);
				PAWN_BACKWARD_MID	= value;
			}
			else if (!strcmp(command, "PAWN_BACKWARD_END"))	{
				sscanf(line, "PAWN_BACKWARD_END = %i", &value);
				PAWN_BACKWARD_END	= value;
			}

			else if (!strcmp(command, "PAWN_BACKWARD_DIST_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_BACKWARD_DIST_CENTER_FILE_MID = %i", &value);
				PAWN_BACKWARD_DIST_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_BACKWARD_DIST_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_BACKWARD_DIST_CENTER_FILE_END = %i", &value);
				PAWN_BACKWARD_DIST_CENTER_FILE_END	= value;
			}

			else if (!strcmp(command, "PAWN_BACKWARD_DIST_LAST_RANK_MID"))	{
				sscanf(line, "PAWN_BACKWARD_DIST_LAST_RANK_MID = %i", &value);
				PAWN_BACKWARD_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "PAWN_BACKWARD_DIST_LAST_RANK_END"))	{
				sscanf(line, "PAWN_BACKWARD_DIST_LAST_RANK_END = %i", &value);
				PAWN_BACKWARD_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "PAWN_BACKWARD_PERMANENCY_MID"))	{
				sscanf(line, "PAWN_BACKWARD_PERMANENCY_MID = %i", &value);
				PAWN_BACKWARD_PERMANENCY_MID	= value;
			}
			else if (!strcmp(command, "PAWN_BACKWARD_PERMANENCY_END"))	{
				sscanf(line, "PAWN_BACKWARD_PERMANENCY_END = %i", &value);
				PAWN_BACKWARD_PERMANENCY_END	= value;
			}

			else if (!strcmp(command, "PAWN_BACKWARD_RESOURCE_ECONOMY_MID"))	{
				sscanf(line, "PAWN_BACKWARD_RESOURCE_ECONOMY_MID = %i", &value);
				PAWN_BACKWARD_RESOURCE_ECONOMY_MID	= value;
			}
			else if (!strcmp(command, "PAWN_BACKWARD_RESOURCE_ECONOMY_END"))	{
				sscanf(line, "PAWN_BACKWARD_RESOURCE_ECONOMY_END = %i", &value);
				PAWN_BACKWARD_RESOURCE_ECONOMY_END	= value;
			}

			else if (!strcmp(command, "PAWN_ADVANCED_MID"))	{
				sscanf(line, "PAWN_ADVANCED_MID = %i", &value);
				PAWN_ADVANCED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_ADVANCED_END"))	{
				sscanf(line, "PAWN_ADVANCED_END = %i", &value);
				PAWN_ADVANCED_END	= value;
			}

			else if (!strcmp(command, "PAWN_ADVANCED_DIST_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_ADVANCED_DIST_CENTER_FILE_MID = %i", &value);
				PAWN_ADVANCED_DIST_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_ADVANCED_DIST_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_ADVANCED_DIST_CENTER_FILE_END = %i", &value);
				PAWN_ADVANCED_DIST_CENTER_FILE_END	= value;
			}

			else if (!strcmp(command, "PAWN_ADVANCED_DIST_LAST_RANK_MID"))	{
				sscanf(line, "PAWN_ADVANCED_DIST_LAST_RANK_MID = %i", &value);
				PAWN_ADVANCED_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "PAWN_ADVANCED_DIST_LAST_RANK_END"))	{
				sscanf(line, "PAWN_ADVANCED_DIST_LAST_RANK_END = %i", &value);
				PAWN_ADVANCED_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_MID"))	{
				sscanf(line, "PAWN_PASSED_MID = %i", &value);
				PAWN_PASSED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_END"))	{
				sscanf(line, "PAWN_PASSED_END = %i", &value);
				PAWN_PASSED_END	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_DIST_LAST_RANK_MID"))	{
				sscanf(line, "PAWN_PASSED_DIST_LAST_RANK_MID = %i", &value);
				PAWN_PASSED_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_DIST_LAST_RANK_END"))	{
				sscanf(line, "PAWN_PASSED_DIST_LAST_RANK_END = %i", &value);
				PAWN_PASSED_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_PASSED_CENTER_FILE_MID = %i", &value);
				PAWN_PASSED_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_PASSED_CENTER_FILE_END = %i", &value);
				PAWN_PASSED_CENTER_FILE_END	= value;
			}

			else if (!strcmp(command, "PAWN_UNITED_MID"))	{
				sscanf(line, "PAWN_UNITED_MID = %i", &value);
				PAWN_UNITED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_UNITED_END"))	{
				sscanf(line, "PAWN_UNITED_END = %i", &value);
				PAWN_UNITED_END	= value;
			}

			else if (!strcmp(command, "PAWN_CANDIDATE_MID"))	{
				sscanf(line, "PAWN_CANDIDATE_MID = %i", &value);
				PAWN_CANDIDATE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_CANDIDATE_END"))	{
				sscanf(line, "PAWN_CANDIDATE_END = %i", &value);
				PAWN_CANDIDATE_END	= value;
			}

			else if (!strcmp(command, "PAWN_CANDIDATE_DIST_LAST_RANK_MID"))	{
				sscanf(line, "PAWN_CANDIDATE_DIST_LAST_RANK_MID = %i", &value);
				PAWN_CANDIDATE_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "PAWN_CANDIDATE_DIST_LAST_RANK_END"))	{
				sscanf(line, "PAWN_CANDIDATE_DIST_LAST_RANK_END = %i", &value);
				PAWN_CANDIDATE_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "PAWN_CANDIDATE_CENTER_FILE_MID"))	{
				sscanf(line, "PAWN_CANDIDATE_CENTER_FILE_MID = %i", &value);
				PAWN_CANDIDATE_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "PAWN_CANDIDATE_CENTER_FILE_END"))	{
				sscanf(line, "PAWN_CANDIDATE_CENTER_FILE_END = %i", &value);
				PAWN_CANDIDATE_CENTER_FILE_END	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_PROTECTED_MID"))	{
				sscanf(line, "PAWN_PASSED_PROTECTED_MID = %i", &value);
				PAWN_PASSED_PROTECTED_MID	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_PROTECTED_END"))	{
				sscanf(line, "PAWN_PASSED_PROTECTED_END = %i", &value);
				PAWN_PASSED_PROTECTED_END	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_MY_KING_TROPISM_END"))	{
				sscanf(line, "PAWN_PASSED_MY_KING_TROPISM_END = %i", &value);
				PAWN_PASSED_MY_KING_TROPISM_END	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_ENEMY_KING_TROPISM_END"))	{
				sscanf(line, "PAWN_PASSED_ENEMY_KING_TROPISM_END = %i", &value);
				PAWN_PASSED_ENEMY_KING_TROPISM_END	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_UNSTOPPABLE_END"))	{
				sscanf(line, "PAWN_PASSED_UNSTOPPABLE_END = %i", &value);
				PAWN_PASSED_UNSTOPPABLE_END	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_DANGEROUS_END"))	{
				sscanf(line, "PAWN_PASSED_DANGEROUS_END = %i", &value);
				PAWN_PASSED_DANGEROUS_END	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_BLOCKED_BY_BISHOP_MID"))	{
				sscanf(line, "PAWN_PASSED_BLOCKED_BY_BISHOP_MID = %i", &value);
				PAWN_PASSED_BLOCKED_BY_BISHOP_MID	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_BLOCKED_BY_BISHOP_END"))	{
				sscanf(line, "PAWN_PASSED_BLOCKED_BY_BISHOP_END = %i", &value);
				PAWN_PASSED_BLOCKED_BY_BISHOP_END	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_BLOCKED_BY_KING"))	{
				sscanf(line, "PAWN_PASSED_BLOCKED_BY_KING = %i", &value);
				PAWN_PASSED_BLOCKED_BY_KING	= value;
			}

			else if (!strcmp(command, "PAWN_PASSED_BLOCKED_BY_KNIGHT_MID"))	{
				sscanf(line, "PAWN_PASSED_BLOCKED_BY_KNIGHT_MID = %i", &value);
				PAWN_PASSED_BLOCKED_BY_KNIGHT_MID	= value;
			}
			else if (!strcmp(command, "PAWN_PASSED_BLOCKED_BY_KNIGHT_END"))	{
				sscanf(line, "PAWN_PASSED_BLOCKED_BY_KNIGHT_END = %i", &value);
				PAWN_PASSED_BLOCKED_BY_KNIGHT_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_MAT_MID"))	{
				sscanf(line, "KNIGHT_MAT_MID = %i", &value);
				KNIGHT_MAT_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_MAT_END"))	{
				sscanf(line, "KNIGHT_MAT_END = %i", &value);
				KNIGHT_MAT_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_KING_TROPISM_MID"))	{
				sscanf(line, "KNIGHT_KING_TROPISM_MID = %i", &value);
				KNIGHT_KING_TROPISM_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_KING_TROPISM_END"))	{
				sscanf(line, "KNIGHT_KING_TROPISM_END = %i", &value);
				KNIGHT_KING_TROPISM_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_DIST_CENTER_MID"))	{
				sscanf(line, "KNIGHT_DIST_CENTER_MID = %i", &value);
				KNIGHT_DIST_CENTER_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_DIST_CENTER_END"))	{
				sscanf(line, "KNIGHT_DIST_CENTER_END = %i", &value);
				KNIGHT_DIST_CENTER_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_DIST_LAST_RANK_MID"))	{
				sscanf(line, "KNIGHT_DIST_LAST_RANK_MID = %i", &value);
				KNIGHT_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_DIST_LAST_RANK_END"))	{
				sscanf(line, "KNIGHT_DIST_LAST_RANK_END = %i", &value);
				KNIGHT_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_SLOW_DEVELOPMENT_MID"))	{
				sscanf(line, "KNIGHT_SLOW_DEVELOPMENT_MID = %i", &value);
				KNIGHT_SLOW_DEVELOPMENT_MID	= value;
			}

			else if (!strcmp(command, "KNIGHT_ADJUST_MID"))	{
				sscanf(line, "KNIGHT_ADJUST_MID = %i", &value);
				KNIGHT_ADJUST_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_ADJUST_END"))	{
				sscanf(line, "KNIGHT_ADJUST_END = %i", &value);
				KNIGHT_ADJUST_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_ATTACK_KING_MID"))	{
				sscanf(line, "KNIGHT_ATTACK_KING_MID = %i", &value);
				KNIGHT_ATTACK_KING_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_ATTACK_KING_END"))	{
				sscanf(line, "KNIGHT_ATTACK_KING_END = %i", &value);
				KNIGHT_ATTACK_KING_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_PAIR_MID"))	{
				sscanf(line, "KNIGHT_PAIR_MID = %i", &value);
				KNIGHT_PAIR_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_PAIR_END"))	{
				sscanf(line, "KNIGHT_PAIR_END = %i", &value);
				KNIGHT_PAIR_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_TRAPPED_A1"))	{
				sscanf(line, "KNIGHT_TRAPPED_A1 = %i", &value);
				KNIGHT_TRAPPED_A1	= value;
			}

			else if (!strcmp(command, "KNIGHT_TRAPPED_A2"))	{
				sscanf(line, "KNIGHT_TRAPPED_A2 = %i", &value);
				KNIGHT_TRAPPED_A2	= value;
			}

			else if (!strcmp(command, "KNIGHT_OUTPOST1_MID"))	{
				sscanf(line, "KNIGHT_OUTPOST1_MID = %i", &value);
				KNIGHT_OUTPOST1_MID	= value;
			}

			else if (!strcmp(command, "KNIGHT_OUTPOST2_MID"))	{
				sscanf(line, "KNIGHT_OUTPOST2_MID = %i", &value);
				KNIGHT_OUTPOST2_MID	= value;
			}

			else if (!strcmp(command, "KNIGHT_OUTPOST3_MID"))	{
				sscanf(line, "KNIGHT_OUTPOST3_MID = %i", &value);
				KNIGHT_OUTPOST3_MID	= value;
			}

			else if (!strcmp(command, "KNIGHT_OUTPOST4_MID"))	{
				sscanf(line, "KNIGHT_OUTPOST4_MID = %i", &value);
				KNIGHT_OUTPOST4_MID	= value;
			}

			else if (!strcmp(command, "KNIGHT_PROTECTED_BY_PAWN_MID"))	{
				sscanf(line, "KNIGHT_PROTECTED_BY_PAWN_MID = %i", &value);
				KNIGHT_PROTECTED_BY_PAWN_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_PROTECTED_BY_PAWN_END"))	{
				sscanf(line, "KNIGHT_PROTECTED_BY_PAWN_END = %i", &value);
				KNIGHT_PROTECTED_BY_PAWN_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_MOB1_MID"))	{
				sscanf(line, "KNIGHT_MOB1_MID = %i", &value);
				KNIGHT_MOB1_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_MOB1_END"))	{
				sscanf(line, "KNIGHT_MOB1_END = %i", &value);
				KNIGHT_MOB1_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_MOB2_MID"))	{
				sscanf(line, "KNIGHT_MOB2_MID = %i", &value);
				KNIGHT_MOB2_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_MOB2_END"))	{
				sscanf(line, "KNIGHT_MOB2_END = %i", &value);
				KNIGHT_MOB2_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_ATTACK_WEAK_PIECE_MID"))	{
				sscanf(line, "KNIGHT_ATTACK_WEAK_PIECE_MID = %i", &value);
				KNIGHT_ATTACK_WEAK_PIECE_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_ATTACK_WEAK_PIECE_END"))	{
				sscanf(line, "KNIGHT_ATTACK_WEAK_PIECE_END = %i", &value);
				KNIGHT_ATTACK_WEAK_PIECE_END	= value;
			}

			else if (!strcmp(command, "KNIGHT_ATTACK_WEAK_PAWN_MID"))	{
				sscanf(line, "KNIGHT_ATTACK_WEAK_PAWN_MID = %i", &value);
				KNIGHT_ATTACK_WEAK_PAWN_MID	= value;
			}
			else if (!strcmp(command, "KNIGHT_ATTACK_WEAK_PAWN_END"))	{
				sscanf(line, "KNIGHT_ATTACK_WEAK_PAWN_END = %i", &value);
				KNIGHT_ATTACK_WEAK_PAWN_END	= value;
			}

			else if (!strcmp(command, "BISHOP_MAT_MID"))	{
				sscanf(line, "BISHOP_MAT_MID = %i", &value);
				BISHOP_MAT_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_MAT_END"))	{
				sscanf(line, "BISHOP_MAT_END = %i", &value);
				BISHOP_MAT_END	= value;
			}

			else if (!strcmp(command, "BISHOP_KING_TROPISM_MID"))	{
				sscanf(line, "BISHOP_KING_TROPISM_MID = %i", &value);
				BISHOP_KING_TROPISM_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_KING_TROPISM_END"))	{
				sscanf(line, "BISHOP_KING_TROPISM_END = %i", &value);
				BISHOP_KING_TROPISM_END	= value;
			}

			else if (!strcmp(command, "BISHOP_DIST_CENTER_MID"))	{
				sscanf(line, "BISHOP_DIST_CENTER_MID = %i", &value);
				BISHOP_DIST_CENTER_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_DIST_CENTER_END"))	{
				sscanf(line, "BISHOP_DIST_CENTER_END = %i", &value);
				BISHOP_DIST_CENTER_END	= value;
			}

			else if (!strcmp(command, "BISHOP_DIST_LAST_RANK_MID"))	{
				sscanf(line, "BISHOP_DIST_LAST_RANK_MID = %i", &value);
				BISHOP_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_DIST_LAST_RANK_END"))	{
				sscanf(line, "BISHOP_DIST_LAST_RANK_END = %i", &value);
				BISHOP_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "BISHOP_SLOW_DEVELOPMENT_MID"))	{
				sscanf(line, "BISHOP_SLOW_DEVELOPMENT_MID = %i", &value);
				BISHOP_SLOW_DEVELOPMENT_MID	= value;
			}

			else if (!strcmp(command, "BISHOP_ATTACK_KING_MID"))	{
				sscanf(line, "BISHOP_ATTACK_KING_MID = %i", &value);
				BISHOP_ATTACK_KING_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_ATTACK_KING_END"))	{
				sscanf(line, "BISHOP_ATTACK_KING_END = %i", &value);
				BISHOP_ATTACK_KING_END	= value;
			}

			else if (!strcmp(command, "BISHOP_TRAPPED_A2"))	{
				sscanf(line, "BISHOP_TRAPPED_A2 = %i", &value);
				BISHOP_TRAPPED_A2	= value;
			}

			else if (!strcmp(command, "BISHOP_TRAPPED_A3"))	{
				sscanf(line, "BISHOP_TRAPPED_A3 = %i", &value);
				BISHOP_TRAPPED_A3	= value;
			}

			else if (!strcmp(command, "BISHOP_PROTECTED_BY_PAWN_MID"))	{
				sscanf(line, "BISHOP_PROTECTED_BY_PAWN_MID = %i", &value);
				BISHOP_PROTECTED_BY_PAWN_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_PROTECTED_BY_PAWN_END"))	{
				sscanf(line, "BISHOP_PROTECTED_BY_PAWN_END = %i", &value);
				BISHOP_PROTECTED_BY_PAWN_END	= value;
			}

			else if (!strcmp(command, "BISHOP_MOB1_MID"))	{
				sscanf(line, "BISHOP_MOB1_MID = %i", &value);
				BISHOP_MOB1_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_MOB1_END"))	{
				sscanf(line, "BISHOP_MOB1_END = %i", &value);
				BISHOP_MOB1_END	= value;
			}

			else if (!strcmp(command, "BISHOP_MOB2_MID"))	{
				sscanf(line, "BISHOP_MOB2_MID = %i", &value);
				BISHOP_MOB2_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_MOB2_END"))	{
				sscanf(line, "BISHOP_MOB2_END = %i", &value);
				BISHOP_MOB2_END	= value;
			}

			else if (!strcmp(command, "BISHOP_MOB3_MID"))	{
				sscanf(line, "BISHOP_MOB3_MID = %i", &value);
				BISHOP_MOB3_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_MOB3_END"))	{
				sscanf(line, "BISHOP_MOB3_END = %i", &value);
				BISHOP_MOB3_END	= value;
			}

			else if (!strcmp(command, "BISHOP_PAIR_MID"))	{
				sscanf(line, "BISHOP_PAIR_MID = %i", &value);
				BISHOP_PAIR_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_PAIR_END"))	{
				sscanf(line, "BISHOP_PAIR_END = %i", &value);
				BISHOP_PAIR_END	= value;
			}

			else if (!strcmp(command, "BISHOP_ATTACK_WEAK_PIECE_MID"))	{
				sscanf(line, "BISHOP_ATTACK_WEAK_PIECE_MID = %i", &value);
				BISHOP_ATTACK_WEAK_PIECE_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_ATTACK_WEAK_PIECE_END"))	{
				sscanf(line, "BISHOP_ATTACK_WEAK_PIECE_END = %i", &value);
				BISHOP_ATTACK_WEAK_PIECE_END	= value;
			}

			else if (!strcmp(command, "BISHOP_ATTACK_WEAK_PAWN_MID"))	{
				sscanf(line, "BISHOP_ATTACK_WEAK_PAWN_MID = %i", &value);
				BISHOP_ATTACK_WEAK_PAWN_MID	= value;
			}
			else if (!strcmp(command, "BISHOP_ATTACK_WEAK_PAWN_END"))	{
				sscanf(line, "BISHOP_ATTACK_WEAK_PAWN_END = %i", &value);
				BISHOP_ATTACK_WEAK_PAWN_END	= value;
			}

			else if (!strcmp(command, "BAD_BISHOP1_MID"))	{
				sscanf(line, "BAD_BISHOP1_MID = %i", &value);
				BAD_BISHOP1_MID	= value;
			}
			else if (!strcmp(command, "BAD_BISHOP1_END"))	{
				sscanf(line, "BAD_BISHOP1_END = %i", &value);
				BAD_BISHOP1_END	= value;
			}

			else if (!strcmp(command, "BAD_BISHOP2_MID"))	{
				sscanf(line, "BAD_BISHOP2_MID = %i", &value);
				BAD_BISHOP2_MID	= value;
			}
			else if (!strcmp(command, "BAD_BISHOP2_END"))	{
				sscanf(line, "BAD_BISHOP2_END = %i", &value);
				BAD_BISHOP2_END	= value;
			}

			else if (!strcmp(command, "BAD_BISHOP3_MID"))	{
				sscanf(line, "BAD_BISHOP3_MID = %i", &value);
				BAD_BISHOP3_MID	= value;
			}
			else if (!strcmp(command, "BAD_BISHOP3_END"))	{
				sscanf(line, "BAD_BISHOP3_END = %i", &value);
				BAD_BISHOP3_END	= value;
			}

			else if (!strcmp(command, "BAD_BISHOP4_MID"))	{
				sscanf(line, "BAD_BISHOP4_MID = %i", &value);
				BAD_BISHOP4_MID	= value;
			}
			else if (!strcmp(command, "BAD_BISHOP4_END"))	{
				sscanf(line, "BAD_BISHOP4_END = %i", &value);
				BAD_BISHOP4_END	= value;
			}

			else if (!strcmp(command, "ROOK_MAT_MID"))	{
				sscanf(line, "ROOK_MAT_MID = %i", &value);
				ROOK_MAT_MID	= value;
			}
			else if (!strcmp(command, "ROOK_MAT_END"))	{
				sscanf(line, "ROOK_MAT_END = %i", &value);
				ROOK_MAT_END	= value;
			}

			else if (!strcmp(command, "ROOK_KING_TROPISM_MID"))	{
				sscanf(line, "ROOK_KING_TROPISM_MID = %i", &value);
				ROOK_KING_TROPISM_MID	= value;
			}
			else if (!strcmp(command, "ROOK_KING_TROPISM_END"))	{
				sscanf(line, "ROOK_KING_TROPISM_END = %i", &value);
				ROOK_KING_TROPISM_END	= value;
			}

			else if (!strcmp(command, "ROOK_DIST_CENTER_FILE_MID"))	{
				sscanf(line, "ROOK_DIST_CENTER_FILE_MID = %i", &value);
				ROOK_DIST_CENTER_FILE_MID	= value;
			}
			else if (!strcmp(command, "ROOK_DIST_CENTER_FILE_END"))	{
				sscanf(line, "ROOK_DIST_CENTER_FILE_END = %i", &value);
				ROOK_DIST_CENTER_FILE_END	= value;
			}

			else if (!strcmp(command, "ROOK_DIST_LAST_RANK_MID"))	{
				sscanf(line, "ROOK_DIST_LAST_RANK_MID = %i", &value);
				ROOK_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "ROOK_DIST_LAST_RANK_END"))	{
				sscanf(line, "ROOK_DIST_LAST_RANK_END = %i", &value);
				ROOK_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "ROOK_ADJUST_MID"))	{
				sscanf(line, "ROOK_ADJUST_MID = %i", &value);
				ROOK_ADJUST_MID	= value;
			}
			else if (!strcmp(command, "ROOK_ADJUST_END"))	{
				sscanf(line, "ROOK_ADJUST_END = %i", &value);
				ROOK_ADJUST_END	= value;
			}

			else if (!strcmp(command, "ROOK_IN_OPEN_FILE_MID"))	{
				sscanf(line, "ROOK_IN_OPEN_FILE_MID = %i", &value);
				ROOK_IN_OPEN_FILE_MID	= value;
			}

			else if (!strcmp(command, "ROOK_IN_HALF_OPEN_FILE_MID"))	{
				sscanf(line, "ROOK_IN_HALF_OPEN_FILE_MID = %i", &value);
				ROOK_IN_HALF_OPEN_FILE_MID	= value;
			}
			else if (!strcmp(command, "ROOK_IN_HALF_OPEN_FILE_END"))	{
				sscanf(line, "ROOK_IN_HALF_OPEN_FILE_END = %i", &value);
				ROOK_IN_HALF_OPEN_FILE_END	= value;
			}

			else if (!strcmp(command, "ROOK_ATTACK_KING_MID"))	{
				sscanf(line, "ROOK_ATTACK_KING_MID = %i", &value);
				ROOK_ATTACK_KING_MID	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_KING_END"))	{
				sscanf(line, "ROOK_ATTACK_KING_END = %i", &value);
				ROOK_ATTACK_KING_END	= value;
			}

			else if (!strcmp(command, "ROOK_ATTACK_KING_HALF_OPEN_FILE_MID"))	{
				sscanf(line, "ROOK_ATTACK_KING_HALF_OPEN_FILE_MID = %i", &value);
				ROOK_ATTACK_KING_HALF_OPEN_FILE_MID	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_KING_HALF_OPEN_FILE_END"))	{
				sscanf(line, "ROOK_ATTACK_KING_HALF_OPEN_FILE_END = %i", &value);
				ROOK_ATTACK_KING_HALF_OPEN_FILE_END	= value;
			}

			else if (!strcmp(command, "ROOK_ATTACK_KING_OPEN_FILE_MID"))	{
				sscanf(line, "ROOK_ATTACK_KING_OPEN_FILE_MID = %i", &value);
				ROOK_ATTACK_KING_OPEN_FILE_MID	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_KING_OPEN_FILE_END"))	{
				sscanf(line, "ROOK_ATTACK_KING_OPEN_FILE_END = %i", &value);
				ROOK_ATTACK_KING_OPEN_FILE_END	= value;
			}

			else if (!strcmp(command, "ROOK_PAIR_MID"))	{
				sscanf(line, "ROOK_PAIR_MID = %i", &value);
				ROOK_PAIR_MID	= value;
			}
			else if (!strcmp(command, "ROOK_PAIR_END"))	{
				sscanf(line, "ROOK_PAIR_END = %i", &value);
				ROOK_PAIR_END	= value;
			}

			else if (!strcmp(command, "ROOK_TRAPPED_MID"))	{
				sscanf(line, "ROOK_TRAPPED_MID = %i", &value);
				ROOK_TRAPPED_MID	= value;
			}
			else if (!strcmp(command, "ROOK_TRAPPED_END"))	{
				sscanf(line, "ROOK_TRAPPED_END = %i", &value);
				ROOK_TRAPPED_END	= value;
			}

			else if (!strcmp(command, "ROOK_7_MID"))	{
				sscanf(line, "ROOK_7_MID = %i", &value);
				ROOK_7_MID	= value;
			}
			else if (!strcmp(command, "ROOK_7_END"))	{
				sscanf(line, "ROOK_7_END = %i", &value);
				ROOK_7_END	= value;
			}

			else if (!strcmp(command, "ROOK_SUPP_PASSED"))	{
				sscanf(line, "ROOK_SUPP_PASSED = %i", &value);
				ROOK_SUPP_PASSED	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_PASSED")){
				sscanf(line, "ROOK_ATTACK_PASSED = %i", &value);
				ROOK_ATTACK_PASSED	= value;
			}

			else if (!strcmp(command, "ROOK_ROOK_SUPPORT_MID"))	{
				sscanf(line, "ROOK_ROOK_SUPPORT_MID = %i", &value);
				ROOK_ROOK_SUPPORT_MID	= value;
			}
			else if (!strcmp(command, "ROOK_ROOK_SUPPORT_END"))	{
				sscanf(line, "ROOK_ROOK_SUPPORT_END = %i", &value);
				ROOK_ROOK_SUPPORT_END	= value;
			}

			else if (!strcmp(command, "ROOK_QUEEN_SUPPORT_MID")){
				sscanf(line, "ROOK_QUEEN_SUPPORT_MID = %i", &value);
				ROOK_QUEEN_SUPPORT_MID	= value;
			}
			else if (!strcmp(command, "ROOK_QUEEN_SUPPORT_END")){
				sscanf(line, "ROOK_QUEEN_SUPPORT_END = %i", &value);
				ROOK_QUEEN_SUPPORT_END	= value;
			}

			else if (!strcmp(command, "ROOK_MOB1_MID"))	{
				sscanf(line, "ROOK_MOB1_MID = %i", &value);
				ROOK_MOB1_MID	= value;
			}
			else if (!strcmp(command, "ROOK_MOB1_END"))	{
				sscanf(line, "ROOK_MOB1_END = %i", &value);
				ROOK_MOB1_END	= value;
			}

			else if (!strcmp(command, "ROOK_MOB2_MID"))	{
				sscanf(line, "ROOK_MOB2_MID = %i", &value);
				ROOK_MOB2_MID	= value;
			}
			else if (!strcmp(command, "ROOK_MOB2_END"))	{
				sscanf(line, "ROOK_MOB2_END = %i", &value);
				ROOK_MOB2_END	= value;
			}

			else if (!strcmp(command, "ROOK_ATTACK_WEAK_PIECE_MID"))	{
				sscanf(line, "ROOK_ATTACK_WEAK_PIECE_MID = %i", &value);
				ROOK_ATTACK_WEAK_PIECE_MID	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_WEAK_PIECE_END"))	{
				sscanf(line, "ROOK_ATTACK_WEAK_PIECE_END = %i", &value);
				ROOK_ATTACK_WEAK_PIECE_END	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_WEAK_PAWN_MID"))	{
				sscanf(line, "ROOK_ATTACK_WEAK_PAWN_MID = %i", &value);
				ROOK_ATTACK_WEAK_PAWN_MID	= value;
			}
			else if (!strcmp(command, "ROOK_ATTACK_WEAK_PAWN_END"))	{
				sscanf(line, "ROOK_ATTACK_WEAK_PAWN_END = %i", &value);
				ROOK_ATTACK_WEAK_PAWN_END	= value;
			}

			else if (!strcmp(command, "OWN_ROOK_BLOCK_PASSED_PAWN"))	{
				sscanf(line, "OWN_ROOK_BLOCK_PASSED_PAWN = %i", &value);
				OWN_ROOK_BLOCK_PASSED_PAWN	= value;
			}

			else if (!strcmp(command, "QUEEN_MAT_MID"))	{
				sscanf(line, "QUEEN_MAT_MID = %i", &value);
				QUEEN_MAT_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_MAT_END"))	{
				sscanf(line, "QUEEN_MAT_END = %i", &value);
				QUEEN_MAT_END	= value;
			}

			else if (!strcmp(command, "QUEEN_KING_TROPISM_MID"))	{
				sscanf(line, "QUEEN_KING_TROPISM_MID = %i", &value);
				QUEEN_KING_TROPISM_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_KING_TROPISM_END"))	{
				sscanf(line, "QUEEN_KING_TROPISM_END = %i", &value);
				QUEEN_KING_TROPISM_END	= value;
			}

			else if (!strcmp(command, "QUEEN_DIST_CENTER_MID"))	{
				sscanf(line, "QUEEN_DIST_CENTER_MID = %i", &value);
				QUEEN_DIST_CENTER_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_DIST_CENTER_END"))	{
				sscanf(line, "QUEEN_DIST_CENTER_END = %i", &value);
				QUEEN_DIST_CENTER_END	= value;
			}

			else if (!strcmp(command, "QUEEN_DIST_LAST_RANK_MID"))	{
				sscanf(line, "QUEEN_DIST_LAST_RANK_MID = %i", &value);
				QUEEN_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_DIST_LAST_RANK_END"))	{
				sscanf(line, "QUEEN_DIST_LAST_RANK_END = %i", &value);
				QUEEN_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "QUEEN_ATTACK_KING_MID"))	{
				sscanf(line, "QUEEN_ATTACK_KING_MID = %i", &value);
				QUEEN_ATTACK_KING_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_ATTACK_KING_END"))	{
				sscanf(line, "QUEEN_ATTACK_KING_END = %i", &value);
				QUEEN_ATTACK_KING_END	= value;
			}

			else if (!strcmp(command, "QUEEN_7_MID"))	{
				sscanf(line, "QUEEN_7_MID = %i", &value);
				QUEEN_7_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_7_END"))	{
				sscanf(line, "QUEEN_7_END = %i", &value);
				QUEEN_7_END	= value;
			}

			else if (!strcmp(command, "QUEEN_MOB_MID"))	{
				sscanf(line, "QUEEN_MOB_MID = %i", &value);
				QUEEN_MOB_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_MOB_END"))	{
				sscanf(line, "QUEEN_MOB_END = %i", &value);
				QUEEN_MOB_END	= value;
			}

			else if (!strcmp(command, "KING_DIST_CASTTLE_MID"))	{
				sscanf(line, "KING_DIST_CASTTLE_MID = %i", &value);
				KING_DIST_CASTTLE_MID	= value;
			}
			else if (!strcmp(command, "KING_DIST_CENTER_END"))	{
				sscanf(line, "KING_DIST_CENTER_END = %i", &value);
				KING_DIST_CENTER_END	= value;
			}

			else if (!strcmp(command, "KING_DIST_LAST_RANK_MID"))	{
				sscanf(line, "KING_DIST_LAST_RANK_MID = %i", &value);
				KING_DIST_LAST_RANK_MID	= value;
			}
			else if (!strcmp(command, "KING_DIST_LAST_RANK_END"))	{
				sscanf(line, "KING_DIST_LAST_RANK_END = %i", &value);
				KING_DIST_LAST_RANK_END	= value;
			}

			else if (!strcmp(command, "KING_TROPISM_WEAK_PAWN_END"))	{
				sscanf(line, "KING_TROPISM_WEAK_PAWN_END = %i", &value);
				KING_TROPISM_WEAK_PAWN_END	= value;
			}

			else if (!strcmp(command, "KING_FIANCHETTO_MID"))	{
				sscanf(line, "KING_FIANCHETTO_MID = %i", &value);
				KING_FIANCHETTO_MID	= value;
			}

			else if (!strcmp(command, "KING_NO_FIANCHETTO_MID"))	{
				sscanf(line, "KING_NO_FIANCHETTO_MID = %i", &value);
				KING_NO_FIANCHETTO_MID	= value;
			}

			else if (!strcmp(command, "KING_IN_CENTER_MID"))	{
				sscanf(line, "KING_IN_CENTER_MID = %i", &value);
				KING_IN_CENTER_MID	= value;
			}

			else if (!strcmp(command, "KING_CASTTLE_DONE_MID"))	{
				sscanf(line, "KING_CASTTLE_DONE_MID = %i", &value);
				KING_CASTTLE_DONE_MID	= value;
			}

			else if (!strcmp(command, "KING_NO_PAWN_IN_FILE"))	{
				sscanf(line, "KING_NO_PAWN_IN_FILE = %i", &value);
				KING_NO_PAWN_IN_FILE	= value;
			}

			else if (!strcmp(command, "KING_PAWN_IN_4"))	{
				sscanf(line, "KING_PAWN_IN_4 = %i", &value);
				KING_PAWN_IN_4	= value;
			}

			else if (!strcmp(command, "KING_PAWN_IN_3"))	{
				sscanf(line, "KING_PAWN_IN_3 = %i", &value);
				KING_PAWN_IN_3	= value;
			}

			else if (!strcmp(command, "KING_NO_PAWN_ENEMY_IN_FILE"))	{
				sscanf(line, "KING_NO_PAWN_ENEMY_IN_FILE = %i", &value);
				KING_NO_PAWN_ENEMY_IN_FILE	= value;
			}

			else if (!strcmp(command, "KING_PAWN_ENEMY_IN_3"))	{
				sscanf(line, "KING_PAWN_ENEMY_IN_3 = %i", &value);
				KING_PAWN_ENEMY_IN_3	= value;
			}

			else if (!strcmp(command, "KING_PAWN_ENEMY_IN_4"))	{
				sscanf(line, "KING_PAWN_ENEMY_IN_4 = %i", &value);
				KING_PAWN_ENEMY_IN_4	= value;
			}
			else if (!strcmp(command, "KING_IN_OPEN_FILE"))	{
				sscanf(line, "KING_IN_OPEN_FILE = %i", &value);
				KING_IN_OPEN_FILE	= value;
			}

			else if (!strcmp(command, "KING_ONLY_DEFEND_MID"))	{
				sscanf(line, "KING_ONLY_DEFEND_MID = %i", &value);
				KING_ONLY_DEFEND_MID	= value;
			}

			else if (!strcmp(command, "KING_WITHOUT_FRIEND_MID"))	{
				sscanf(line, "KING_WITHOUT_FRIEND_MID = %i", &value);
				KING_WITHOUT_FRIEND_MID	= value;
			}

			else if (!strcmp(command, "KING_WITH_ENEMY_MID"))	{
				sscanf(line, "KING_WITH_ENEMY_MID = %i", &value);
				KING_WITH_ENEMY_MID	= value;
			}

			else if (!strcmp(command, "KING_WITH_ENEMY2_MID"))	{
				sscanf(line, "KING_WITH_ENEMY2_MID = %i", &value);
				KING_WITH_ENEMY2_MID	= value;
			}

			else if (!strcmp(command, "TEMPO_MID"))	{
				sscanf(line, "TEMPO_MID = %i", &value);
				TEMPO_MID	= value;
			}

			else if (!strcmp(command, "ROOK_COMPELLING_MID"))	{
				sscanf(line, "ROOK_COMPELLING_MID = %i", &value);
				ROOK_COMPELLING_MID	= value;
			}
			else if (!strcmp(command, "ROOK_COMPELLING_END"))	{
				sscanf(line, "ROOK_COMPELLING_END = %i", &value);
				ROOK_COMPELLING_END	= value;
			}

			else if (!strcmp(command, "QUEEN_COMPELLING_MID"))	{
				sscanf(line, "QUEEN_COMPELLING_MID = %i", &value);
				QUEEN_COMPELLING_MID	= value;
			}
			else if (!strcmp(command, "QUEEN_COMPELLING_END"))	{
				sscanf(line, "QUEEN_COMPELLING_END = %i", &value);
				QUEEN_COMPELLING_END	= value;
			}

			else if (!strcmp(command, "PROTECTED_PIECES_MID"))	{
				sscanf(line, "PROTECTED_PIECES_MID = %i", &value);
				PROTECTED_PIECES_MID	= value;
			}

			else if (!strcmp(command, "PROTECTED_PIECES_END"))	{
				sscanf(line, "PROTECTED_PIECES_END = %i", &value);
				PROTECTED_PIECES_END	= value;
			}

			else if (!strcmp(command, "KBKB_END"))	{
				sscanf(line, "KBKB_END = %i", &value);
				KBKB_END	= value;
			}
			else if (!strcmp(command, "KBKB2_END"))	{
				sscanf(line, "KBKB2_END = %i", &value);
				KBKB2_END	= value;
			}
			else if (!strcmp(command, "KBBKBB_END"))	{
				sscanf(line, "KBBKBB_END = %i", &value);
				KBBKBB_END	= value;
			}
			else if (!strcmp(command, "KRKR_END"))	{
				sscanf(line, "KRKR_END = %i", &value);
				KRKR_END	= value;
			}
			else if (!strcmp(command, "KQKQ_END"))	{
				sscanf(line, "KQKQ_END = %i", &value);
				KQKQ_END	= value;
			}
			else if (!strcmp(command, "KRBKRB_END"))	{
				sscanf(line, "KRBKRB_END = %i", &value);
				KRBKRB_END	= value;
			}
			else if (!strcmp(command, "KRBKRB2_END"))	{
				sscanf(line, "KRBKRB2_END = %i", &value);
				KRBKRB2_END	= value;
			}
			else if (!strcmp(command, "KQBKQB_END"))	{
				sscanf(line, "KQBKQB_END = %i", &value);
				KQBKQB_END	= value;
			}
			else if (!strcmp(command, "KK_END"))	{
				sscanf(line, "KK_END = %i", &value);
				KK_END	= value;
			}
			else if (!strcmp(command, "KNKN_END"))	{
				sscanf(line, "KNKN_END = %i", &value);
				KNKN_END	= value;
			}
			else if (!strcmp(command, "KBNKBN2_END"))	{
				sscanf(line, "KBNKBN2_END = %i", &value);
				KBNKBN2_END	= value;
			}
			else if (!strcmp(command, "KBNKBN_END"))	{
				sscanf(line, "KBNKBN_END = %i", &value);
				KBNKBN_END	= value;
			}
			else if (!strcmp(command, "KNNKNN_END"))	{
				sscanf(line, "KNNKNN_END = %i", &value);
				KNNKNN_END	= value;
			}

			else if (!strcmp(command, "ROOK_VS_MINOR_MID"))	{
				sscanf(line, "ROOK_VS_MINOR_MID = %i", &value);
				ROOK_VS_MINOR_MID	= value;
			}
			else if (!strcmp(command, "ROOK_VS_MINOR_END"))	{
				sscanf(line, "ROOK_VS_MINOR_END = %i", &value);
				ROOK_VS_MINOR_END	= value;
			}

			else if (!strcmp(command, "ROOK_VS_PAWN_MID"))	{
				sscanf(line, "ROOK_VS_PAWN_MID = %i", &value);
				ROOK_VS_PAWN_MID	= value;
			}
			else if (!strcmp(command, "ROOK_VS_PAWN_END"))	{
				sscanf(line, "ROOK_VS_PAWN_END = %i", &value);
				ROOK_VS_PAWN_END	= value;
			}

			else if (!strcmp(command, "MINOR_VS_PAWN_MID"))	{
				sscanf(line, "MINOR_VS_PAWN_MID = %i", &value);
				MINOR_VS_PAWN_MID	= value;
			}
			else if (!strcmp(command, "MINOR_VS_PAWN_END"))	{
				sscanf(line, "MINOR_VS_PAWN_END = %i", &value);
				MINOR_VS_PAWN_END	= value;
			}

			else if (!strcmp(command, "TWO_MINOR_VS_ROOK_MID"))	{
				sscanf(line, "TWO_MINOR_VS_ROOK_MID = %i", &value);
				TWO_MINOR_VS_ROOK_MID	= value;
			}
			else if (!strcmp(command, "TWO_MINOR_VS_ROOK_END"))	{
				sscanf(line, "TWO_MINOR_VS_ROOK_END = %i", &value);
				TWO_MINOR_VS_ROOK_END	= value;
			}
		}

		fclose(eval_file);
	}
}


// Carga el nombre del libro a usar y su configuracion
void	LoadEvalParamConfig(char* file)			{
	char	command[256], line[256];
	FILE*	config_file		= fopen(file, "r");


	if (config_file != NULL)	{
		fseek(config_file, 0, SEEK_SET);

		while (fgets(line, 256, config_file))	{
			if (line[0] == '#' || line[0] == ';' || (int)line[0] < 32)
				continue;

			sscanf(line, "%s", command);
			if (!strcmp(command, "EvalParam"))		{
				sscanf(line, "EvalParam = %s", EvalParamPath);
				break;
			}
		}

		fclose(config_file);
	}
}



//	750
