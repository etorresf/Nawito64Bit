# Nawito64Bit

Nawito2012 - Chess engine by Ernesto Torres. Cuba.
===============================================================================================
Engine version:				2012
Release date:				?-12-2020
Available versions:			Windows/Linux of 64 bits
Communication protocol:		Xboard
ELO estimated:				± 2650
License:					GPL
Autor email:				ernesto2@nauta.cu


TABLE OF CONTENTS
===============================================================================================
1) General description.
2) Content.
3) Performance.
4) Features.
5) License.
6) Credits.


1)	GENERAL DESCRIPTION 
===============================================================================================

Nawito2012 is a free Cuban chess engine derived from Danasah5.07 compatible with the protocol
Winboard. To get the most out of it, it should be used under a GUI (graphical user interface)
like Sand (Linux / Windows), Winboard (Windows), Pychess (Linux / Windows), SCID (Linux / Windows).

Its author, Ernesto Torres, has been based on the search function of the Danasah engine and inspired in 
other engines like TSCP, CPW, Rebel, etc. To develop your own ideas and give it a particular style

It is written in C and CodeBlocks are used with GNU GCC, for its compilation, both for Windows
as for Linux. 

This engine uses its own scheme of opening books.

Please report any ideas, suggestions or errors about the engine.


2)	CONTENT
===============================================================================================

- Nawito2012				->	Engine for Linux.
- Nawito2012.exe			->	Engine for Windows.
- LEEME.txt					->	Spanish translation of this file.
- README.txt				->	This file.
- Nawito2012.ini			->	Engine configuration.
- Nawito1812book.nbk
  Nawito1812MainBook.nbk	->	Opening books for the chess engine.
- logo.jpg					->	Logo del programa.


3)	PERFORMANCE
===============================================================================================

--perft <n>				->	Command to discover faults or measure move generator speed from the 
							position:
							"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "
							to the depth <n> (by default 5).
--bench					->	Command to check the relative speed of the computer with respect to 
							another. Solve mate from the position:
							"r3rk2/ppq2pbQ/2p1b1p1/4p1B1/2P3P1/3P1B2/P3PPK1/1R5R w - - 0 1 "


4)	FEATURES
===============================================================================================

- Protocol						->	Xboard.
- Board representation			->	Bitboard.
- Generador de movimientos		->	Bitboard.
- Búsqueda						->	Aspiration window
								->	Iterative depth.
								->	PVS/Alpha-Beta.
								->	Quiescence.
- Reducciones/Podas				->	Checkmate distance.
								->	Null moveme.
								->	Transposition table.
								->	Futility.
								->	Razoring.
								->	Captures.
								->	Late Move Reductions (LMR).
- Extensiones					->	Check.
								->	Checkmate threat.
								->	Pawns end.
- Ordenación de movimientos		->	PV.
								->	Transposition table.
								->	Internal iterative deepening(IID).
								->	MVV/LVA.
								->	Killer heuristic.
								->	Historical heuristic.
- Evaluación					->	Material.
								->	Piece square table
								->	Material tables.
								->	Tapered eval.
								->	Pawns struct.
									->	Backward pawn.
									->	Advanced pawn.
									->	Candidate pawns.
									->	Doubled pawns.
									->	Isolated pawn.
									->	Passed pawn.
								->	Bishop pair.
								->	Bad bishop.
								->	Traped bishop.
								->	Traped knight.
								->	Outpost knight.
								->	Knight decreasing value as pawns disappear.
								->	Rook increasing value as pawns disappear.
								->	Rook in open o semi-open file abierta.
								->	Rook in 8th.
								->	Traped Rook.
								->	Blocked Rook.
								->	Rook in 7th.
								->	King seafty.
								->	King shield.
								->	King Tropism.
								->	Endings (KP vs k, KBN vs k, KR vs k, KBB vs K, KRP vs KR, others).
- Opening Book					->	Own book scheme.
- Others						->	Detection of the end of the game with the rules of FIDE, including triple repetition and the rule of 50 movements.
									Tables due to insufficient material (KNN vs k, K vs knn, (KR vs k, K vs kr).
- Game mode						->	Time management (conventional, incremental and fixed time per movement) or fixed depth.


5)	LICENSE
===============================================================================================

This program is free software: you can redistribute it and / or modify it. Under the terms of the
GNU General Public License as published by the Free Software Foundation, be it the version 3 of the 
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. Watch the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.


6)	CREDIT
===============================================================================================

I am very grateful to the following people:

- To my family and friends, who are always there to help.
- To all those who have decided to share the source code of their engines (I have clarified functions and understanding of them).
- To Pedro Castro for the search function of his DanaSah engine. For your cooperation and for clarifying legal issues regarding chess engines.
- To the CPW website <a href="http://chessprogramming.wikispaces.com"> (http://chessprogramming.wikispaces.com) </a> for all the content on the techniques applicable to a chess engine.
- To the web ajedrezmagno <a href="http://ajedrezmagno.cubava.cu"> for disclosing and providing information about Nawito.
- To Dusan Stamenkovic for make a logo for Nawito.
- To Pedro Moreno for creating the books: Nawito1812book.nbk and Nawito1812MainBook.nbk
- To all those who have played with the engine and have published their experience and opinions.

