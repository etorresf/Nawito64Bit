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

#include "move_gen.h"


/*
 * Agrega un movimiento de captura o promoción a la lista de movimientos.
 * Lo ordena segun su relevancia
 */
void	PushCapture(STACK_MOVE** HMove, SQUARE From, SQUARE To, int Type)		{
	STACK_MOVE*	Move	= (*HMove);
	Move->ID			= ENCODE_MOVE(From, To, Type);


	// Buenas capturas y promociones
	if (PROMOTION(Type))
		Move->Score		= PROM_SCORE;

	else if (!BadCapture(From, To))
		Move->Score		= GOOD_CAPTURE_SCORE + PieceValue[Board[To]] - PieceValue[Board[From]] / 10;


	// Malas capturas por detrás
	else Move->Score	= BAD_CAPTURE_SCORE;


	(*HMove)++;
}


/*
 * Agrega un movimiento a la lista de movimientos.
 * Lo ordena segun su relevancia
 */
void	PushMove(STACK_MOVE** HMove, SQUARE From, SQUARE To, int Type)			{
	STACK_MOVE*	Move	= (*HMove);
	Move->ID			= ENCODE_MOVE(From, To, Type);
	int			BCapture= 0,
				ID		= GET_ID(Move->ID);


	// Buenas capturas y promociones
	if (PROMOTION(Type))
		Move->Score		= PROM_SCORE + PieceValue[Type];

	else if (Type == EN_PASSANT_CAPTURE)
		Move->Score		= GOOD_CAPTURE_SCORE;

	else if (Board[To] && !(BCapture = BadCapture(From, To)))
		Move->Score		= GOOD_CAPTURE_SCORE + PieceValue[Board[To]] - PieceValue[Board[From]] / 10;


	// Killers y matekillers
	else if (ID == MateKiller[Ply])
		Move->Score		= MATE_KILLER_SCORE;

	else if (ID == Killer1[Ply])
		Move->Score		= KILLER_SCORE;

	else if (Ply > 2 && ID == Killer1[Ply - 2])
		Move->Score		= KILLER_SCORE >> 1;

	else if (ID == Killer2[Ply])
		Move->Score		= KILLER_SCORE >> 2;

	else if (Ply > 2 && ID == Killer2[Ply - 2])
		Move->Score		= KILLER_SCORE >> 3;


	// Malas capturas
	else if (BCapture)
		Move->Score		= BAD_CAPTURE_SCORE + PieceValue[Board[To]] - PieceValue[Board[From]] / 10;


	// Los restantes movimientos son ordenados por el historial
	else Move->Score	= History[Turn][ID];


	(*HMove)++;
}


/*
 * Genera los pseudos movimientos del turno que le toca jugar.
 * Estos movimientos son guardados en 'HMove'(movimientos para el nivel actual) y se
 * retorna puntero a posicion despues del ultimo movimiento generado(proximo nivel).
 * Al igual que el motor TSCP se usa un arreglo global para la generacion de movimientos
 * teniendo como entrada el nivel o profundidad de la busqueda. Esto trae como principal
 * ventaja la velocidad, ya que por cada nivel no hace falta reservar y eliminar memoria
 * para estas listas
 */
