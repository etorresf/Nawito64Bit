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


#include "move_gen.h"



// Agrega un movimiento a la lista de movimientos
void	PushMove(STACK_MOVE** move_list, SQUARE from, SQUARE to, int type)		{
	(*move_list)->ID	= ENCODE_MOVE(from, to, type);
	(*move_list)++;
}


/*
 * Genera pseudos movimientos(capturas) del turno que toca jugar. Estos movimientos 
 * son guardados en 'MoveList', el movimiento vacio marca el final de la lista
 */
void	MoveGen(TURN side, STACK_MOVE* move_list, U8 only_capture)				{
	BITBOARD	from_bb, to_bb, enemy_pieces_bb,
				all_pieces_bb	= ALL_PIECES_BB;
	TURN		cside			= FLIP_TURN(side);
	SQUARE		king_sq			= KING_SQ(side),
				from, to;
	int			front			= Front[side];


	if (only_capture)
		enemy_pieces_bb		= AllPieceBB[cside];

	else enemy_pieces_bb	= ~AllPieceBB[side];


	// Movimientos de capturas al paso
	if (EnPassant)
		for (from_bb = PAWN_ATTACK_BB(EnPassant, side); from_bb; from_bb ^= SquareBB[from])		{
			from	= LSB_BB(from_bb);
			PushMove(&move_list, from, EnPassant, EN_PASSANT_CAPTURE);
		}


	// Movimientos de enroque
	if (Casttle && !(only_capture || Attacked(king_sq, cside)))	{
		// Enroque corto
		if ((Casttle & KingSideCasttle[side]) && !(KingCasttleBB[side] & all_pieces_bb)	&& !Attacked(king_sq + 1, cside))
			PushMove(&move_list, king_sq, king_sq + 2, CASTTLE);

		// Enroque largo
		if ((Casttle & QueenSideCasttle[side]) && !(QueenCasttleBB[side] & all_pieces_bb)	&& !Attacked(king_sq - 1, cside))
			PushMove(&move_list, king_sq, king_sq - 2, CASTTLE);
	}


	// Movimientos de Caballos
	for (from_bb = KNIGHT_BB(side); from_bb; from_bb ^=SquareBB[from])			{
		from	= LSB_BB(from_bb);

		for (to_bb = KNIGHT_MOVES_BB(from) & enemy_pieces_bb; to_bb; to_bb ^= SquareBB[to])		{
			to	= LSB_BB(to_bb);
			PushMove(&move_list, from, to, NORMAL);
		}
	}


	// Movimientos de Alfiles y Damas
	for (from_bb = BISHOP_BB(side) | QUEEN_BB(side); from_bb; from_bb ^= SquareBB[from])		{
		from	= LSB_BB(from_bb);

		for (to_bb = BISHOP_MOVES_BB(from, all_pieces_bb) & enemy_pieces_bb; to_bb; to_bb ^= SquareBB[to])		{
			to	= LSB_BB(to_bb);
			PushMove(&move_list, from, to, NORMAL);
		}
	}


	// Movimientos de Torres y Damas
	for (from_bb = ROOK_BB(side) | QUEEN_BB(side); from_bb; from_bb ^= SquareBB[from])			{
		from	= LSB_BB(from_bb);

		for (to_bb = ROOK_MOVES_BB(from, all_pieces_bb) & enemy_pieces_bb; to_bb; to_bb ^= SquareBB[to])		{
			to	= LSB_BB(to_bb);
			PushMove(&move_list, from, to, NORMAL);
		}
	}


	// Movimientos de Rey
	for (to_bb = KING_MOVES_BB(king_sq) & enemy_pieces_bb; to_bb; to_bb ^= SquareBB[to])		{
		to	= LSB_BB(to_bb);
		PushMove(&move_list, king_sq, to, NORMAL);
	}


	// Capturas y promocion con Peones
	if (only_capture)
		for (from_bb = PAWN_BB(side); from_bb; from_bb ^= SquareBB[from])		{
			from	= LSB_BB(from_bb);
			to_bb	= (PawnAdvanceBB[side][from] & (~all_pieces_bb) & (RANK_1_BB | RANK_8_BB))
					| (PawnAttackBB[side][from]  & enemy_pieces_bb);


			for (; to_bb; to_bb ^= SquareBB[to]){
				to	= LSB_BB(to_bb);

				if ((RANK_1_BB | RANK_8_BB) & SquareBB[to])
					PushMove(&move_list, from, to, QUEEN | side);// Movimientos de promocion

				else PushMove(&move_list, from, to, NORMAL);	// Movimientos de captura
			}
		}

	// Movimientos de Peones
	else for (from_bb = PAWN_BB(side); from_bb; from_bb ^= SquareBB[from])		{
		from	= LSB_BB(from_bb);
		to_bb	= PawnAdvanceBB[side][from] & (~all_pieces_bb);


		// Movimiento de avance de 2 pasos
		if (to_bb && (PawnAdvance2BB[side][from] & (~all_pieces_bb)))
			PushMove(&move_list, from, from + 2 * front, PAWN_PUSH_2_SQUARE);

		for (to_bb |= (PawnAttackBB[side][from] & AllPieceBB[cside]); to_bb; to_bb ^= SquareBB[to])				{
			to	= LSB_BB(to_bb);

			// Movimientos de promocion
			if ((RANK_1_BB | RANK_8_BB) & SquareBB[to])			{
				PushMove(&move_list, from, to, QUEEN	| side);
				PushMove(&move_list, from, to, KNIGHT	| side);
				PushMove(&move_list, from, to, ROOK		| side);
				PushMove(&move_list, from, to, BISHOP	| side);
			}

			// Movimientos de captura y avance de 1 paso
			else PushMove(&move_list, from, to, NORMAL);
		}
	}


	move_list->ID = EMPTY_MOVE;
}


