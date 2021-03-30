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


#include "search.h"



U8				WindowFlag;
U8				InCheckRoot;
int				LegalMovesRoot;

STATE			SearchState;
int				Cknodes;
U8				CurrentAge;
int				DepthNull;

MOVE			PV[MAX_DEPHT + 1][MAX_DEPHT + 1];
int				PVLenght[MAX_DEPHT + 1];

char			Command[256];
char			Line[256];

int				Value1[MAX_DEPHT_1];

double			LMRtable[64][256];



// Decrementa el historial de los movimientos en '2 * Dec'
void	HistoryAge(U8 dec)		{
	unsigned		i;

	MaxHistory[BLACK] >>= dec;
	MaxHistory[WHITE] >>= dec;


	for (i = 1; i < 4095; i++)	{
		History[BLACK][i] >>= dec;
		History[WHITE][i] >>= dec;
	}
}


// Actualiza la variante principal con un movimiento
void	UpdatePV(MOVE move)		{
	int		i;

	PV[Ply][Ply] = move;


	for (i = Ply + 1; i < PVLenght[Ply + 1]; i++)
		PV[Ply][i]	= PV[Ply + 1][i];

	PVLenght[Ply]	= PVLenght[Ply + 1];
}


// Pone en primer lugar el movimiento con mayor valor
void	SortMove(STACK_MOVE* stack)				{
	STACK_MOVE	*first, m,
				*best		= stack;

	for (first = stack + 1; first->ID; first++)
		if (first->Score > best->Score)
			best = first;


	if (best != stack)			{
		m			= *(stack);
		*(stack)	= *(best);
		*(best)		= m;
	}
}


// Extensiones
int		Extensions(U8 PV_node, U8 check_mate_threat)			{
	HIST_MOVE*	actual_move		= &(Moves[Nmove - 1]),
				*previous_move	= &(Moves[Nmove - 2]);


	if (check_mate_threat)
		return HALF_PLY << PV_node;								// Amenaza de MATE


	// Se da jaque 
	if (InCheck(Turn))			{
		if (CountLegals(Turn, 2) < 2)
			return PLY << PV_node;								// Movimiento Forzado

		return HALF_PLY << PV_node;
	}


	if (PV_node && actual_move->Capture)		{
		// Entrando en un final de peones, idea del motor Rebel. Si se entra a final de peones pensar mas
		if (MatSum < END_GAME_MATERIAL && !(PieceMat[BLACK] || PieceMat[WHITE]) && PawnEndgameExt[actual_move->Capture])
			return PawnEndgameExt[actual_move->Capture];

		// Recaptura
		if (previous_move->Capture && (GET_TO(actual_move->Move) == GET_TO(previous_move->Move)) && (PieceValue[actual_move->Capture] <= PieceValue[previous_move->Capture]))
			return PLY;
	}

	return 0;
}


