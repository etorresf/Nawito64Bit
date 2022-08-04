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

#include "search.h"


//====================================================================================================================================
int				InCheckRoot;

int				Cknodes;
int				Li;

MOVE			PV[MAX_DEPHT * MAX_DEPHT];
int				PVLenght[MAX_DEPHT];

int				Value1[MAX_DEPHT];

int				LMRtable[MAX_DEPHT][256];

int				UpdateInfoUCI	= 0;
int				FailUCI			= 0;

int				Nodes;

STACK_MOVE		MoveStack[MAX_DEPHT * 256];

MOVE			Killer1[MAX_DEPHT];
MOVE			Killer2[MAX_DEPHT];
MOVE			MateKiller[MAX_DEPHT];



//====================================================================================================================================
// Decrementa el historial de los movimientos en '2 * Dec' para el turno 'side'
void	HistoryAge(TURN side, int dec)			{
	int		i,
			*history	= History[side];

	for (i = HISTORY_SIZE; i; i--)				{
		(*history) >>= dec;
		history++;
	}
}


// Actualiza la historia para los movimientos tranquilos
void	UpdateHistory(MOVE move, int depth)		{
	int*	history		= History[Turn],
			ID			= GET_ID(move);

	history[ID]			+= (depth * depth) / PLY4;

	if (history[ID] > history[MAX_HISTORY])		{
		history[MAX_HISTORY] = history[ID];

		if (history[MAX_HISTORY] >= BAD_CAPTURE_SCORE)
			HistoryAge(Turn, 1);
	}
}