STACK_MOVE*		MoveGen(STACK_MOVE* HMove)		{
	BITBOARD	FromBB, ToBB,
				EnemyPiecesBB	= ~AllPieceBB[Turn],
				AllPiecesBB		= ALL_PIECES_BB,
				EmptySquaresBB	= ~AllPiecesBB;
	TURN		CTurn			= FLIP_TURN(Turn);
	SQUARE		KingSQ			= KING_SQ(Turn),
				From, To;


	// Movimientos de capturas al paso
	if (EnPassant)
		for (FromBB = PAWN_ATTACK_BB(EnPassant, Turn); FromBB; FromBB ^= SquareBB[From])		{
			From	= LSB_BB(FromBB);
			PushMove(&HMove, From, EnPassant, EN_PASSANT_CAPTURE);
		}


	// Movimientos de enroque
	if (Casttle && !Attacked(KingSQ, CTurn))	{
		// Enroque	corto
		if ((Casttle & KingSideCasttle[Turn]) && !(SquareKingCasttleBB[Turn] & AllPiecesBB)		&& !Attacked(KingSQ + 1, CTurn))
			PushMove(&HMove, KingSQ, KingSQ + 2, CASTTLE);

		// Enroque	largo
		if ((Casttle & QueenSideCasttle[Turn]) && !(SquareQueenCasttleBB[Turn] & AllPiecesBB)	&& !Attacked(KingSQ - 1, CTurn))
			PushMove(&HMove, KingSQ, KingSQ - 2, CASTTLE);
	}


	// Movimientos de Caballos
	for (FromBB = KNIGHT_BB(Turn); FromBB; FromBB ^=SquareBB[From])				{
		From	= LSB_BB(FromBB);

		for (ToBB = KNIGHT_MOVES_BB(From) & EnemyPiecesBB; ToBB; ToBB ^= SquareBB[To])			{
			To	= LSB_BB(ToBB);
			PushMove(&HMove, From, To, NORMAL);
		}
	}


	// Movimientos de Alfiles y Damas
	for (FromBB = BISHOP_BB(Turn) | QUEEN_BB(Turn); FromBB; FromBB ^= SquareBB[From])			{
		From	= LSB_BB(FromBB);

		for (ToBB = BISHOP_MOVES_BB(From, AllPiecesBB) & EnemyPiecesBB; ToBB; ToBB ^= SquareBB[To])				{
			To	= LSB_BB(ToBB);
			PushMove(&HMove, From, To, NORMAL);
		}
	}


	// Movimientos de Torres y Damas
	for (FromBB = ROOK_BB(Turn) | QUEEN_BB(Turn); FromBB; FromBB ^= SquareBB[From])				{
		From	= LSB_BB(FromBB);

		for (ToBB = ROOK_MOVES_BB(From, AllPiecesBB) & EnemyPiecesBB; ToBB; ToBB ^= SquareBB[To])				{
			To	= LSB_BB(ToBB);
			PushMove(&HMove, From, To, NORMAL);
		}
	}


	// Movimientos de Rey
	for (ToBB = KING_MOVES_BB(KingSQ) & EnemyPiecesBB; ToBB; ToBB ^= SquareBB[To])				{
		To	= LSB_BB(ToBB);
		PushMove(&HMove, KingSQ, To, NORMAL);
	}


	// Movimientos de Peones
	for (FromBB = PAWN_BB(Turn); FromBB; FromBB ^= SquareBB[From])				{
		From	= LSB_BB(FromBB);
		ToBB	= PawnAdvanceBB[Turn][From] & EmptySquaresBB;


		// Movimiento de avance de 2 pasos
		if (ToBB && (PawnAdvance2BB[Turn][From] & EmptySquaresBB))
			PushMove(&HMove, From, From + 2 * Front[Turn], PAWN_PUSH_2_SQUARE);

		for (ToBB |= (PawnAttackBB[Turn][From] & AllPieceBB[CTurn]); ToBB; ToBB ^= SquareBB[To]){
			To	= LSB_BB(ToBB);

			// Movimientos de promocion
			if ((RANK_1_BB | RANK_8_BB) & SquareBB[To])		{
				PushMove(&HMove, From, To, QUEEN	| Turn);
				PushMove(&HMove, From, To, KNIGHT	| Turn);
				PushMove(&HMove, From, To, ROOK		| Turn);
				PushMove(&HMove, From, To, BISHOP	| Turn);
			}

			// Movimientos de captura y avance de 1 paso
			else PushMove(&HMove, From, To, NORMAL);
		}
	}


	return HMove;
}