// Da valor a los movimientos para ordenarlos en PVS y PVSroot
void	SetMoveScore(STACK_MOVE* move_list, MOVE hash_move, MOVE PV_move)		{
	STACK_MOVE	*move;
	int			move_type, bad_capture;
	SQUARE		from, to;
	STATE		follow_PV	= SearchState & FOLLOW_PV;

	if (follow_PV)
		SearchState	^= FOLLOW_PV;


	for(move = move_list; move->ID; move++)		{
		from		= GET_FROM(move->ID);
		to			= GET_TO(move->ID);
		move_type	= GET_MOVE_TYPE(move->ID);
		bad_capture	= 0;


		// Buenas capturas y promociones
		if (PROMOTION(move_type))
			move->Score	= PROM_SCORE + PieceValue[move_type];

		else if (move_type == EN_PASSANT_CAPTURE)
			move->Score	= GOOD_CAPTURE_SCORE;

		else if (Board[to] && !(bad_capture = SEE(from, to, 1) < 0))
			move->Score	= GOOD_CAPTURE_SCORE + PieceValue[Board[to]] - PieceValue[Board[from]] / 10;

		// Killers y matekillers
		else if (move->ID == MateKiller[Ply])
			move->Score	= MATE_KILLER_SCORE;

		else if (move->ID == Killer1[Ply])
			move->Score	= KILLER_SCORE;

		else if (Ply > 2 && move->ID == Killer1[Ply - 2])
			move->Score	= KILLER_SCORE >> 1;

		else if (move->ID == Killer2[Ply])
			move->Score	= KILLER_SCORE >> 2;

		else if (Ply > 2 && move->ID == Killer2[Ply - 2])
			move->Score	= KILLER_SCORE >> 3;

		// Malas capturas
		else if (bad_capture)
			move->Score	= BAD_CAPTURE_SCORE + PieceValue[Board[to]] - PieceValue[Board[from]] / 10;

		// Los restantes movimientos son ordenados por el historial
		else move->Score= History[Turn][GET_ID(move->ID)];


		// Movimiento de la Variante Principal(PV)
		if (follow_PV && move->ID == PV_move)	{
			SearchState	|= FOLLOW_PV;
			move->Score	|= FOLLOW_PV_SCORE;
		}

		// Movimiento de tablas hash
		if (move->ID == hash_move)
			move->Score	|= HASH_SCORE;
	}
}


// Da valor a los movimientos para ordenarlos en PVS y PVSroot
void	SetQMoveScore(STACK_MOVE* move_list, MOVE PV_move)		{
	STACK_MOVE	*move;
	int			move_type;
	SQUARE		from, to;
	STATE		follow_PV	= SearchState & FOLLOW_PV;

	if (follow_PV)
		SearchState	^= FOLLOW_PV;


	for(move = move_list; move->ID; move++)		{
		from		= GET_FROM(move->ID);
		to			= GET_TO(move->ID);
		move_type	= GET_MOVE_TYPE(move->ID);


		// Buenas capturas y promociones
		if (PROMOTION(move_type))
			move->Score	= PROM_SCORE;

		// Malas capturas para QuiesenceSearch
		else if (SEE(from, to, 1) < 0)
			move->Score	= BAD_CAPTURE_SCORE;

		// Buenas capturas
		else move->Score= GOOD_CAPTURE_SCORE + PieceValue[Board[to]] - PieceValue[Board[from]] / 10;


		// Movimiento de la Variante Principal(PV)
		if (follow_PV && move->ID == PV_move)	{
			SearchState	|= FOLLOW_PV;
			move->Score	|= FOLLOW_PV_SCORE;
		}
	}
}


#if defined (_WIN32) || defined(_WIN64)
	#ifndef WINDOWS
		#define WINDOWS
	#endif
	#include <windows.h>
#else
	#include <sys/time.h>
	#include <string.h>
//	struct timeval tv;
//	struct timezone tz;
#endif

/*
 * La siguiente función permite salir de una búsqueda si llega una interrupción por parte del GUI o
 * permite saber cuando tenemos que dejar de ponderar y empezar a pensar, esta función está basada en
 * el programa Olithink y hay cantidad de programas que la usan
 */
int		Bioskey(void)			{
	#ifndef WINDOWS
		fd_set readfds;
		FD_ZERO (&readfds);
		FD_SET (fileno(stdin), &readfds);
		tv.tv_sec = 0; tv.tv_usec=0;
		select(16, &readfds, 0, 0, &tv);

		return (FD_ISSET(fileno(stdin), &readfds));
	#else
		static int init = 0, pipe;
		static HANDLE inh;
		DWORD dw;

		if (!init)				{
			init = 1;
			inh = GetStdHandle(STD_INPUT_HANDLE);
			pipe = !GetConsoleMode(inh, &dw);

			if (!pipe)			{
				SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
				FlushConsoleInputBuffer(inh);
			}
		}
		if (pipe)				{
			if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL))
				return 1;
			return dw;
		}
		else	{
			GetNumberOfConsoleInputEvents(inh, &dw);
			return dw <= 1 ? 0 : dw;
		}
	#endif
}


