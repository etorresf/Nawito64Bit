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
void	HistoryAge(U8 Dec)		{
	unsigned		i;

	MaxHistory[BLACK] >>= Dec;
	MaxHistory[WHITE] >>= Dec;

	for (i = 1; i < 4095; i++)	{
		History[BLACK][i] >>= Dec;
		History[WHITE][i] >>= Dec;
	}
}


// Actualiza la variante principal con un movimiento
void	UpdatePV(MOVE Move)		{
	int	i;

	PV[Ply][Ply] = Move;

	for (i = Ply + 1; i < PVLenght[Ply + 1]; i++)
		PV[Ply][i]	= PV[Ply + 1][i];

	PVLenght[Ply]	= PVLenght[Ply + 1];
}


// Pone en primer lugar el movimiento con mayor valor
void	SortMove(STACK_MOVE* Stack)				{
	STACK_MOVE	*First, m,
				*Last		= FirstMove[Ply + 1],
				*Best		= Stack;

	for (First = Stack + 1; First < Last; First++)
		if (First->Score > Best->Score)
			Best = First;

	if (Best != Stack)			{
		m			= *(Stack);
		*(Stack)	= *(Best);
		*(Best)		= m;
	}
}


// Extensiones
int		Extensions(U8 PVnode, U8 CheckMateThreat)				{
	HIST_MOVE	*Actual		= &(Moves[Nmove - 1]),
				*Previous	= &(Moves[Nmove - 2]);


	// Amenaza de MATE
	if (CheckMateThreat)
		return HALF_PLY << PVnode;


	// Amenaza de MATE o Se da jaque 
	if (InCheck(Turn))			{
		if (CountLegals(2) < 2)
			return PLY << PVnode;								// Movimiento Forzado

		return HALF_PLY << PVnode;
	}


	if (PVnode && Actual->Capture)				{
		// Entrando en un final de peones, idea del motor Rebel. Si se entra a final de peones pensar mas
		if (MatSum < END_GAME_MATERIAL && !(PieceMat[BLACK] || PieceMat[WHITE]) && PawnEndgameExt[Actual->Capture])
			return PawnEndgameExt[Actual->Capture];

		// Recaptura
		if (Previous->Capture && (GET_TO(Actual->Move) == GET_TO(Previous->Move)) && (PieceValue[Actual->Capture] <= PieceValue[Previous->Capture]))
			return PLY;
	}

	return 0;
}