// Genera los movimientos de captura y promociones. Básicamente una copia de MoveGen
STACK_MOVE*		CaptureGen(STACK_MOVE* HMove)	{
	SQUARE		From, To,
				KingSQ			= KING_SQ(Turn);
	TURN		CTurn			= FLIP_TURN(Turn);
	BITBOARD	EnemyPiecesBB	= AllPieceBB[CTurn],
				AllPiecesBB		= ALL_PIECES_BB,
				EmptySquaresBB	= ~AllPiecesBB,
				FromBB, ToBB;


	// Capturas y promocion con Peones
	for (FromBB = PAWN_BB(Turn); FromBB; FromBB ^= SquareBB[From])				{
		From	= LSB_BB(FromBB);
		ToBB	= (PawnAdvanceBB[Turn][From] & EmptySquaresBB & (RANK_1_BB | RANK_8_BB))
				| (PawnAttackBB[Turn][From]  & EnemyPiecesBB);


		for (; ToBB; ToBB ^= SquareBB[To])		{
			To	= LSB_BB(ToBB);

			// Movimientos de promocion
			if ((RANK_1_BB | RANK_8_BB) & SquareBB[To])
				PushCapture(&HMove, From, To, QUEEN | Turn);

			// Movimientos de captura
			else PushCapture(&HMove, From, To, NORMAL);
		}
	}

	// Capturas al paso
	if (EnPassant)
		for (FromBB = PAWN_ATTACK_BB(EnPassant, Turn); FromBB; FromBB ^= SquareBB[From])		{
			From	= LSB_BB(FromBB);
			PushCapture(&HMove, From, EnPassant, EN_PASSANT_CAPTURE);
		}


	// Capturas con Caballos
	for (FromBB = KNIGHT_BB(Turn); FromBB; FromBB ^= SquareBB[From])			{
		From	= LSB_BB(FromBB);

		for (ToBB = KNIGHT_MOVES_BB(From) & EnemyPiecesBB; ToBB; ToBB ^= SquareBB[To])			{
			To	= LSB_BB(ToBB);
			PushCapture(&HMove, From, To, NORMAL);
		}
	}


	// Capturas con Alfiles y Damas
	for (FromBB = BISHOP_BB(Turn) | QUEEN_BB(Turn); FromBB; FromBB ^= SquareBB[From])			{
		From	= LSB_BB(FromBB);

		for (ToBB = EnemyPiecesBB & BISHOP_MOVES_BB(From, AllPiecesBB); ToBB; ToBB ^= SquareBB[To])				{
			To	= LSB_BB(ToBB);
			PushCapture(&HMove, From, To, NORMAL);
		}
	}


	// Capturas con Torres y Damas
	for (FromBB = ROOK_BB(Turn) | QUEEN_BB(Turn); FromBB; FromBB ^= SquareBB[From])				{
		From	= LSB_BB(FromBB);

		for (ToBB = EnemyPiecesBB & ROOK_MOVES_BB(From, AllPiecesBB); ToBB; ToBB ^= SquareBB[To])				{
			To	= LSB_BB(ToBB);
			PushCapture(&HMove, From, To, NORMAL);
		}
	}


	// Capturas con Rey
	for (ToBB = KING_MOVES_BB(KingSQ) & EnemyPiecesBB; ToBB; ToBB ^= SquareBB[To])				{
		To	= LSB_BB(ToBB);
		PushCapture(&HMove, KingSQ, To, NORMAL);
	}

	return HMove;
}