// Comprueba si es necesario detener la búsqueda
STATE	Checkup(int score)		{
	int		time_used	= ClockTime() - TimeLeft;


	if ((EngineState & SEARCHING) && (SearchState & ABORT))		{
		if (Bioskey())			{
			EngineTurn = EMPTY_TURN;
			return SearchState |= STOP;
		}

		if (!WindowFlag && ((time_used >= TimeLimit) || (time_used > NoNewMov && score + MARGEN > Value1[Li - 1])))
			return SearchState |= STOP;
	}

	else if (EngineState & PONDERING)			{
		if (Bioskey())
			return SearchState |= STOP;
	}

	else if (EngineState & ANALYZING)			{
		if (Bioskey())			{
			if (!fgets(Line, 256, stdin))
				return SearchState;

			sscanf(Line, "%s", Command);						// Recibiendo comando desde la interfaz

			if (!strcmp(Command, "."))
				return SearchState ^= (SearchState & STOP);

			if (!strcmp(Command, "exit"))
				return SearchState |= STOP;
		}

		// Tiempo agotado
		if (!WindowFlag && time_used >= TimeLimit)
			return SearchState |= STOP;
	}

	return SearchState;
}


/*
 * Formula de ruduccion logaritmica basada en 'Mini Rodent 1.0'.
 * Creo que 'Mini Rodent 1.0' se basa en 'Stockfish' para crear estas formulas.
 */
void	IniLMRtable(void)		{
	int depth, moves;


	for (depth = 0; depth < MAX_DEPHT_1; depth++)				{
		for (moves = 0; moves < 256; moves++){
			LMRtable[depth][moves] = (0.33 + log((double)(depth)) * log((double)(moves)) / 2.25);	// zw node


			if (depth < 2 || moves <= 3)
				LMRtable[depth][moves] = 0;						// Puede ir en la misma busqueda, pero con ponerlo aqui se gana algo de velocidad

			else if (LMRtable[depth][moves] < 1)
				LMRtable[depth][moves] = 0;						// ultra-small reductions make no sense

			else{
				LMRtable[depth][moves] += 0.5;

				if (LMRtable[depth][moves] > depth - 1)
					LMRtable[depth][moves] = depth - 1;			// reduction cannot exceed actual depth
			}

			LMRtable[depth][moves] *= (double)PLY;				// Necesario ajustar, en la busqueda se usa PLY fraccional
		}
	}
}


// Guarda los movimientos en hash y en caso de mATE ajusta su valor
void	StoreTT(MOVE move, int score, int depth, U8 type)		{
	if (score >= MATE_SCORE)
		StoreHash(move, score + Ply, depth, type, Hash, CurrentAge);

	else if (score <= -MATE_SCORE)
		StoreHash(move, score - Ply, depth, type, Hash, CurrentAge);

	else StoreHash(move, score, depth, type, Hash, CurrentAge);
}


