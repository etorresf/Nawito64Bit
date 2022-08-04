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

//====================================================================================================================================
extern int	TEMPO_MID;
extern int	PAWN_CENTRAL_DEFAULT_POS_MID;
extern int	PAWN_MAT_MID;

extern int	PAWN_ATTACK_KING_MID;
extern int	PAWN_ATTACK_KING_END;

extern int	PAWN_DIST_CENTER_FILE_MID;
extern int	PAWN_DIST_LAST_RANK_MID;
extern int	PAWN_MAT_END;
extern int	PAWN_DIST_CENTER_FILE_END;
extern int	PAWN_DIST_LAST_RANK_END;

extern int	PAWN_DOUBLED_MID;
extern int	PAWN_DOUBLED_END;

extern int	PAWN_DOUBLED_CENTER_FILE_MID;
extern int	PAWN_DOUBLED_CENTER_FILE_END;

extern int	PAWN_DOUBLED_DIST_LAST_RANK_MID;
extern int	PAWN_DOUBLED_DIST_LAST_RANK_END;

extern int	PAWN_DOUBLED_PERMANENCY_MID;
extern int	PAWN_DOUBLED_PERMANENCY_END;

extern int	PAWN_DOUBLED_RESOURCE_ECONOMY_MID;
extern int	PAWN_DOUBLED_RESOURCE_ECONOMY_END;

extern int	PAWN_ISOLATED_MID;
extern int	PAWN_ISOLATED_END;
extern int	PAWN_ISOLATED_DIST_CENTER_FILE_END;
extern int	PAWN_ISOLATED_DIST_CENTER_FILE_MID;
extern int	PAWN_ISOLATED_DIST_LAST_RANK_MID;
extern int	PAWN_ISOLATED_DIST_LAST_RANK_END;

extern int	PAWN_BACKWARD_MID;
extern int	PAWN_BACKWARD_END;
extern int	PAWN_BACKWARD_DIST_CENTER_FILE_MID;
extern int	PAWN_BACKWARD_DIST_CENTER_FILE_END;
extern int	PAWN_BACKWARD_DIST_LAST_RANK_MID;
extern int	PAWN_BACKWARD_DIST_LAST_RANK_END;

extern int	PAWN_BACKWARD_PERMANENCY_MID;
extern int	PAWN_BACKWARD_PERMANENCY_END;

extern int	PAWN_BACKWARD_RESOURCE_ECONOMY_MID;
extern int	PAWN_BACKWARD_RESOURCE_ECONOMY_END;

extern int	PAWN_CANDIDATE_MID;
extern int	PAWN_CANDIDATE_END;

extern int	PAWN_CANDIDATE_DIST_LAST_RANK_MID;
extern int	PAWN_CANDIDATE_DIST_LAST_RANK_END;
extern int	PAWN_CANDIDATE_CENTER_FILE_MID;
extern int	PAWN_CANDIDATE_CENTER_FILE_END;
extern int	PAWN_PASSED_MID;
extern int	PAWN_PASSED_END;
extern int	PAWN_PASSED_DIST_LAST_RANK_MID;
extern int	PAWN_PASSED_DIST_LAST_RANK_END;
extern int	PAWN_PASSED_CENTER_FILE_MID;
extern int	PAWN_PASSED_CENTER_FILE_END;
extern int	PAWN_PASSED_MY_KING_TROPISM_END;
extern int	PAWN_PASSED_ENEMY_KING_TROPISM_END;

extern int	PAWN_PASSED_PROTECTED_MID;
extern int	PAWN_PASSED_PROTECTED_END;

extern int	PAWN_PASSED_UNSTOPPABLE_END;
extern int	PAWN_PASSED_DANGEROUS_END;

extern int	PAWN_PASSED_BLOCKED_BY_KNIGHT_MID;
extern int	PAWN_PASSED_BLOCKED_BY_KNIGHT_END;
extern int	PAWN_PASSED_BLOCKED_BY_BISHOP_MID;
extern int	PAWN_PASSED_BLOCKED_BY_BISHOP_END;
extern int	PAWN_PASSED_BLOCKED_BY_KING;

extern int	PAWN_CENTRAL_BLOCKED_MID;
extern int	PAWN_C2_BLOCKED_MID;
extern int	PAWN_ADJUST_MID;
extern int	PAWN_ADJUST_END;
extern int	PAWN_UNITED_MID;
extern int	PAWN_UNITED_END;
extern int	PAWN_ADVANCED_MID;
extern int	PAWN_ADVANCED_DIST_CENTER_FILE_MID;
extern int	PAWN_ADVANCED_DIST_LAST_RANK_MID;
extern int	PAWN_ADVANCED_END;
extern int	PAWN_ADVANCED_DIST_CENTER_FILE_END;
extern int	PAWN_ADVANCED_DIST_LAST_RANK_END;