// Lo contrario a hacer un movimiento
void	UnMakeMove(void)		{
	TURN		CTurn		= Turn;
	SAWP_TURN(Turn);
	Nmove--;
	Ply--;


	HIST_MOVE*	HistPtr		= &Moves[Nmove];
	SQUARE		From		= GET_FROM(HistPtr->Move),
				To			= GET_TO(HistPtr->Move);
	int			Type		= GET_MOVE_TYPE(HistPtr->Move);
	PIECE		Captured	= HistPtr->Capture;

	Casttle		= HistPtr->Casttle;
	EnPassant	= HistPtr->EnPassant;
	Rule50		= HistPtr->Rule50;
	Hash		= HistPtr->Hash;


	PieceBB[Board[To]]	^= SquareBB[To];


	if (PROMOTION(Type))		{
		Board[From]		 = PAWN | Turn;

		// Actualiza datos referente al material
		PieceNum[PAWN | Turn]++;
		PieceNum[Type]--;

		MatSum			+= PAWN_VALUE - PieceValue[Type];
		PieceMat[Turn]	-= PieceValue[Type];
		PawnMat[Turn]	+= PAWN_VALUE;
		HashMat			+= HashMatKey[PAWN | Turn] - HashMatKey[Type];
	}

	else		{
		// Mueve la torre a su lugar de origen en caso de enroque
		if (Type == CASTTLE)
			switch (To)			{
				case G1:
					Board[H1]			 = WHITE_ROOK;
					Board[F1]			 = EMPTY_PIECE;
					PieceBB[WHITE_ROOK]	^= F1_H1_BB;
					AllPieceBB[WHITE]	^= F1_H1_BB;
					break;
				case C1:
					Board[A1]			 = WHITE_ROOK;
					Board[D1]			 = EMPTY_PIECE;
					PieceBB[WHITE_ROOK]	^= A1_D1_BB;
					AllPieceBB[WHITE]	^= A1_D1_BB;
					break;
				case G8:
					Board[H8]			 = BLACK_ROOK;
					Board[F8]			 = EMPTY_PIECE;
					PieceBB[BLACK_ROOK]	^= F8_H8_BB;
					AllPieceBB[BLACK]	^= F8_H8_BB;
					break;
				case C8:
					Board[A8]			 = BLACK_ROOK;
					Board[D8]			 = EMPTY_PIECE;
					PieceBB[BLACK_ROOK]	^= A8_D8_BB;
					AllPieceBB[BLACK]	^= A8_D8_BB;
					break;
			default:;
			}

		Board[From] = Board[To];

		if (Board[From] >= KING)
			KingSquare[Turn] = From;
	}


	PieceBB[Board[From]]	^= SquareBB[From];
	AllPieceBB[Turn]		^= (SquareBB[From] | SquareBB[To]);
	Board[To]				= EMPTY_PIECE;


	if (Captured)				{
		// Corrige donde esta la pieza capturada en caso de captura al paso
		if (Type == EN_PASSANT_CAPTURE)
			To ^= 8;

		Board[To] = Captured;


		// Actualiza datos referente al material
		MatSum	+= PieceValue[Captured];
		PieceNum[Captured]++;

		if (NOT_PAWN(Captured))
			PieceMat[CTurn]	+= PieceValue[Captured];

		else PawnMat[CTurn]	+= PAWN_VALUE;

		HashMat += HashMatKey[Captured];

		PieceBB[Captured]	^= SquareBB[To];
		AllPieceBB[CTurn]	^= SquareBB[To];
	}
}