// Busca capturas y promociones, no lleva profundidad. Sirve para evitar el efecto horizonte
int		QuiesenceSearch(int alpha, int beta, int depth)			{
	U8			in_check;
	STACK_MOVE	*move				= MoveStack[Ply];
	int			score				= alpha,
				best_score			= -MATE,
				static_eval			= 0,
				delta_prunning_val	= 0,
				legal_moves			= 0;


	// Comprueba cada un cierto numero de nodos que tenemos tiempo
	if (!((++Nodes) % Cknodes) && (Checkup(score) & STOP))
		return 0;

	// Mate-distance pruning
	alpha	= MAX(alpha, Ply - MATE);
	beta	= MIN(beta, MATE - Ply + 1);

	if (alpha >= beta)
		return alpha;


	// Hoja del árbol de búsqueda, se procede a evaluar
	if (Ply & MAX_DEPHT_1)
		return Eval(alpha, beta);


	// La posición se repite o no existe material suficiente para ganar. Se valora como tablas
	if (RepeatPos(2) || MatInsuf())
		return MatSum / ContemFactor[Turn & EngineTurn];


	PVLenght[Ply]	= Ply;
	in_check			= InCheck(Turn);


	// Generando los capturas para la posición, en caso de estar en jaque se generan evasiones
	if (in_check)				{
		MoveGen(Turn, move, 0);
		SetMoveScore(move, EMPTY_MOVE, PV[0][Ply]);				// Pone 1ro los movimientos de la variante principal
	}

	else{
		best_score	= static_eval = Eval(alpha, beta);
		alpha		= MAX(alpha, best_score);

		if (alpha >= beta)
			return alpha;

		// delta prunning
		if (MatSum > MID_GAME_MATERIAL)
			delta_prunning_val	= alpha - 200 - static_eval;

		else if (MatSum > END_GAME_MATERIAL)
			delta_prunning_val	= alpha - 500 - static_eval;


		MoveGen(Turn, move, 1);
		SetQMoveScore(move, PV[0][Ply]);						// Pone 1ro los movimientos de la variante principal
	}


	// Recorre las capturas de la posición
	for (; move->ID; move++)	{
		SortMove(move);


		if (!in_check )			{
			if (best_score > -MATE_SCORE && !PROMOTION(move->ID) && (PieceValue[Board[GET_TO(move->ID)]] < delta_prunning_val))
				continue;										// delta pruning

			if (move->Score & BAD_CAPTURE_SCORE)
				continue;										// see pruning
		}


		if (depth <= QS_LIMITE && !in_check)
			score = static_eval + SEE(GET_FROM(move->ID), GET_TO(move->ID), 0);			// Previene qsearch exploxion

		else	{
			if (!MakeMove(move->ID))
				continue;

			legal_moves++;
			score = -QuiesenceSearch(-beta, -alpha, depth - 1);
			UnMakeMove();
		}


		if (SearchState & STOP)
			return 0;


		if (score > best_score)	{
			best_score = score;

			if (score > alpha)	{
				alpha = score;

				if (score >= beta)
					return best_score;
			}
		}
	}


	if (in_check && !legal_moves)
		return Ply - MATE;

	return best_score;
}


/*
 * Algoritmo alphabeta encargado de la búsqueda, basado en un algoritmo de
 * Bruce Moreland conocido como variante principal
 */