// Lo contrario a hacer un movimiento. Incluye deshacer movimiento nulo
void	UnMakeMove(void)		{
	SAWP_TURN(Turn);
	Nmove--;
	Ply--;

	int			move_type;
	SQUARE		from, to;
	PIECE		piece_captured;
	HIST_MOVE*	hist_ptr	= &Moves[Nmove];
	Casttle					= hist_ptr->Casttle;
	EnPassant				= hist_ptr->EnPassant;
	Rule50					= hist_ptr->Rule50;
	Hash					= hist_ptr->Hash;


	// Movimientos no nulos
	if (hist_ptr->Move)			{
		from			= GET_FROM(hist_ptr->Move);
		to				= GET_TO(hist_ptr->Move);
		move_type		= GET_MOVE_TYPE(hist_ptr->Move);
		piece_captured	= hist_ptr->Capture;


		// Si promocion cambiar la pieza por Peon
		if (PROMOTION(move_type))				{
			PutPiece(from, PAWN | Turn, 0);
			QuitPiece(to, 0);
		}
		else MovePiece(to, from, 0);


		// Si captura, poner la pieza capturada en el tablero
		if (piece_captured)		{
			if (move_type == EN_PASSANT_CAPTURE)
				PutPiece(to ^ 8, piece_captured, 0);

			else PutPiece(to, piece_captured, 0);
		}


		// Si enroque, retornar a la torre
		if (move_type == CASTTLE)
			switch (to)			{
				case G1:	MovePiece(F1, H1, 0);	break;
				case C1:	MovePiece(D1, A1, 0);	break;
				case G8:	MovePiece(F8, H8, 0);	break;
				case C8:	MovePiece(D8, A8, 0);	break;
			default:;
			}
	}
}


/*
 * Se encarga de realizar un movimiento. Devuelve 1 si el movimiento es legal, sino 0.
 * Incluye hacer movimiento nulo
 */
U8		MakeMove(MOVE move)		{
	int				move_type;
	SQUARE			from, to, old_enpassant;
	CASTTLE_RIGHT	old_casttle;
	HIST_MOVE*		hist_ptr		= &Moves[Nmove];

	hist_ptr->Move					= move;
	hist_ptr->Rule50				= Rule50;
	hist_ptr->Hash					= Hash;
	hist_ptr->Casttle				= Casttle;
	hist_ptr->EnPassant				= EnPassant;
	hist_ptr->Capture				= EMPTY_PIECE;


	// Movimientos no nulos
	if (move)	{
		old_casttle		= Casttle;
		move_type		= GET_MOVE_TYPE(move);
		from			= GET_FROM(move);
		to				= GET_TO(move);
		old_enpassant	= EnPassant;


		// Actualiza la regla de los 50 movimientos
		if (Board[to] || Board[from] < KNIGHT)
			Rule50 = 0;

		else Rule50++;


		//	Quita la pieza capturada
		if (Board[to])			{
			hist_ptr->Capture	= Board[to];
			QuitPiece(to, 1);
		}
		else if (move_type == EN_PASSANT_CAPTURE)				{
			hist_ptr->Capture	= Board[to ^ 8];
			QuitPiece(to ^ 8, 1);
		}


		// Pone la pieza en la casilla destino, incluyendo las promociones
		if (PROMOTION(move_type))				{
			PutPiece(to, move_type, 1);
			QuitPiece(from, 1);
		}
		else MovePiece(from, to, 1);


		// Mueve la torre en caso de enroque
		if (move_type == CASTTLE)
			switch (to)				{
				case G1:	MovePiece(H1, F1, 1);	break;
				case C1:	MovePiece(A1, D1, 1);	break;
				case G8:	MovePiece(H8, F8, 1);	break;
				case C8:	MovePiece(A8, D8, 1);	break;
				default:;
			}


		if (Casttle)
			Casttle	&= CasttleMask[from] & CasttleMask[to];

		Hash ^= HashTurn[WHITE] ^ HashCasttle[old_casttle] ^ HashCasttle[Casttle] ^ HashEnPassant[old_enpassant];


		/*
		 * Actualiza hash en caso de captura al paso, la casilla de captura debe estar atacada por
		 * peones para mantener la compatibilidad con polyglot
		 */
		if (move_type == PAWN_PUSH_2_SQUARE && PAWN_ATTACK_BB(to ^ 8, FLIP_TURN(Turn)))			{
			EnPassant	= to ^ 8;
			Hash		^= HashEnPassant[EnPassant];
		}
		else EnPassant = 0;
	}

	// Movimiento nulo
	else		{
		Rule50++;												// Actualiza la regla de los 50 movimientos
		Hash ^= HashTurn[WHITE];

		if (EnPassant)
			Hash		^= HashEnPassant[EnPassant];

		else EnPassant	= 0;
	}


	Ply++;
	Nmove++;
	SAWP_TURN(Turn);


	/*
	 * No estar en jaque despues de hacer el movimiento.
	 * No necesario con movimiento nulo ya que se chequea anteriormente
	 */
	if (move && InCheck(FLIP_TURN(Turn)))		{
		UnMakeMove();
		return 0;
	}

	return 1;
}