// Pone primero el moviminto de hash y luego el de la variante principal
void	SortHmovePV(MOVE HashID){
	MOVE		PV_ID	= PV[0][Ply];
	STACK_MOVE	*First	= FirstMove[Ply],
				*Last	= FirstMove[Ply + 1];


	// Detecta si es un movimiento del PV
	if (SearchState & FOLLOW_PV){
		SearchState ^= FOLLOW_PV;

		if (PV_ID)
			for(; First < Last; First++)
				if (First->ID == PV_ID)			{
					SearchState		|= FOLLOW_PV;
					First->Score	|= FOLLOW_PV_SCORE;
					break;
				}
	}

	// Detecta si es un movimiento de las tablas hash
	if (HashID)
		for(; First < Last; First++)
			if (First->ID == HashID)			{
				First->Score |= HASH_SCORE;
				return;
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
	struct timeval tv;
	struct timezone tz;
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
STATE	Checkup(int Score)		{
	int	TimeUsed = ClockTime() - TimeLeft;


	if ((EngineState & SEARCHING) && (SearchState & ABORT))		{
		if (Bioskey())			{
			EngineTurn = EMPTY_TURN;
			return SearchState |= STOP;
		}

		if (!WindowFlag && ((TimeUsed >= TimeLimit) || (TimeUsed > NoNewMov && Score + MARGEN > Value1[Li - 1])))
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

			// Recibiendo comando desde la interfaz
			sscanf(Line, "%s", Command);

			if (!strcmp(Command, "."))
				return SearchState ^= (SearchState & STOP);

			if (!strcmp(Command, "exit"))
				return SearchState |= STOP;
		}

		// Tiempo agotado
		if (!WindowFlag && TimeUsed >= TimeLimit)
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
void	StoreTT(MOVE Move, int Score, int Depth, U8 Type)		{
	if (Score >= MATE_SCORE)
		StoreHash(Move, Score + Ply, Depth, Type, Hash, CurrentAge);

	else if (Score <= -MATE_SCORE)
		StoreHash(Move, Score - Ply, Depth, Type, Hash, CurrentAge);

	else StoreHash(Move, Score, Depth, Type, Hash, CurrentAge);
}


// Busca capturas y promociones, no lleva profundidad. Sirve para evitar el efecto horizonte
int		QuiesenceSearch(int Alpha, int Beta, int Depth)			{
	U8			InCheck;
	STACK_MOVE	*Move, *LastMove;
	int			Score		= Alpha,
				BestScore	= -MATE,
				StaticEval	= 0,
				Delta		= 0,
				LegalMoves	= 0;


	// Comprueba cada un cierto numero de nodos que tenemos tiempo
	if (!((++Nodes) % Cknodes) && (Checkup(Score) & STOP))
		return 0;

	// Mate-distance pruning
	Alpha	= MAX(Alpha, Ply - MATE);
	Beta	= MIN(Beta, MATE - Ply);

	if (Alpha >= Beta)
		return Alpha;


	// Hoja del árbol de búsqueda, se procede a evaluar
	if (Ply & MAX_DEPHT_1)
		return Eval(Alpha, Beta);


	// La posición se repite o no existe material suficiente para ganar. Se valora como tablas
	if (RepeatPos(2) || MatInsuf())
		return MatSum / ContemFactor[Turn & EngineTurn];


	PVLenght[Ply]	= Ply;
	InCheck			= InCheck(Turn);


	// Generando los capturas para la posición, en caso de estar en jaque se generan evasiones
	if (InCheck)
		LastMove = FirstMove[Ply + 1] = MoveGen(FirstMove[Ply]);

	else	{
		BestScore	= StaticEval = Eval(Alpha, Beta);
		Alpha		= MAX(Alpha, BestScore);

		if (Alpha >= Beta)
			return Alpha;

		if (MatSum > END_GAME_MATERIAL)
			Delta = Alpha - 200 - StaticEval;					// delta prunning


		LastMove = FirstMove[Ply + 1] = CaptureGen(FirstMove[Ply]);
	}


	SortHmovePV(EMPTY_MOVE);									// Pone 1ro los movimientos de la variante principal

	// Recorre las capturas de la posición
	for (Move = FirstMove[Ply]; Move < LastMove; Move++)		{
		SortMove(Move);


		if (!InCheck )			{
			if (BestScore > -MATE_SCORE && !PROMOTION(Move->ID) && (PieceValue[Board[GET_TO(Move->ID)]] < Delta))
				continue;										// delta pruning

			if ((Move->Score & BAD_CAPTURE_SCORE) || SEE(Turn, GET_TO(Move->ID), GET_FROM(Move->ID), 1) < 0)
				continue;										// see pruning
		}


		if (Depth <= QS_LIMITE && !InCheck)
			Score = StaticEval + SEE(Turn, GET_TO(Move->ID), GET_FROM(Move->ID), 0);			// Previene qsearch exploxion

		else	{
			if (!MakeMove(Move->ID))
				continue;

			LegalMoves++;
			Score = -QuiesenceSearch(-Beta, -Alpha, Depth - 1);
			UnMakeMove();
		}


		if (SearchState & STOP)
			return 0;


		if (Score > BestScore)	{
			BestScore = Score;

			if (Score > Alpha)	{
				Alpha = Score;

				if (Score >= Beta)
					return BestScore;
			}
		}
	}


	if (InCheck && !LegalMoves)
		return Ply - MATE;

	return BestScore;
}


/*
 * Algoritmo alphabeta encargado de la búsqueda, basado en un algoritmo de
 * Bruce Moreland conocido como variante principal
 */
int		PVS(int Alpha, int Beta, int Depth, U8 MakeNullMove)	{
	U8			HType, InCheck,
				HashFlag		= HASF_ALPHA,
				WeCanCut		= 0,
				CheckMateThreat = 0,
				PVnode			= (Beta - Alpha) > 1;
	int			Score			= Alpha,
				LegalMoves		= 0,
				BestScore		= Ply - MATE,
				Ev, Ext, Reduc, RWindow, HashScore, HashDepth;
	MOVE		BestMove		= EMPTY_MOVE,
				HashMove		= EMPTY_MOVE,
				ID;
	STACK_MOVE	*HTmp, *Move, *LastMove;


	// Si la profundidad es menor a PLY o se ha ido muy profundo, se llama al buscador de capturas y promociones
	if (Depth < PLY)
		return QuiesenceSearch(Alpha, Beta, 0);


	// Comprueba cada un cierto numero de nodos que tenemos tiempo
	if (!((++Nodes) % Cknodes) && (Checkup(Score) & STOP))
		return 0;


	// Mate-distance pruning
	Alpha	= MAX(Alpha, Ply - MATE);
	Beta	= MIN(Beta, MATE - Ply + 1);

	if (Alpha >= Beta)
		return Alpha;


	if (Ply & MAX_DEPHT_1)
		return Eval(-MATE, MATE);


	// La posición se repite o no existe material suficiente para ganar. Se valora como tablas
	if (RepeatPos(2) || MatInsuf())
		return MatSum / ContemFactor[Turn & EngineTurn];


	// Comprueba las tablas hash
	ProbeHash(&HashMove, Nmove + Depth, &HType, Hash, &HashDepth, &HashScore, CurrentAge);
	if (HType & HASF_PRUNING)	{
		// Ajusta el valor en caso de MATE
		if (HashScore >= MATE_SCORE)
			HashScore -= Ply;

		else if (HashScore <= -MATE_SCORE)
			HashScore += Ply;


		if (HType & HASF_EXACT)	{
			if (!PVnode)
				return HashScore;
		}
		else if (HType & HASF_ALPHA)			{
			if (HashScore <= Alpha)
				return Alpha;
		}
		else if (HType & HASF_BETA)				{
			if (HashScore >= Beta)
				return Beta;
		}
	}


	/*
	 * A la hora de cortar el árbol es necesario:
	 *
	 * 1) No estar en jaque a quien le toque mover
	 * 2) No estar en un movimiento de la variante principal
	 * 3) No estar Beta cerca de un valor de MATE
	 */
	InCheck = InCheck(Turn);
	if (!(InCheck || PVnode) && Beta < MATE_SCORE)				{
		Ev = Eval(-MATE, MATE);

		// Razoring
		if (!HashMove && (Depth <= PLY3) && (Ev + RazoringMargen[Depth] <= Alpha)){
			if (Depth <= PLY)
				return MAX(QuiesenceSearch(Alpha, Beta, 0), Ev + RazoringMargen[Depth]);

			RWindow	= Alpha - RazoringMargen[Depth];
			Score	= QuiesenceSearch(RWindow, RWindow + 1, 0);

			if (Score <= RWindow)
				return Score;
		}


		if (PieceMat[Turn])		{
			// Static null move prunning
			if (Depth < PLY7 && (Ev >= Beta + ReverseFutilityMargen[Depth]))
				return Ev;


			DepthNull = DepthNullTable[Depth >> 1];

			if ((MakeNullMove && Depth > PLY && Ev >= Beta) &&
			!((HType & HASF_HIT) && (HType & HASF_ALPHA) && (HashScore < Beta) && ((int)HashDepth >= DepthNull + Nmove)))		{
				// Guarda donde inician los movimientos del próximo nivel
				HTmp					= FirstMove[Ply + 1];
				FirstMove[Ply + 1]		= FirstMove[Ply];


				// Realiza el movimiento nulo, actualiza la llave hash y otros datos
				Moves[Nmove].Move		= 0;
				Moves[Nmove].Casttle	= Casttle;
				Moves[Nmove].EnPassant	= EnPassant;
				Moves[Nmove].Rule50		= Rule50;
				Moves[Nmove].Hash		= Hash;

				Hash					^= HashEnPassant[EnPassant];
				EnPassant				= 0;

				Rule50++;
				Ply++;
				Nmove++;
				Hash					^= HashTurn[Turn];
				Hash					^= HashTurn[Turn ^= 1];


				if (DepthNull < PLY)
					Score	= -QuiesenceSearch(-Beta, 1 - Beta, 0);

				else Score	= -PVS(-Beta, 1 - Beta, DepthNull, 0);


				// Deshace el movimiento nulo
				SAWP_TURN(Turn);
				Nmove--;
				Ply--;
				Casttle		= Moves[Nmove].Casttle;
				EnPassant	= Moves[Nmove].EnPassant;
				Rule50		= Moves[Nmove].Rule50;
				Hash		= Moves[Nmove].Hash;


				// Recupera donde inicia los movimientos del actual nivel
				FirstMove[Ply + 1] = HTmp;

				if (SearchState & STOP)
					return 0;


				if (Score >= Beta)				{
					if (MatSum < END_GAME_MATERIAL)				{
						Depth -= PLY5;

						if (Depth < PLY)						// Null reduction
							return QuiesenceSearch(Alpha, Beta, 0);
					}
					else		{
						StoreTT(BestMove, Beta, Nmove + Depth, HASF_BETA);

						if (Score >= MATE_SCORE)
							Score = Beta;						// No devuelve un mate

						return Score;							// Cutoff
					}
				}
				// Detecta si se recibe una amenaza de MATE
				else CheckMateThreat = Score <= -MATE / 2 && Beta > -MATE / 2;
			}
		}

		// Comprueba si es posible hacer futility prunning
		if (Depth < PLY4 && Alpha > -MATE_SCORE)
			WeCanCut = Ev + Margen[Depth / PLY] < Beta;
	}


	// Internal Iterative Deepening
	if (!HashMove && (Depth >= PLY7 || (PVnode && Depth >= PLY4)))				{
		Score = PVS(Alpha, Beta, Depth - PLY2, 1);
		ProbeHash(&HashMove, Nmove + Depth, &HType, Hash, &HashDepth, &HashScore, CurrentAge);
	}


	// Generando los movimientos para la posición
	PVLenght[Ply] = Ply;
	LastMove = FirstMove[Ply + 1] = MoveGen(FirstMove[Ply]);


	// Ordena los movimientos: 1ro hash, 2do variante principal
	SortHmovePV(HashMove);


	// Recorre los movimientos de la posición
	for (Move = FirstMove[Ply]; Move < LastMove; Move++)		{
		SortMove(Move);

		if (!MakeMove(Move->ID))
			continue;


		LegalMoves++;
		Ext = Extensions(PVnode, CheckMateThreat);


		// Realiza una búsqueda completa para el primer movimiento
		if (LegalMoves == 1)
			Score = -PVS(-Beta, -Alpha, Depth + Ext - PLY, 1);

		else	{
			// Futility prunning
			if (WeCanCut && !Ext && Move->Score < FUTILITY_PRUNNING_SCORE)		{
				UnMakeMove();
				continue;
			}


			// LMR prunning en las terminaciones
			if (LegalMoves > 12 && Depth < PLY2 && !(InCheck || PVnode || Ext) && Alpha > -MATE / 2 &&
			(LegalMoves > 20 || Move->Score < FUTILITY_PRUNNING_SCORE))			{
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
			Reduc = (int)LMRtable[Depth / PLY][LegalMoves];
			if (Reduc && !(Ext || InCheck || PVnode) && Alpha > -MATE / 2 && Beta < MATE / 2 && Move->Score < BAD_CAPTURE_SCORE)
				Score = -PVS(-Alpha - 1, -Alpha, Depth - Reduc, 1);

			else Score = Alpha + 1;								// Asegura que una búsqueda completa sea hecha


			if (Score > Alpha)	{
				Score = -PVS(-Alpha - 1, -Alpha, Depth + Ext - PLY, 1);

				if (!(SearchState & STOP) && Score > Alpha && Score < Beta)
					Score = -PVS(-Beta, -Alpha, Depth + Ext - PLY, 1);
			}
		}


		UnMakeMove();

		if (SearchState & STOP)
			return 0;


		if (Score > BestScore)	{
			BestScore	= Score;
			BestMove	= Move->ID;


			if (Score > Alpha)	{
				Alpha	= Score;
				HashFlag= HASF_EXACT;

				UpdatePV(Move->ID);								// Actualiza la variante principal
				ID = GET_ID(Move->ID);


				// Actualiza la historia para los movimientos tranquilos
				if (!(Ext || InCheck) && (Move->Score < BAD_CAPTURE_SCORE))		{
					History[Turn][ID] += (Depth * Depth) / PLY4;

					if (History[Turn][ID] > MaxHistory[Turn])
						MaxHistory[Turn] = History[Turn][ID];

					if (MaxHistory[Turn] >= BAD_CAPTURE_SCORE)
						HistoryAge(1);
				}


				if (Score >= Beta)				{
					// Movimientos matekiller y killer son buenos, aprovechamos para guardar información de ellos
					if (!(Move->Score & GOOD_CAPTURE_PROM_SCORE))				{
						if (Score > MATE_SCORE)
							MateKiller[Ply] = ID;

						else if ((ID != Killer1[Ply]) && !(Move->Score & MATE_KILLER_SCORE))	{
							Killer2[Ply] = Killer1[Ply];
							Killer1[Ply] = ID;
						}
					}

					StoreTT(BestMove, Beta, Nmove + Depth, HASF_BETA);
					return BestScore;							// Cutoff del alphabeta
				}
			}
		}
	}


	if (LegalMoves)				{
		if (Rule50 >= 100)
			return MatSum / ContemFactor[Turn & EngineTurn];	// Tablas por repeticion
	}
	else if (InCheck)
		BestScore = Ply - MATE;									// Mate

	else BestScore = MatSum / ContemFactor[Turn & EngineTurn];	// Ahogado


	StoreTT(BestMove, BestScore, Nmove + Depth, HashFlag);
	return BestScore;
}


// Muestra el movimiento seleccionado, profundidad, puntuación, tiempo empleado, nodos examinados
void	PrintPV(int Depth, int Score)			{
	int	i	= 0;

	if (PVLenght[Ply] < 1 || !(EngineConfig & POST))
		return;


	// Envía profundidad, puntos, tiempo(centisegundos), nodos
	if (EngineState & PONDERING)
		printf("%d %d %d %d ", Depth, -Score, (ClockTime() - TimeLeft) / 10, Nodes);

	else printf("%d %d %d %d ", Depth, Score, (ClockTime() - TimeLeft) / 10, Nodes);


	// Imprime la mejor linea
	for (i = 0; (i < PVLenght[Ply]) && PV[0][i]; i++)			{
		PrintMove(PV[0][i]);
		printf(" ");
	}


	printf("\n");												// Indica el fin de linea
	fflush(stdout);												// Obligado por el protocolo xboard
}


// Similar a PVS con algunos datos precalculados
int		PVSroot(int Alpha, int Beta, int Depth)	{
	STACK_MOVE	*Move;
	int			Ext,
				Score			= 0,
				BestScore		= -MATE;
	U8			HashFlag		= HASF_ALPHA;
	MOVE		BestMove		= EMPTY_MOVE,
				ID;
	LegalMovesRoot	= 0;


	Nodes++;

	FirstMove[1]	= MoveGen(FirstMove[0]);
	SearchState		|= FOLLOW_PV;

	SortHmovePV(EMPTY_MOVE);									// Pone 1ro los movimientos de la variante principal


	for (Move = FirstMove[0]; Move < FirstMove[1]; Move++)		{
		SortMove(Move);

		if (!MakeMove(Move->ID))
			continue;


		LegalMovesRoot++;
		Ext		= Extensions(1, 0);
		Score	= -PVS(-Beta, -Alpha, Depth + Ext - PLY, 1);


		UnMakeMove();
		if (SearchState & STOP)
			return 0;


		if (Score > BestScore)	{
			BestScore	= Score;
			BestMove	= Move->ID;


			if (Score > Alpha)	{
				Alpha	 = Score;
				HashFlag = HASF_EXACT;

				UpdatePV(Move->ID);								// Actualiza la variante principal
				ID = GET_ID(Move->ID);


				// Actualiza la historia para los movimientos tranquilos
				if (!(Ext || InCheckRoot) && (Move->Score < BAD_CAPTURE_SCORE)){
					History[Turn][ID] += (Depth * Depth) / PLY4;

					if (History[Turn][ID] > MaxHistory[Turn])
						MaxHistory[Turn] = History[Turn][ID];

					if (MaxHistory[Turn] >= BAD_CAPTURE_SCORE)
						HistoryAge(1);
				}


				if (Score >= Beta)				{
					// Movimientos matekiller y killer son buenos, aprovechamos para guardar información de ellos
					if (!(Move->Score & GOOD_CAPTURE_PROM_SCORE))				{
						if (Score > MATE_SCORE)
							MateKiller[Ply] = ID;

						else if ((ID != Killer1[Ply]) && !(Move->Score & MATE_KILLER_SCORE))	{
							Killer2[Ply] = Killer1[Ply];
							Killer1[Ply] = ID;
						}
					}

					StoreTT(BestMove, Beta, Nmove + Depth, HASF_BETA);
					return BestScore;
				}


				if (Nodes > Noise)
					PrintPV(Li, Score);							// Mostrar valoración
			}
		}
	}


	if (LegalMovesRoot)			{
		if ((LegalMovesRoot == 1) && !(EngineState & ANALYZING))
			SearchState |= STOP;								// Un solo 1 movimiento, hacerlo de inmediato
	}

	else if (InCheckRoot)
		BestScore	= Ply - MATE;								// Mate

	else BestScore	= MatSum / ContemFactor[Turn & EngineTurn];	// Ahogado


	StoreTT(BestMove, BestScore, Nmove + Depth, HashFlag);
	return BestScore;
}


// Retorna el mejor movimiento para una profundidad, usando de forma iterativa al algoritmo alphabeta.
MOVE	EngineThink(void)		{
	TimeLeft	= ClockTime();
	Li			= 0;
	MOVE		BestMove;
	int			i,
				WindowAlpha		= 16,
				WindowBeta		= 16,
				Alpha			= -MATE,
				Beta			= MATE;


	// Inicializa algunas variables
	CurrentAge		= (U8)(Nmove >> 1);
	BestMove		= 0;
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


	/*
	 * Búsqueda de forma iterativa, tiene 2 ventajas:
	 *
	 * 1) Si el tiempo se termina al menos se tiene una búsqueda completa en una profundidad.
	 * 2) Cada profundidad se puede resolver mas rápido que la anterior debido al uso de las
	 *	tablas hash y a una mejor ordenación de la variante principal
	 */
	for (i = 1; i <= MaxDepth;)	{
		Li			= i;

		if (WindowFlag & 16)
			break;


		// Para las primeras profundidades se realiza una búsqueda completa
		if (i < PLY)			{
			Alpha	= -MATE;
			Beta	= MATE;
		}


		Value1[i]	= PVSroot(Alpha, Beta, i * PLY);			// Profundidad fraccional, permite un mejor control
		BestMove	= PV[0][0];

		if (SearchState & STOP)
			break;


		// Se abren nuevas ventanas si se produce un fallo en la búsqueda
		if (Value1[i] <= Alpha)	{
			if (WindowAlpha & 0xFE00)
				Alpha = -MATE;

			else Alpha = Value1[i] - (WindowAlpha <<= 1);

			WindowFlag++;
			continue;
		}

		if (Value1[i] >= Beta)	{
			if (WindowBeta & 0xFE00)
				Beta = MATE;

			else Beta = Value1[i] + (WindowBeta <<= 1);

			WindowFlag++;
			continue;
		}


		if ((EngineState & SEARCHING) && i > 4)	{				// Si la cosa es clara, reducir el tiempo de búsqueda
			SearchState |= ABORT;								// Se tiene una profundidad máxima para abandonar la búsqueda

			if (ClockTime() - TimeLeft > NoNewIteration)
				break;											// No comenzar nueva iteración si ya ha consumido al menos la mitad de tiempo
		}


		// Configura las ventanas para el siguiente ciclo
		WindowAlpha	= WindowBeta = 16;
		Alpha		= Value1[i] - WindowAlpha;
		Beta		= Value1[i] + WindowBeta;


		i++;
		WindowFlag = 0;
	}

	return BestMove;
}


// Si es final de partida indica el resultado de la misma
void	PrintResult(void)		{
	Ply	= 0;


	if (CountLegals(1))			{
		if (RepeatPos(3))
			printf("1/2-1/2 {Draw by repetition}\n");

		else if (Rule50 >= 100)
			printf("1/2-1/2 {Draw by 50 move rule}\n");

		else if (MatInsuf())
			printf("1/2-1/2 {Draw by insufficient material}\n");
	}

	else if (InCheck(Turn))		{
		if (Turn == WHITE)
			printf("0-1 {White mates}\n");

		else printf("1-0 {Black mates}\n");
	}

	else printf("1/2-1/2 {Stalemate}\n");
}


// 900