int		PVS(int alpha, int beta, int depth, U8 make_null_move)	{
	U8			hash_move_type, in_check,
				hash_flag			= HASF_ALPHA,
				we_can_cut			= 0,
				check_mate_threat	= 0,
				PV_node				= (beta - alpha) > 1;
	int			score				= alpha,
				legal_moves			= 0,
				best_score			= Ply - MATE,
				eval_score, extension, reduction, r_window, hash_score, hash_depth;
	MOVE		best_move			= EMPTY_MOVE,
				hash_move			= EMPTY_MOVE;
	STACK_MOVE	*move				= MoveStack[Ply];


	// Si la profundidad es menor a PLY o se ha ido muy profundo, se llama al buscador de capturas y promociones
	if (depth < PLY)
		return QuiesenceSearch(alpha, beta, 0);


	// Comprueba cada un cierto numero de nodos que tenemos tiempo
	if (!((++Nodes) % Cknodes) && (Checkup(score) & STOP))
		return 0;


	// Mate-distance pruning
	alpha	= MAX(alpha, Ply - MATE);
	beta	= MIN(beta, MATE - Ply + 1);

	if (alpha >= beta)
		return alpha;


	if (Ply & MAX_DEPHT_1)
		return Eval(-MATE, MATE);


	// La posición se repite o no existe material suficiente para ganar. Se valora como tablas
	if (RepeatPos(2) || MatInsuf())
		return MatSum / ContemFactor[Turn & EngineTurn];


	// Comprueba las tablas hash
	ProbeHash(&hash_move, Nmove + depth, &hash_move_type, Hash, &hash_depth, &hash_score, CurrentAge);
	if (hash_move_type & HASF_PRUNING)			{
		// Ajusta el valor en caso de MATE
		if (hash_score >= MATE_SCORE)
			hash_score -= Ply;

		else if (hash_score <= -MATE_SCORE)
			hash_score += Ply;


		if (hash_move_type & HASF_EXACT)		{
			if (!PV_node)
				return hash_score;
		}
		else if (hash_move_type & HASF_ALPHA)	{
			if (hash_score <= alpha)
				return alpha;
		}
		else if (hash_move_type & HASF_BETA)	{
			if (hash_score >= beta)
				return beta;
		}
	}


	/*
	 * A la hora de cortar el árbol es necesario:
	 *
	 * 1) No estar en jaque a quien le toque mover
	 * 2) No estar en un movimiento de la variante principal
	 * 3) No estar Beta cerca de un valor de MATE
	 */
	in_check = InCheck(Turn);
	if (!(in_check || PV_node) && beta < MATE_SCORE)			{
		eval_score = Eval(-MATE, MATE);

		// Razoring
		if (!hash_move && (depth <= PLY3) && (eval_score + RazoringMargen[depth] <= alpha))		{
			if (depth <= PLY)
				return MAX(QuiesenceSearch(alpha, beta, 0), eval_score + RazoringMargen[depth]);

			r_window	= alpha - RazoringMargen[depth];
			score		= QuiesenceSearch(r_window, r_window + 1, 0);

			if (score <= r_window)
				return score;
		}


		if (PieceMat[Turn])		{
			// Static null move prunning
			if (depth < PLY7 && (eval_score >= beta + ReverseFutilityMargen[depth]))
				return eval_score;


			DepthNull = DepthNullTable[depth >> 1];

			if ((make_null_move && depth > PLY && eval_score >= beta) &&
			!((hash_move_type & HASF_HIT) && (hash_move_type & HASF_ALPHA) && (hash_score < beta) && ((int)hash_depth >= DepthNull + Nmove)))	{
				MakeMove(EMPTY_MOVE);							// Realiza movimiento nulo

				if (DepthNull < PLY)
					score	= -QuiesenceSearch(-beta, 1 - beta, 0);

				else score	= -PVS(-beta, 1 - beta, DepthNull, 0);


				UnMakeMove();									// Deshace movimiento nulo

				if (SearchState & STOP)
					return 0;


				if (score >= beta)				{
					if (MatSum < END_GAME_MATERIAL)				{
						depth -= PLY5;

						if (depth < PLY)						// Null reduction
							return QuiesenceSearch(alpha, beta, 0);
					}
					else		{
						StoreTT(best_move, beta, Nmove + depth, HASF_BETA);

						if (score >= MATE_SCORE)
							score = beta;						// No devuelve un mate

						return score;							// Cutoff
					}
				}
				else check_mate_threat = score <= -MATE / 2 && beta > -MATE / 2;		// Detecta si se recibe una amenaza de MATE
			}
		}

		// Comprueba si es posible hacer futility prunning
		if (depth < PLY4 && alpha > -MATE_SCORE)
			we_can_cut = eval_score + Margen[depth / PLY] < beta;
	}


	// Internal Iterative Deepening
	if (!hash_move && (depth >= PLY7 || (PV_node && depth >= PLY4)))			{
		score	= PVS(alpha, beta, depth - PLY2, 1);
		ProbeHash(&hash_move, Nmove + depth, &hash_move_type, Hash, &hash_depth, &hash_score, CurrentAge);
	}


	// Generando los movimientos para la posición
	PVLenght[Ply] = Ply;
	MoveGen(Turn, move, 0);


	SetMoveScore(move, hash_move, PV[0][Ply]);					// Ordena los movimientos: 1ro hash, 2do variante principal


	// Recorre los movimientos de la posición
	for (; move->ID; move++)	{
		SortMove(move);

		if (!MakeMove(move->ID))
			continue;


		legal_moves++;
		extension	= Extensions(PV_node, check_mate_threat);


		if (legal_moves == 1)
			score	= -PVS(-beta, -alpha, depth + extension - PLY, 1);	// Realiza una búsqueda completa para el primer movimiento

		else	{
			// Futility prunning
			if (we_can_cut && !extension && move->Score < FUTILITY_PRUNNING_SCORE)				{
				UnMakeMove();
				continue;
			}


			// LMR prunning en las terminaciones
			if (legal_moves > 12 && depth < PLY2 && !(in_check || PV_node || extension) && alpha > -MATE / 2 &&
			(legal_moves > 20 || move->Score < FUTILITY_PRUNNING_SCORE))		{
				UnMakeMove();
				continue;
			}


			/*
			 * Prueba reducir la profundidad (LMR) si:
			 *
			 * 1) No hay extensiones
			 * 2) Hay un numero máximo de movimientos legales y se tiene una determinada profundidad
			 * 3) El valor del movimiento no es grande
			 * 4) No es el movimiento de la tabla hash, variante principal, killers o matekiller
			 *
			 * La formula de ruduccion logaritmica esta basada en la de sotckfish
			 */
			reduction = (int)LMRtable[depth / PLY][legal_moves];
			if (reduction && !(extension || in_check || PV_node) && alpha > -MATE / 2 && beta < MATE / 2 && move->Score < BAD_CAPTURE_SCORE)
				score	= -PVS(-alpha - 1, -alpha, depth - reduction, 1);

			else score	= alpha + 1;							// Asegura que una búsqueda completa sea hecha


			if (score > alpha)	{
				score	= -PVS(-alpha - 1, -alpha, depth + extension - PLY, 1);

				if (!(SearchState & STOP) && score > alpha && score < beta)
					score= -PVS(-beta, -alpha, depth + extension - PLY, 1);
			}
		}


		UnMakeMove();

		if (SearchState & STOP)
			return 0;


		if (score > best_score)	{
			best_score	= score;
			best_move	= move->ID;


			if (score > alpha)	{
				alpha		= score;
				hash_flag	= HASF_EXACT;

				UpdatePV(best_move);								// Actualiza la variante principal


				// Actualiza la historia para los movimientos tranquilos
				if (!(extension || in_check) && (move->Score < BAD_CAPTURE_SCORE))				{
					History[Turn][GET_ID(best_move)] += (depth * depth) / PLY4;

					if (History[Turn][GET_ID(best_move)] > MaxHistory[Turn])
						MaxHistory[Turn] = History[Turn][GET_ID(best_move)];

					if (MaxHistory[Turn] >= BAD_CAPTURE_SCORE)
						HistoryAge(1);
				}


				if (score >= beta)				{
					// Movimientos matekiller y killer son buenos, aprovechamos para guardar información de ellos
					if (!(move->Score & (GOOD_CAPTURE_SCORE | PROM_SCORE)))		{
						if (score > MATE_SCORE)
							MateKiller[Ply] = best_move;

						else if ((best_move != Killer1[Ply]) && !(move->Score & MATE_KILLER_SCORE))				{
							Killer2[Ply] = Killer1[Ply];
							Killer1[Ply] = best_move;
						}
					}

					StoreTT(best_move, beta, Nmove + depth, HASF_BETA);
					return best_score;							// Cutoff del alphabeta
				}
			}
		}
	}


	if (legal_moves)			{
		if (Rule50 >= 100)
			return MatSum / ContemFactor[Turn & EngineTurn];	// Tablas por repeticion
	}
	else if (in_check)
		best_score = Ply - MATE;								// Mate

	else best_score = MatSum / ContemFactor[Turn & EngineTurn];	// Ahogado


	StoreTT(best_move, best_score, Nmove + depth, hash_flag);
	return best_score;
}