extern int	KNIGHT_MAT_MID;
extern int	KNIGHT_PAIR_MID;
extern int	KNIGHT_PAIR_END;
extern int	KNIGHT_KING_TROPISM_MID;
extern int	KNIGHT_KING_TROPISM_END;
extern int	KNIGHT_DIST_CENTER_MID;
extern int	KNIGHT_DIST_LAST_RANK_MID;
extern int	KNIGHT_DIST_LAST_RANK_END;
extern int	KNIGHT_MAT_END;
extern int	KNIGHT_DIST_CENTER_END;
extern int	KNIGHT_ADJUST_MID;
extern int	KNIGHT_ADJUST_END;
extern int	KNIGHT_PROTECTED_BY_PAWN_MID;
extern int	KNIGHT_PROTECTED_BY_PAWN_END;
extern int	KNIGHT_MOB1_MID;
extern int	KNIGHT_MOB2_MID;
extern int	KNIGHT_MOB1_END;
extern int	KNIGHT_MOB2_END;

extern int	KNIGHT_ATTACK_WEAK_PIECE_MID;
extern int	KNIGHT_ATTACK_WEAK_PIECE_END;

extern int	KNIGHT_ATTACK_WEAK_PAWN_MID;
extern int	KNIGHT_ATTACK_WEAK_PAWN_END;

extern int	KNIGHT_SLOW_DEVELOPMENT_MID;
extern int	KNIGHT_ATTACK_KING_MID;
extern int	KNIGHT_ATTACK_KING_END;
extern int	KNIGHT_TRAPPED_A1;
extern int	KNIGHT_TRAPPED_A2;
extern int	KNIGHT_OUTPOST1_MID;
extern int	KNIGHT_OUTPOST2_MID;
extern int	KNIGHT_OUTPOST3_MID;
extern int	KNIGHT_OUTPOST4_MID;
extern int	BISHOP_MAT_MID;

extern int	BISHOP_KING_TROPISM_MID;
extern int	BISHOP_KING_TROPISM_END;

extern int	BISHOP_DIST_CENTER_MID;
extern int	BISHOP_DIST_LAST_RANK_MID;
extern int	BISHOP_DIST_LAST_RANK_END;
extern int	BISHOP_MAT_END;
extern int	BISHOP_DIST_CENTER_END;
extern int	BISHOP_SLOW_DEVELOPMENT_MID;
extern int	BISHOP_PROTECTED_BY_PAWN_MID;
extern int	BISHOP_PROTECTED_BY_PAWN_END;
extern int	BISHOP_ATTACK_KING_MID;
extern int	BISHOP_ATTACK_KING_END;

extern int	BISHOP_MOB1_MID;
extern int	BISHOP_MOB2_MID;
extern int	BISHOP_MOB3_MID;

extern int	BISHOP_MOB1_END;
extern int	BISHOP_MOB2_END;
extern int	BISHOP_MOB3_END;

extern int	BISHOP_TRAPPED_A2;
extern int	BISHOP_TRAPPED_A3;
extern int	BAD_BISHOP1_END;
extern int	BAD_BISHOP1_MID;
extern int	BAD_BISHOP2_MID;
extern int	BAD_BISHOP2_END;
extern int	BAD_BISHOP3_MID;
extern int	BAD_BISHOP3_END;
extern int	BAD_BISHOP4_MID;
extern int	BAD_BISHOP4_END;
extern int	BISHOP_PAIR_MID;
extern int	BISHOP_PAIR_END;
extern int	BISHOP_ATTACK_WEAK_PIECE_MID;
extern int	BISHOP_ATTACK_WEAK_PIECE_END;
extern int	BISHOP_ATTACK_WEAK_PAWN_MID;
extern int	BISHOP_ATTACK_WEAK_PAWN_END;

extern int	ROOK_MAT_MID;

extern int	ROOK_KING_TROPISM_MID;
extern int	ROOK_KING_TROPISM_END;

extern int	ROOK_DIST_CENTER_FILE_MID;
extern int	ROOK_MAT_END;
extern int	ROOK_DIST_CENTER_FILE_END;
extern int	ROOK_DIST_LAST_RANK_MID;
extern int	ROOK_DIST_LAST_RANK_END;