// Se encarga de realizar un movimiento. Devuelve 1 si el movimiento es legal, sino 0
U8		MakeMove(MOVE Move)		{
	SQUARE			From			= GET_FROM(Move),
					To				= GET_TO(Move);
	int				Type			= GET_MOVE_TYPE(Move);
	TURN			CTurn			= FLIP_TURN(Turn);
	HIST_MOVE*		HistPtr			= &Moves[Nmove];
	CASTTLE_RIGHT	OldCasttle		= HistPtr->Casttle		= Casttle;
	PIECE			Captured		= HistPtr->Capture		= Board[To];
	SQUARE			OldEnPassant	= HistPtr->EnPassant	= EnPassant;

	HistPtr->Move					= Move;
	HistPtr->Rule50					= Rule50;
	HistPtr->Hash					= Hash;


	// Mueve la torre en caso de enroque
	if (Type == CASTTLE)
		switch (To)				{
			case G1:
				Board[F1]			 = WHITE_ROOK;
				Board[H1]			 = EMPTY_PIECE;
				PieceBB[WHITE_ROOK]	^= F1_H1_BB;
				AllPieceBB[WHITE]	^= F1_H1_BB;
				Hash				^= HashPiece[WHITE_ROOK][H1] ^ HashPiece[WHITE_ROOK][F1];
				break;
			case C1:
				Board[D1]			 = WHITE_ROOK;
				Board[A1]			 = EMPTY_PIECE;
				PieceBB[WHITE_ROOK]	^= A1_D1_BB;
				AllPieceBB[WHITE]	^= A1_D1_BB;
				Hash				^= HashPiece[WHITE_ROOK][A1] ^ HashPiece[WHITE_ROOK][D1];
				break;
			case G8:
				Board[F8]			 = BLACK_ROOK;
				Board[H8]			 = EMPTY_PIECE;
				PieceBB[BLACK_ROOK]	^= F8_H8_BB;
				AllPieceBB[BLACK]	^= F8_H8_BB;
				Hash				^= HashPiece[BLACK_ROOK][H8] ^ HashPiece[BLACK_ROOK][F8];
				break;
			case C8:
				Board[D8]			 = BLACK_ROOK;
				Board[A8]			 = EMPTY_PIECE;
				PieceBB[BLACK_ROOK]	^= A8_D8_BB;
				AllPieceBB[BLACK]	^= A8_D8_BB;
				Hash				^= HashPiece[BLACK_ROOK][A8] ^ HashPiece[BLACK_ROOK][D8];
				break;
			default:;
		}

	// Borra el peón si captura al_paso
	else if (Type == EN_PASSANT_CAPTURE)		{
		Captured		= HistPtr->Capture = Board[To ^ 8];
		Board[To ^ 8]	= EMPTY_PIECE;
	}


	Hash				^= HashPiece[Board[From]][From];
	PieceBB[Board[From]]^= SquareBB[From];


	// Actualiza la regla de los 50 movimientos
	if (Captured || Board[From] < KNIGHT)
		Rule50 = 0;

	else Rule50++;


	if (PROMOTION(Type))		{
		Board[To]		 = Type;

		// Actualiza datos referente al material
		PieceNum[PAWN | Turn]--;
		PieceNum[Type]++;

		MatSum			+= PieceValue[Type] - PAWN_VALUE;
		PieceMat[Turn]	+= PieceValue[Type];
		PawnMat[Turn]	-= PAWN_VALUE;
		HashMat			+= HashMatKey[Type] - HashMatKey[PAWN | Turn];
	}

	else{
		Board[To]		 = Board[From];

		if (Board[To] >= KING)
			KingSquare[Turn] = To;
	}


	PieceBB[Board[To]]	^= SquareBB[To];
	AllPieceBB[Turn]	^= (SquareBB[From] | SquareBB[To]);
	Board[From]			 = EMPTY_PIECE;


	if (Captured)				{
		// Corrige donde esta la pieza capturada en caso de captura al paso
		if (Type == EN_PASSANT_CAPTURE)
			To	^= 8;


		// Actualiza datos referente al material
		MatSum	-= PieceValue[Captured];
		PieceNum[Captured]--;


		if (NOT_PAWN(Captured))
			PieceMat[CTurn]	-= PieceValue[Captured];

		else PawnMat[CTurn]	-= PAWN_VALUE;


		HashMat				-= HashMatKey[Captured];
		Hash				^= HashPiece[Captured][To];

		PieceBB[Captured]	^= SquareBB[To];
		AllPieceBB[CTurn]	^= SquareBB[To];
	}


	if (Casttle)
		Casttle	&= CasttleMask[From] & CasttleMask[To];

	Hash ^= HashPiece[Board[To]][To] ^ HashTurn[WHITE] ^ HashCasttle[OldCasttle] ^ HashCasttle[Casttle] ^ HashEnPassant[OldEnPassant];


	/*
	 * Actualiza hash en caso de captura al paso, la casilla de captura debe estar atacada por
	 * peones para mantener la compatibilidad con polyglot
	 */
	EnPassant = 0;
	if (Type == PAWN_PUSH_2_SQUARE && PAWN_ATTACK_BB(To ^ 8, CTurn))			{
		EnPassant	= To ^ 8;
		Hash		^= HashEnPassant[EnPassant];
	}


	Ply++;
	Nmove++;
	SAWP_TURN(Turn);


	if (InCheck(FLIP_TURN(Turn)))				{
		UnMakeMove();
		return 0;
	}

	return 1;
}


/*
 * Retorna el número de movimientos legales(<= 'MaxNumLegals'). Se debe dejar a los punteros que indican
 * donden empiezan los movimientos para cada Ply en el mismo lugar, de lo contrario puede probocar errores
 */
int		CountLegals(int MaxNumLegals)			{
	STACK_MOVE	*FirstPtr, *LastPtr,
				*HPtr	= FirstMove[Ply + 1];
	int			Count	= 0;


	LastPtr = FirstMove[Ply + 1] = MoveGen(FirstMove[Ply]);

	for (FirstPtr = FirstMove[Ply]; FirstPtr < LastPtr; FirstPtr++)
		if (MakeMove(FirstPtr->ID))				{
			UnMakeMove();

			if (++Count >= MaxNumLegals)
				break;
		}


	FirstMove[Ply + 1] = HPtr;
	return Count;
}