// Muestra el movimiento seleccionado, profundidad, puntuación, tiempo empleado, nodos examinados
void	PrintPV(int depth, int score)			{
	int		i	= 0,
			j	= PVLenght[Ply] ;


	if (!(j && (EngineConfig & POST)))
		return;


	if (EngineState & PONDERING)
		score = -score;											//	Se esta ponderando es necesario cambiar  el signo de la evaluacion

	printf("%d %d %d %d ", depth, score, (ClockTime() - TimeLeft) / 10, Nodes);	// Envía profundidad, puntos, tiempo(centisegundos), nodos


	// Imprime la mejor linea
	for (i = 0; (i < j) && PV[0][i]; i++)		{
		PrintMove(PV[0][i]);
		printf(" ");
	}


	printf("\n");												// Indica el fin de linea
	fflush(stdout);												// Obligado por el protocolo xboard
}


// Similar a PVS con algunos datos precalculados
int		PVSroot(int alpha, int beta, int depth)	{
	int			extension,
				score			= 0,
				best_score		= -MATE;
	U8			hash_flag		= HASF_ALPHA;
	MOVE		best_move		= EMPTY_MOVE;
	STACK_MOVE	*move			= MoveStack[0];


	LegalMovesRoot		= 0;
	SearchState			|= FOLLOW_PV;

	Nodes++;
	MoveGen(Turn, move, 0);
	SetMoveScore(move, EMPTY_MOVE, PV[0][Ply]);					// Pone 1ro los movimientos de la variante principal


	for (; move->ID; move++)	{
		SortMove(move);

		if (!MakeMove(move->ID))
			continue;


		LegalMovesRoot++;
		extension		= Extensions(1, 0);
		score	= -PVS(-beta, -alpha, depth + extension - PLY, 1);


		UnMakeMove();
		if (SearchState & STOP)
			return 0;


		if (score > best_score)	{
			best_score	= score;
			best_move	= move->ID;


			if (score > alpha)	{
				alpha		= score;
				hash_flag	= HASF_EXACT;

				UpdatePV(best_move);							// Actualiza la variante principal


				// Actualiza la historia para los movimientos tranquilos
				if (!(extension || InCheckRoot) && (move->Score < BAD_CAPTURE_SCORE))			{
					History[Turn][GET_ID(best_move)] += (depth * depth) / PLY4;

					if (History[Turn][GET_ID(best_move)] > MaxHistory[Turn])
						MaxHistory[Turn] = History[Turn][GET_ID(best_move)];

					if (MaxHistory[Turn] >= BAD_CAPTURE_SCORE)
						HistoryAge(1);
				}


				if (score >= beta)				{
					// Movimientos matekiller y killer son buenos, aprovechamos para guardar información de ellos
					if (!(move->Score & (GOOD_CAPTURE_SCORE | PROM_SCORE)))		{
						if (score > MATE_SCORE)
							MateKiller[Ply] = best_move;

						else if ((best_move != Killer1[Ply]) && !(move->Score & MATE_KILLER_SCORE))				{
							Killer2[Ply] = Killer1[Ply];
							Killer1[Ply] = best_move;
						}
					}

					StoreTT(best_move, beta, Nmove + depth, HASF_BETA);
					return best_score;
				}


				if (Nodes > Noise)
					PrintPV(Li, score);							// Mostrar valoración
			}
		}
	}


	if (LegalMovesRoot)			{
		if ((LegalMovesRoot == 1) && !(EngineState & ANALYZING))
			SearchState |= STOP;								// Un solo 1 movimiento, hacerlo de inmediato
	}

	else if (InCheckRoot)
		best_score	= Ply - MATE;								// Mate

	else best_score	= MatSum / ContemFactor[Turn & EngineTurn];	// Ahogado


	StoreTT(best_move, best_score, Nmove + depth, hash_flag);
	return best_score;
}