extern int	ROOK_ATTACK_KING_MID;
extern int	ROOK_ATTACK_KING_END;
extern int	ROOK_PAIR_MID;
extern int	ROOK_PAIR_END;
extern int	ROOK_ADJUST_MID;
extern int	ROOK_ADJUST_END;
extern int	ROOK_IN_OPEN_FILE_MID;
extern int	ROOK_IN_HALF_OPEN_FILE_MID;
extern int	ROOK_IN_HALF_OPEN_FILE_END;
extern int	ROOK_ATTACK_KING_OPEN_FILE_MID;
extern int	ROOK_ATTACK_KING_OPEN_FILE_END;
extern int	ROOK_ATTACK_KING_HALF_OPEN_FILE_MID;
extern int	ROOK_ATTACK_KING_HALF_OPEN_FILE_END;
extern int	ROOK_ROOK_SUPPORT_MID;
extern int	ROOK_ROOK_SUPPORT_END;
extern int	ROOK_QUEEN_SUPPORT_MID;
extern int	ROOK_QUEEN_SUPPORT_END;

extern int	ROOK_7_MID;
extern int	ROOK_7_END;
extern int	ROOK_TRAPPED_MID;
extern int	ROOK_TRAPPED_END;

extern int	ROOK_MOB1_MID;
extern int	ROOK_MOB2_MID;

extern int	ROOK_MOB1_END;
extern int	ROOK_MOB2_END;

extern int	ROOK_ATTACK_WEAK_PAWN_MID;
extern int	ROOK_ATTACK_WEAK_PAWN_END;

extern int	ROOK_ATTACK_WEAK_PIECE_MID;
extern int	ROOK_ATTACK_WEAK_PIECE_END;

extern int	QUEEN_MAT_MID;

extern int	QUEEN_KING_TROPISM_MID;
extern int	QUEEN_KING_TROPISM_END;

extern int	QUEEN_DIST_CENTER_MID;
extern int	QUEEN_DIST_LAST_RANK_MID;
extern int	QUEEN_DIST_LAST_RANK_END;
extern int	QUEEN_MAT_END;
extern int	QUEEN_DIST_CENTER_END;
extern int	QUEEN_ATTACK_KING_MID;
extern int	QUEEN_ATTACK_KING_END;

extern int	QUEEN_7_MID;
extern int	QUEEN_7_END;
extern int	QUEEN_MOB_MID;
extern int	QUEEN_MOB_END;
extern int	KING_DIST_LAST_RANK_MID;
extern int	KING_DIST_LAST_RANK_END;
extern int	KING_DIST_CASTTLE_MID;
extern int	KING_DIST_CENTER_END;
extern int	KING_TROPISM_WEAK_PAWN_END;
extern int	KING_FIANCHETTO_MID;
extern int	KING_NO_FIANCHETTO_MID;
extern int	KING_IN_CENTER_MID;
extern int	KING_CASTTLE_DONE_MID;
extern int	KING_NO_PAWN_IN_FILE;
extern int	KING_PAWN_IN_3;
extern int	KING_PAWN_IN_4;
extern int	KING_NO_PAWN_ENEMY_IN_FILE;
extern int	KING_PAWN_ENEMY_IN_3;
extern int	KING_PAWN_ENEMY_IN_4;
extern int	KING_IN_OPEN_FILE;

extern int	KING_ONLY_DEFEND_MID;
extern int	KING_WITHOUT_FRIEND_MID;
extern int	KING_WITH_ENEMY_MID;
extern int	KING_WITH_ENEMY2_MID;

extern int	ROOK_VS_MINOR_MID;
extern int	ROOK_VS_MINOR_END;
extern int	ROOK_VS_PAWN_MID;
extern int	ROOK_VS_PAWN_END;
extern int	MINOR_VS_PAWN_MID;
extern int	MINOR_VS_PAWN_END;
extern int	TWO_MINOR_VS_ROOK_MID;
extern int	TWO_MINOR_VS_ROOK_END;

extern int	PROTECTED_PIECES_MID;
extern int	PROTECTED_PIECES_END;

extern int	ROOK_COMPELLING_MID;
extern int	ROOK_COMPELLING_END;
extern int	QUEEN_COMPELLING_MID;
extern int	QUEEN_COMPELLING_END;
extern int	OWN_ROOK_BLOCK_PASSED_PAWN;
extern int	KBKB_END;
extern int	KBKB2_END;
extern int	KBBKBB_END;
extern int	KRKR_END;
extern int	KQKQ_END;
extern int	KRBKRB_END;
extern int	KRBKRB2_END;
extern int	KQBKQB_END;
extern int	KK_END;
extern int	KNKN_END;
extern int	KBNKBN_END;
extern int	KBNKBN2_END;
extern int	KNNKNN_END;
extern int	ROOK_SUPP_PASSED;
extern int	ROOK_ATTACK_PASSED;


#define		NUM_EV_PARAM	238


//====================================================================================================================================
extern	char	EvalParamPath[2048];


//====================================================================================================================================
void	LoadEvalParamConfig(char* file);						// Carga el nombre del libro a usar y su configuracion
void	LoadEvalConfig(char* file);								// Cargando configuracion de la evaluacion

//	150