// Actualiza la variante principal con un movimiento
void	UpdatePV(MOVE move)		{
	int		pv_size		= PVLenght[Ply + 1] - Ply - 1;
	MOVE	*pTarget	= PV_INDEX(Ply),
			*pSource	= PV_INDEX(Ply + 1);

	PVLenght[Ply]	= PVLenght[Ply + 1];
	*pTarget		= move;
	pTarget++;

	while (pv_size-- && (*pTarget++ = *pSource++));
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


// Da valor a los movimientos para ordenarlos en 'PVS', 'PVSroot', o 'QuiesenceSearch'
void	SetMoveScore(STACK_MOVE* move_list, MOVE hash_move, int qmove)			{
	STACK_MOVE	*move;
	int			move_type, see_value;
	SQUARE		from, to;
	STATE		follow_PV	= Engine & FOLLOW_PV;
	MOVE		PV_move		= PV[Ply];


	if (follow_PV)
		Engine		^= FOLLOW_PV;

	else PV_move	= EMPTY_MOVE;

	for(move = move_list; move->ID; move++)		{
		from		= GET_FROM(move->ID);
		to			= GET_TO(move->ID);
		move_type	= GET_MOVE_TYPE(move->ID);
		see_value	= 0;

		// Promociones
		if (PROMOTION(move_type))
			move->Score	= PROM_SCORE | (PieceValue[move_type] << 10);

		// Buenas capturas
		else if (move_type == EN_PASSANT_CAPTURE)
			move->Score	= GOOD_CAPTURE_SCORE;

		else if (Board[to] && (see_value = SEE(from, to, 1) >= 0))
			move->Score	= GOOD_CAPTURE_SCORE | (PieceValue[Board[to]] - PieceValue[Board[from]] / 10);

		// Movimientos de "QuiesenceSearch", solo buenas o malas capturas
		else if (qmove)
			move->Score	= BAD_CAPTURE_SCORE;

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
		else if (see_value < 0)
			move->Score	= BAD_CAPTURE_SCORE | (PieceValue[Board[to]] - PieceValue[Board[from]] / 10);

		// Los restantes movimientos son ordenados por el historial
		else move->Score= History[Turn][GET_ID(move->ID)];

		// Movimiento de la Variante Principal(PV)
		if (move->ID == PV_move){
			move->Score	|= FOLLOW_PV_SCORE;
			Engine		|= FOLLOW_PV;
		}

		// Movimiento de tablas hash
		if (move->ID == hash_move)
			move->Score	|= HASH_SCORE;
	}
}


// Extensiones
int		Extensions(int PV_node, int check_mate_threat, int negative_SEE)		{
	HIST_MOVE	*actual_move	= &(Moves[Nmove - 1]);
	SQUARE		to				= GET_TO(actual_move->Move);


	// Amenaza de MATE y Jaque extension
	if (!negative_SEE && (check_mate_threat || IN_CHECK(Turn)))
		return HALF_PLY	<< PV_node;

	if (PV_node && actual_move->Capture)		{
		// Entrando a final de PEONES extension. Incrementar segun sea mayor la PIEZA capturada (Idea de [Rebel])
		if (!(PieceMat[BLACK] || PieceMat[WHITE]) && PawnEndgameExt[actual_move->Capture])
			return PawnEndgameExt[actual_move->Capture];

		// Recaptura extension
		if ((actual_move - 1)->Capture && !negative_SEE && (to == GET_TO((actual_move - 1)->Move)))
			return PLY;
	}

	// Extension de avance de PEON a 6ta o 7ma
	if (Board[to] < KNIGHT)
		switch (RANK(to + Front[Turn ^ 1]))		{
			case RANK_1: case RANK_8:	return HALF_PLY		  << PV_node;
			case RANK_2: case RANK_7:	return (HALF_PLY / 2) << PV_node;
			default:;
		}

	return 0;
}


#if defined (_WIN32) || defined(_WIN64)
	#ifndef WINDOWS
		#define WINDOWS
	#endif
	#include <windows.h>
#else
	#include <sys/time.h>
	#include <string.h>
#endif

/*
 * La siguiente función permite salir de una búsqueda si llega una interrupción por parte del GUI o
 * permite saber cuando tenemos que dejar de ponderar y empezar a pensar, esta función está basada en
 * el programa [Olithink] y hay cantidad de programas que la usan
 */
int		Bioskey()				{
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


// Obtiene que comando llega en caso de haber actuado la funcion Bioskey()
void	ReadLine(char *str, int n)				{
	char	*ptr;

	if (fgets(str, n, stdin) == NULL)
		exit(0);

	if ((ptr = strchr(str, '\n')) != NULL)
		*ptr = '\0';
}


// Comprueba si es necesario detener la búsqueda
STATE	StopSearch(int score)	{
	char	input_line[80];

	switch (Engine & (SEARCHING | ANALYZING | LEARNING))		{
		case SEARCHING:
			if (Engine & ABORT)	{
				// tiempo limite sobrepasado, salir de la busqueda 
				if (TIME_EXC())
					return	Engine	|= STOP;

				// tiempo para buscar nuevo movimiento agotado,
				// salir de la busqueda si evaluacion del actual PLY no se ha reducido en un MARGEN
				if (score + MARGEN >= Value1[Li - 1] && MOVE_TIME_EXC())
					return	Engine	|= STOP;

				if (Bioskey())	{
					ReadLine(input_line, sizeof(input_line));

					// comando stop, detener busqueda
					if (!strcmp(input_line, "stop"))
						return	Engine	|= STOP;

					// comando isready
					if (!strcmp(input_line, "isready"))
						printf("readyok\n");

					// comando quit, abandonar busqueda y salir del motor
					else if (!strcmp(input_line, "quit"))		{
						printf("Quitting during think, bye\n");
						exit(0);
					}
				}
			}
			return 0;
		case ANALYZING:
			if (Bioskey())		{
				ReadLine(input_line, sizeof(input_line));

				// comando stop, detener analisis 
				if (!strcmp(input_line, "stop"))
					return	Engine	|= STOP;

				// comando isready
				if (!strcmp(input_line, "isready"))
					printf("readyok\n");

				// comando quit, abandonar busqueda y salir del motor
				else if (!strcmp(input_line, "quit"))			{
					printf("Quitting during analyze, bye\n");
					exit(0);
				}
			}

			// terminamos si hemos sobrepasado el tiempo limite aunque en la practica no ocurrira ya que es casi infinito 
			else if (TIME_EXC())
				return	Engine	|= STOP;
			return 0;
		case LEARNING:
			return 0;
		default: return 0;
	}
}


// Formula de ruduccion logaritmica basada en [Mini Rodent 1.0]
void	IniLMRtable()			{
	int		depth, moves;
	double	lmr_val;


	for (depth = 0; depth < MAX_DEPHT; depth++)
		for (moves = 0; moves < 256; moves++)	{
			lmr_val		= 0.32 + (log((double)(depth)) * log((double)(moves))) / 2.24;	// zw node

			// ultra-small reductions make no sense
			// Puede ir en la misma busqueda, pero con ponerlo aqui se gana algo de velocidad
			if (lmr_val < 1.0 || depth < 2 || moves <= 3)
				lmr_val		= 0;

			// reduction cannot exceed actual depth
			else lmr_val = MIN(lmr_val + 0.5, depth - 1);

			// Se ajusta a PLY, en la busqueda se usa PLY fraccional
			LMRtable[depth][moves]	= (int)(lmr_val * (double)PLY);
		}
}


// Inicializa los valores necesarios para efectuar la busqueda
void	IniSearch()				{
		IniLMRtable();
}


// Busca capturas y promociones, no lleva profundidad. Sirve para evitar el efecto horizonte
int		QuiesenceSearch(int alpha, int beta, int depth)			{
	int			in_check,
				score			= alpha,
				best_score		= -MATE,
				static_eval		= 0,
				delta_prunning	= 0,
				legal_moves		= 0;
	STACK_MOVE	*move			= STACK_INDEX(Ply);


	// Comprueba cada un cierto numero de nodos que tenemos tiempo
	Nodes++;
	if (Cknodes && !(Nodes % Cknodes) && StopSearch(score))
		return 0;

	// Mate-distance pruning
	alpha	= MAX(alpha, Ply - MATE);
	beta	= MIN(beta, MATE - Ply + 1);

	if (alpha >= beta)
		return alpha;

	// Posición tablas
	if (MatInsuf() || RepeatPos(2))
		return DRAW_VALUE(Turn);

	// Hoja del árbol de búsqueda, se procede a evaluar
	if (Ply >= MAX_DEPHT)
		return Eval(alpha, beta);

	// Generando los capturas para la posición, en caso de estar en jaque se generan evasiones
	in_check		= IN_CHECK(Turn);
	if (!in_check)	{
		best_score	= static_eval = Eval(alpha, beta);

		alpha		= MAX(alpha, best_score);

		if (alpha >= beta)
			return alpha;

		if (GAME_PHASE > 85)
			delta_prunning	= alpha - TAPERED_SCORE(200, 650) - static_eval;
	}

	PVLenght[Ply] = Ply;
	MoveGen(Turn, move, !in_check);
	SetMoveScore(move, EMPTY_MOVE, !in_check);					// Pone 1ro los movimientos de la variante principal

	// Recorre las capturas de la posición
	for (; move->ID; move++)	{
		SortMove(move);

		if (!in_check)			{
			if (move->Score & BAD_CAPTURE_SCORE)
				continue;										// see pruning

			if (best_score > -MATE_SCORE && !PROMOTION(move->ID) && (PieceValue[Board[GET_TO(move->ID)]] < delta_prunning))
				continue;										// delta pruning
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

		if (Engine & STOP)
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


// Guardar información acerca los movimientos killer y matekiller
void	SetKillers(MOVE move, int score, int sort_score)		{
	if (score > MATE_SCORE)
		MateKiller[Ply]	= move;

	else if ((move != Killer1[Ply]) && !(sort_score & MATE_KILLER_SCORE))	{
		Killer2[Ply]	= Killer1[Ply];
		Killer1[Ply]	= move;
	}
}


// Algoritmo alphabeta encargado de la búsqueda, basado en uno de Bruce Moreland conocido como variante principal(PVS)
int		PVS(int alpha, int beta, int depth, int make_null_move)	{
	U8			hash_move_type;
	int			in_check, eval_score, extension, reduction, r_window, 
				hash_score, hash_depth, depth_null, is_prunnable,
				hash_type			= HASF_ALPHA,
				can_prune_FP		= 0,
				check_mate_threat	= 0,
				legal_moves			= 0,
				PV_node				= (beta - alpha) > 1,
				score				= alpha,
				best_score			= Ply - MATE;
	MOVE		best_move			= EMPTY_MOVE,
				hash_move			= EMPTY_MOVE;
	STACK_MOVE	*move				= STACK_INDEX(Ply);


	// Profundidad menor a PLY, llamar al buscador de capturas y promociones
	if (depth < PLY)
		return QuiesenceSearch(alpha, beta, 0);

	// Comprueba cada un cierto numero de nodos que tenemos tiempo
	Nodes++;
	if (Cknodes && !(Nodes % Cknodes) && StopSearch(score))
		return 0;

	// Mate-distance pruning
	alpha	= MAX(alpha, Ply - MATE);
	beta	= MIN(beta, MATE - Ply + 1);

	if (alpha >= beta)
		return alpha;

	// Posición tablas
	if (MatInsuf() || RepeatPos(2))
		return DRAW_VALUE(Turn);

	if (Ply >= MAX_DEPHT)
		return Eval(-MATE, MATE);

	PVLenght[Ply]		= Ply;

	// Comprueba las tablas hash
	ProbeHash(&hash_move, Nmove + depth, &hash_move_type, Hash, &hash_depth, &hash_score, Ply);
	if (hash_move_type & HASF_PRUNING)
		switch (hash_move_type & (HASF_EXACT | HASF_ALPHA | HASF_BETA))			{
			case HASF_EXACT:
				if (!PV_node)
					return hash_score;
				break;
			case HASF_ALPHA:
				if (hash_score <= alpha)
					return alpha;
				break;
			case HASF_BETA:
				if (hash_score >= beta)
					return beta;
				break;
			default:;
		}

	/*
	 * A la hora de cortar el árbol es necesario:
	 *
	 * 1) No estar en jaque a quien le toque mover
	 * 2) No estar en un movimiento de la variante principal
	 * 3) No estar Beta cerca de un valor de MATE
	 */
	in_check = IN_CHECK(Turn);
	if (!(in_check || PV_node) && alpha > -MATE && beta < MATE_SCORE)			{
		eval_score	= Eval(-MATE, MATE);

		// Razoring
		if (!hash_move && (depth <= RAZOR_DEPTH) && (eval_score + RAZORING_MARGEN(depth) <= alpha))		{
			if (depth <= PLY)
				return MAX(QuiesenceSearch(alpha, beta, 0), eval_score + RAZORING_MARGEN(depth));

			r_window	= alpha - RAZORING_MARGEN(depth);
			score		= QuiesenceSearch(r_window, r_window + 1, 0);

			if (score <= r_window)
				return score;
		}

		if (PieceMat[Turn])		{
			// Reverse Futility Pruning
			if (depth < REVERSE_FUTILITY_DEPTH && (eval_score >= beta + REVERSE_FUTILITY_MARGEN(depth)))
				return eval_score;

			// Null Move Reduction
			depth_null	= DEPTH_NULL(depth);

			if ((make_null_move && depth > PLY && eval_score >= beta) &&
			!((hash_move_type & HASF_HIT) && (hash_move_type & HASF_ALPHA) && (hash_score < beta) && ((int)hash_depth >= depth_null + Nmove)))	{
				MakeMove(EMPTY_MOVE);							// Realiza movimiento nulo
				score = (depth_null < PLY) ? -QuiesenceSearch(-beta, 1 - beta, 0) : -PVS(-beta, 1 - beta, depth_null, 0);

				UnMakeMove();									// Deshace movimiento nulo

				if (Engine & STOP)
					return 0;

				if (score >= beta)				{
					if (score >= MATE_SCORE)
						score = beta;							// No devuelve un mate

					if (GAME_PHASE > 85)		{
						StoreHash(best_move, beta, Nmove + depth, HASF_BETA, Hash, Ply);
						return score;							// Cutoff
					}

					// Null reduction
					if ((depth -= DR) < PLY)
						return QuiesenceSearch(alpha, beta, 0);
				}

				else check_mate_threat = score <= -MATE / 2 && beta > -MATE / 2;		// Detecta si se recibe una amenaza de MATE
			}
		}

		// Comprueba si es posible hacer futility prunning
		if ((PLY <= depth && depth < PLY4) && alpha > -MATE_SCORE)
			can_prune_FP = eval_score + FUTILITY_MARGEN(depth) < beta;
	}

	// Internal Iterative Deepening(IID)
	if (!hash_move && (depth >= PLY7 || (PV_node && depth >= PLY4)))			{
		score	= PVS(alpha, beta, depth - PLY2, 1);
		ProbeHash(&hash_move, Nmove + depth, &hash_move_type, Hash, &hash_depth, &hash_score, Ply);
	}

	// Generando los movimientos para la posición
	MoveGen(Turn, move, 0);
	SetMoveScore(move, hash_move, 0);							// Ordena los movimientos: 1ro hash, 2do variante principal

	// Recorre los movimientos de la posición
	for (; move->ID; move++)	{
		SortMove(move);
		if (!MakeMove(move->ID))
			continue;

		legal_moves++;
		extension		= Extensions(PV_node, check_mate_threat, move->Score & BAD_CAPTURE_SCORE);
		is_prunnable	= !(extension || in_check || PV_node) && alpha > -MATE / 2 && beta < MATE / 2;

		if (legal_moves == 1)
			score	= -PVS(-beta, -alpha, depth + extension - PLY, 1);	// Realiza una búsqueda completa para el primer movimiento

		/// se repite
		else	{
			// Futility prunning
			if (can_prune_FP && is_prunnable && move->Score < FUTILITY_PRUNNING_SCORE)			{
				UnMakeMove();
				continue;
			}

			// LMR prunning en las terminaciones
			if (legal_moves > 12 && depth < PLY2 && is_prunnable && (legal_moves > 20 || move->Score < FUTILITY_PRUNNING_SCORE)){
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
			 */
			reduction = LMRtable[depth / PLY][legal_moves];
			if (reduction && is_prunnable && move->Score < BAD_CAPTURE_SCORE)
				score	= -PVS(-alpha - 1, -alpha, depth - reduction, 1);

			else score	= alpha + 1;							// Asegura que una búsqueda completa sea hecha

			if (score > alpha)	{
				score	= -PVS(-alpha - 1, -alpha, depth + extension - PLY, 1);

				if (!(Engine & STOP) && score > alpha && score < beta)
					score= -PVS(-beta, -alpha, depth + extension - PLY, 1);
			}
		}

		UnMakeMove();
		if (Engine & STOP)
			return 0;

		if (score > best_score)	{
			best_score	= score;
			best_move	= move->ID;

			if (score > alpha)	{
				alpha		= score;
				hash_type	= HASF_EXACT;

				UpdatePV(best_move);

				if (score >= beta)				{
					// Actualiza la historia para los movimientos tranquilos
					if (!(extension || in_check) && (move->Score < BAD_CAPTURE_SCORE))
						UpdateHistory(best_move, depth);

					// Movimientos matekiller y killer son buenos, guardar información de ellos
					if (!(move->Score & (GOOD_CAPTURE_SCORE | PROM_SCORE)))
						SetKillers(best_move, score, move->Score);

					StoreHash(best_move, beta, Nmove + depth, HASF_BETA, Hash, Ply);
					return best_score;							// Cutoff del alphabeta
				}
			}
		}
	}

	if (legal_moves)			{
		if (Rule50 >= 100)
			return DRAW_VALUE(Turn);
	}

	//								MATE			AHOGADO
	else best_score = in_check ? (Ply - MATE) : (DRAW_VALUE(Turn));

	StoreHash(best_move, best_score, Nmove + depth, hash_type, Hash, Ply);
	return best_score;
}


// Mostrando lo que el motor esta pensando
void	PrintPV(int depth, int score)			{
	int		i, 
			time_used	= ELAPSED_TIME();
	MOVE*	move		= PV;
	double	nps			= (time_used > 16) ? ((1000.0 * (double)Nodes) / (double)time_used) : 0;


	// Envia profundidad, puntos, tiempo, nodos y nodos / segundos
	printf("info depth %d time %d nodes %d nps %d ", depth, time_used, Nodes, (int)nps);

	// Ajusta el score si tenemos un mate para que lo muestre asi la GUI
	if (score > MATE_SCORE || score < -MATE_SCORE)
		printf("score mate %d ", (score > 0) ? ((MATE - score) / 2 + 1) : ((MATE - score) / 2 - MATE));

	else printf("score cp %d ", score);

	// Indica si se tiene un lower o upper bound
	if (FailUCI == LOW)
		printf("upperbound ");

	else if (FailUCI == HIGH)
		printf("lowerbound ");

	// Imprime la mejor linea
	printf("pv");
	for (i = PVLenght[Ply]; i && *move; i--){
		printf(" ");
		PrintMove(*move);
		move++;
	}

	printf("\n");
}


// Similar a PVS con algunos datos precalculados
int		PVSroot(int alpha, int beta, int depth)	{
	int			extension, score, is_prunnable, reduction,
				PV_node			= (beta - alpha) > 1,
				legal_moves		= 0,
				best_score		= -MATE,
				hash_type		= HASF_ALPHA;
	MOVE		best_move		= EMPTY_MOVE;
	STACK_MOVE	*move			= MoveStack;


	Nodes++;
	Engine			|= FOLLOW_PV;
	SetMoveScore(move, EMPTY_MOVE, 0);							// Pone 1ro los movimientos de la variante principal

	for (move = MoveStack; move->ID; move++)	{
		SortMove(move);
		if (!MakeMove(move->ID))
			continue;

		legal_moves++;
		extension		= Extensions(PV_node, 0, move->Score & BAD_CAPTURE_SCORE);
		is_prunnable	= !(extension || InCheckRoot || PV_node) && alpha > -MATE / 2 && beta < MATE / 2;

		// Muestra valoracion extra por cada movimiento en el protocolo uci 
		if (UpdateInfoUCI)	{
			printf("info depth %d", Li);
			printf(" currmove ");
			PrintMove(move->ID);
			printf(" currmovenumber %d\n", legal_moves);
		}

		if (legal_moves == 1)
			score		= -PVS(-beta, -alpha, depth + extension - PLY, 1);

		/// se repite
		else	{
			/*
			 * Prueba reducir la profundidad (LMR) si:
			 *
			 * 1) No hay extensiones
			 * 2) Hay un numero máximo de movimientos legales y se tiene una determinada profundidad
			 * 3) El valor del movimiento no es grande
			 * 4) No es el movimiento de la tabla hash, variante principal, killers o matekiller
			 */
			reduction = LMRtable[depth / PLY][legal_moves];
			if (reduction && is_prunnable && move->Score < BAD_CAPTURE_SCORE)
				score	= -PVS(-alpha - 1, -alpha, depth - reduction, 1);

			else score	= alpha + 1;							// Asegura que una búsqueda completa sea hecha

			if (score > alpha)	{
				score	= -PVS(-alpha - 1, -alpha, depth + extension - PLY, 1);

				if (!(Engine & STOP) && score > alpha && score < beta)
					score= -PVS(-beta, -alpha, depth + extension - PLY, 1);
			}
		}

		UnMakeMove();
		if (Engine & STOP)
			return 0;

		if (score > best_score)				{
			best_score		= score;
			best_move		= move->ID;

			if (score > alpha)				{
				alpha		= score;
				hash_type	= HASF_EXACT;

				UpdatePV(best_move);

				if (score >= beta)			{
					// Actualiza la historia para los movimientos tranquilos
					if (!(extension || InCheckRoot) && (move->Score < BAD_CAPTURE_SCORE))
						UpdateHistory(best_move, depth);

					// Movimientos matekiller y killer son buenos, aprovechamos para guardar información de ellos
					if (!(move->Score & (GOOD_CAPTURE_SCORE | PROM_SCORE)))
						SetKillers(best_move, score, move->Score);

					StoreHash(best_move, beta, Nmove + depth, HASF_BETA, Hash, Ply);
					return best_score;
				}

				if (Li >= MIN_POST && PVLenght[Ply])
					PrintPV(Li, score);
			}
		}
	}

	switch (legal_moves)		{
		// Mate o ahogado
		case 0:	best_score	= InCheckRoot ? (Ply - MATE) : DRAW_VALUE(Turn);	break;

		// Un solo 1 movimiento, hacerlo de inmediato
		case 1:
			if (!(Engine & ANALYZING))
				Engine	|= STOP | ABORT;
			break;

		default:;
	}

	StoreHash(best_move, best_score, Nmove + depth, hash_type, Hash, Ply);
	return best_score;
}


// Retorna el mejor movimiento para una profundidad, usando de forma iterativa al algoritmo alphabeta.
MOVE	EngineThink(int state, int remaining_time, int inc_time, int mps, int max_depth, int movetime, int out_of_book)	{
	int		i,
			mat_sum			= PieceMat[BLACK] + PieceMat[WHITE] + PawnMat[WHITE] + PawnMat[BLACK],
			fail_count		= 0,
			alpha_window	= DEFAULT_WINDOW,
			beta_window		= DEFAULT_WINDOW,
			alpha			= -MATE,
			beta			= MATE;
	MOVE	best_move		= EMPTY_MOVE;


	StartTimer(remaining_time, inc_time, mps, out_of_book, mat_sum, movetime);

	// Inicializa algunas variables
	Engine			= (Engine ^ (Engine & (ABORT | STOP))) | state;
	UpdateInfoUCI	= FailUCI = Nodes = 0;
	MaxDepth		= max_depth;

	// Cada cuanto (nodos) revisar el tiempo? lo hace mas seguido en el final del juego
	Cknodes			= TAPERED_SCORE(16384, 1024);

	// Elimina los datos de los Killers, MateKillers y PV
	memset(PV,			0, sizeof(PV));
	memset(PVLenght,	0, sizeof(PVLenght));
	memset(MateKiller,	0, sizeof(MateKiller));
	memset(Killer1,		0, sizeof(Killer1));
	memset(Killer2,		0, sizeof(Killer2));

	// Actualiza la historia para ambos bandos
	HistoryAge(BLACK, 3);
	HistoryAge(WHITE, 3);

	CalcHashDate(Nmove);

	// Se generan los movimientos(y otros datos) de la raiz, estos no cambian y no es necesario recalcularlos
	InCheckRoot		= IN_CHECK(Turn);
	Ply				= 0;
	MoveGen(Turn, MoveStack, 0);

	/*
	 * Búsqueda de forma iterativa, tiene 2 ventajas:
	 *
	 * 1) Si el tiempo se termina al menos se tiene una búsqueda completa en una profundidad.
	 * 2) Cada profundidad puede resolverse mas rápido que la anterior por el uso de las tablas hash y mejor ordenación de la variante principal
	 */
	for (i = 1; i <= MaxDepth && (fail_count < 16) && !(Engine & STOP);)		{
		Li			= i;

		// Para las primeras profundidades se realiza una búsqueda completa
		if (i <= PLY)			{
			alpha	= -MATE;
			beta	= MATE;
		}

		// Llamando a la busqueda con profundidad fraccional, permite un mejor control.
		Value1[i]	= PVSroot(alpha, beta, i * PLY);
		best_move	= PV[0];

		// Se abren nuevas ventanas si se produce un fallo en la búsqueda
		if (Value1[i] <= alpha)	{
			alpha	= (alpha_window & 0xFE00) ? (-MATE) : (Value1[i] - (alpha_window <<= 1));
			FailUCI	= LOW;
			fail_count++;
			continue;
		}

		if (Value1[i] >= beta)	{
			beta	= (beta_window & 0xFE00) ? (MATE) : (Value1[i] + (beta_window <<= 1));
			FailUCI	= HIGH;
			fail_count++;
			continue;
		}

		// Visualiza la variante principal al final de cada iteración 
		if (PVLenght[Ply])
			PrintPV(i, Value1[i]);

		// Si la cosa es clara, reducir el tiempo de búsqueda
		if ((Engine & SEARCHING) && i > PLY)	{
			Engine |= ABORT;									// Se tiene una profundidad máxima para abandonar la búsqueda

			if (ITERATION_TIME_EXC())
				break;											// No comenzar nueva iteración si ya ha consumido al menos la mitad de tiempo
		}

		// Configura las ventanas para el siguiente ciclo
		alpha_window= beta_window = DEFAULT_WINDOW;
		alpha		= Value1[i] - alpha_window;
		beta		= Value1[i] + beta_window;
		fail_count	= FailUCI = 0;
		i++;

		// envia informacion extra a la GUI en el protocolo uci si al menos ha pasado 1 segundo 
		if (ELAPSED_TIME() > 1 * SEC)
			UpdateInfoUCI	= 1;
	}

	return best_move;
}


//	900