// Retorna el mejor movimiento para una profundidad, usando de forma iterativa al algoritmo alphabeta.
MOVE	EngineThink(void)		{
	TimeLeft	= ClockTime();
	Li			= 0;
	MOVE		best_move;
	int			i,
				alpha_window	= 16,
				beta_window		= 16,
				alpha			= -MATE,
				beta			= MATE;


	// Inicializa algunas variables
	CurrentAge		= (U8)((Nmove - 1) >> 2);
	best_move		= 0;
	WindowFlag		= Nodes = 0;
	SearchState		^= (SearchState & (ABORT | STOP));


	if (MatSum < END_GAME_MATERIAL)
		Cknodes		= 0x3FF;

	else Cknodes	= 0x3FFF;


	// Elimina los datos de los Killers, MateKillers y PV
	memset(PV,			0, sizeof(PV));
	memset(MateKiller,	0, sizeof(MateKiller));
	memset(Killer1,		0, sizeof(Killer1));
	memset(Killer2,		0, sizeof(Killer2));


	HistoryAge(3);												// Actualiza la historia

	InCheckRoot = InCheck(Turn);
	PVLenght[0] = Ply = 0;
	MoveGen(Turn, MoveStack[0], 0);


	/*
	 * Búsqueda de forma iterativa, tiene 2 ventajas:
	 *
	 * 1) Si el tiempo se termina al menos se tiene una búsqueda completa en una profundidad.
	 * 2) Cada profundidad se puede resolver mas rápido que la anterior debido al uso de las
	 *	tablas hash y a una mejor ordenación de la variante principal
	 */
	for (i = 1; i <= MaxDepth;)	{
		if ((WindowFlag & 16) || (SearchState & STOP))
			break;

		Li			= i;

		// Para las primeras profundidades se realiza una búsqueda completa
		if (i <= PLY)			{
			alpha	= -MATE;
			beta	= MATE;
		}


		Value1[i]	= PVSroot(alpha, beta, i * PLY);			// Profundidad fraccional, permite un mejor control
		best_move	= PV[0][0];


		// Se abren nuevas ventanas si se produce un fallo en la búsqueda
		if (Value1[i] <= alpha)	{
			if (alpha_window & 0xFE00)
				alpha	= -MATE;

			else alpha	= Value1[i] - (alpha_window <<= 1);

			WindowFlag++;
			continue;
		}

		if (Value1[i] >= beta)	{
			if (beta_window & 0xFE00)
				beta	= MATE;

			else beta	= Value1[i] + (beta_window <<= 1);

			WindowFlag++;
			continue;
		}


		if ((EngineState & SEARCHING) && i > PLY){				// Si la cosa es clara, reducir el tiempo de búsqueda
			SearchState |= ABORT;								// Se tiene una profundidad máxima para abandonar la búsqueda

			if (ClockTime() - TimeLeft > NoNewIteration)
				break;											// No comenzar nueva iteración si ya ha consumido al menos la mitad de tiempo
		}


		// Configura las ventanas para el siguiente ciclo
		alpha_window	= beta_window = 16;
		alpha			= Value1[i] - alpha_window;
		beta			= Value1[i] + beta_window;

		i++;
		WindowFlag = 0;
	}

	return best_move;
}


// Si es final de partida indica el resultado de la misma
void	PrintResult(void)		{
	Ply	= 0;


	// Existen movimientos legales
	if (CountLegals(Turn, 1))	{
		if (RepeatPos(3))
			printf("1/2-1/2 {Draw by repetition}\n");			// Tablas por repeticion de posicion 3 veces

		else if (Rule50 >= 100)
			printf("1/2-1/2 {Draw by 50 move rule}\n");			// Tablas por regla de los 50 movimientos

		else if (MatInsuf())
			printf("1/2-1/2 {Draw by insufficient material}\n");// Tablas por insuficiencia material
	}

	// No existen movimientos legales
	else if (InCheck(Turn))		{
		if (Turn == WHITE)
			printf("0-1 {White mates}\n");						// Jaque Mate al Rey Blanco

		else printf("1-0 {Black mates}\n");						// Jaque Mate al Rey Negro
	}
	else printf("1/2-1/2 {Stalemate}\n");						// Tablas por ahogado
}


// 900