/*
 * Retorna el número de movimientos legales(<= 'MaxNumLegals'). Se debe dejar a los punteros que indican
 * donden empiezan los movimientos para cada Ply en el mismo lugar, de lo contrario puede probocar errores
 */
int		CountLegals(TURN side, int max_num_legal)				{
	STACK_MOVE	*move, TMPmoves[256];
	int			count	= 0;


	MoveGen(side, TMPmoves, 0);

	for (move = TMPmoves; move->ID; move++)
		if (MakeMove(move->ID))	{
			UnMakeMove();

			if (++count >= max_num_legal)
				break;
		}


	return count;
}


/*
 * Permite conocer el tipo de movimiento que llega. Pasa por 
 * referencia la posición del ultimo carácter analizado de ´c´
 */
int		CheckMoveType(SQUARE from, SQUARE* to, char* s, int* l)	{
	*l			= 4;


	// Detectando movimiento de enroque. !!En formato 'polyglot' se mueve rey a: H1, H8, A1, A8!!, hay que ajustarlo
	if (Board[from] >= KING)	{
		if (s[0] == 'e' && (s[1] == '1' || s[1] == '8') && (s[2] == 'g' || s[2] == 'c' || s[2] == 'h' || s[2] == 'a') && (s[3] == '1' || s[3] == '8'))			{

			// Se ajusta
			if (s[2] == 'h')
				*to = from + 2;

			else if (s[2] == 'a')
				*to = from - 2;

			return CASTTLE;
		}
	}


	if (NOT_PAWN(Board[from]))
		return 0;

	if (EnPassant && *to == EnPassant)
		return EN_PASSANT_CAPTURE;

	if ((s[1] == '2' && s[3] == '4') || (s[1] == '7' && s[3] == '5'))
		return PAWN_PUSH_2_SQUARE;


	// Se pone el tipo de promoción
	*l = 5;
	switch (s[4])				{
		case 'q':	return	QUEEN	| Turn;
		case 'r':	return	ROOK	| Turn;
		case 'b':	return	BISHOP	| Turn;
		case 'n':	return	KNIGHT	| Turn;
		default:	*l = 4;	return 0;
	}
}


/*
 * Dado un movimiento(formato SAN) determina si es legal o no. Retornando el
 * movimiento si es legal, en otro caso retorna un movimiento vacío.
 */
MOVE		CheckMove(char* s, int* l)		{
	*l					= 0;
	SQUARE		from	= (s[0] - 'a') + 8 * (s[1] - '1'),
				to		= (s[2] - 'a') + 8 * (s[3] - '1');
	MOVE		m		= EMPTY_MOVE,
				ID;
	STACK_MOVE	*move, TMPmoves[256];


	if ((int)*s < 32 || from < A1 || from > H8 || to < A1 || to > H8 || !Board[from])
		return m;


	MoveGen(Turn, TMPmoves, 0);
	ID	= ENCODE_MOVE(from, to, CheckMoveType(from, &to, s, l));// Obtiene el tipo de movimiento


	// Comprueba si es legal realizar el movimiento
	for (move = TMPmoves; move->ID; move++)
		if (move->ID == ID)		{
			if (MakeMove(move->ID))				{
				UnMakeMove();
				m	= move->ID;
			}
			break;
		}


	return	m;
}


// 400