/*
 * Permite conocer el tipo de movimiento que llega. Pasa por referencia la
 * posición del ultimo carácter analizado de ´c´
 */
int		CheckMoveType(SQUARE From, SQUARE* To, char* s, int* l)	{
	*l			= 4;


	// Detectando movimiento de enroque. !!En formato 'polyglot' se mueve rey a: H1, H8, A1, A8!!, hay que ajustarlo
	if (Board[From] >= KING)	{
		if (s[0] == 'e' && (s[1] == '1' || s[1] == '8') && (s[2] == 'g' || s[2] == 'c' || s[2] == 'h' || s[2] == 'a') && (s[3] == '1' || s[3] == '8'))			{

			// Se ajusta
			if (s[2] == 'h')
				*To = From + 2;

			else if (s[2] == 'a')
				*To = From - 2;

			return CASTTLE;
		}
	}


	if (NOT_PAWN(Board[From]))
		return 0;

	if (EnPassant && *To == EnPassant)
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
STACK_MOVE		CheckMove(char* s, int* l)		{
	*l					= 0;
	SQUARE		From	= (s[0] - 'a') + 8 * (s[1] - '1'),
				To		= (s[2] - 'a') + 8 * (s[3] - '1');
	int			Sum		= 0,
				Size	= 0;
	STACK_MOVE	m		= {0, 0},
				*First, *End;
	char*		c;
	MOVE		ID;


	// Con estos caracteres no empiezan los movimientos
	if ((int)*s < 32)
		return m;

	if (From < A1 || From > H8 || To < A1 || To > H8 || !Board[From])
		return m;


	// Obtiene el tipo de movimiento
	ID		= ENCODE_MOVE(From, To, CheckMoveType(From, &To, s, l));
	End		= FirstMove[1];


	// Comprueba si es legal realizar el movimiento
	for (First = FirstMove[0]; First != End; First++)
		if (First->ID == ID)	{
			if (MakeMove(First->ID))			{
				UnMakeMove();
				m.ID = First->ID;
			}
			break;
		}


	for (c = s + *l; *c == ' '; c++, Size++);					// Consume espacios


	// Asigna la evaluación del movimiento
	if (*c == '=')				{
		for (Size++, c++; *c == ' '; c++, Size++);

		for (; *c >= '0' && *c <= '9'; c++, Size++)
			Sum = Sum * 10 + (*c - '0');
	}


	m.Score	= Sum;
	*l		+= Size;
	return	m;
}


// Comprueba si un movimiento es legal
STACK_MOVE		CheckOneMove(char* s, int* l)	{
	FirstMove[1]	= MoveGen(FirstMove[0]);
	return CheckMove(s, l);
}


// Comprueba si varios movimientos son legales para almacenarlos en 'MoveList'
long	CheckMoves(char* s, STACK_MOVE* MoveList, long* l)		{
	int			ListSize	= 0,
				i			= 0;
	char*		c			= s;
	STACK_MOVE	Move;


	FirstMove[1] = MoveGen(FirstMove[0]);

	while (1){
		for (; *c == ' ' || *c == ','; c++);					// Consume los espacios y separador

		// Romper si se encuentra el final
		if (*c == '\0' || *c == '\n' || *c == ';')
			break;


		// Chequea que sea legal para añadirlo a 'MoveList'
		Move = CheckMove(c, &ListSize);
		c += ListSize;

		if (Move.ID)
			MoveList[i++] = Move;


		while (*c != ' ' && *c != ',' && *c != ';' && *c != '\0' && *c != '\n')
			c++;
	}


	*l = (c - s) + 1;
	return i;
}


// Carga un archivo en formato EPD. No se reconocen todos los campos de esta notación
long	LoadEPD(char* EPD, STACK_MOVE* MoveList){
	long		l;
	return		CheckMoves(EPD + LoadFEN(EPD), MoveList, &l);
}


// 700
