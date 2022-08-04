# Nawito-22.07 - Cuban Chess Engine by Ernesto Torres

**Engine version:** 22.07  
**Release date:** 31-07-2022  
**Available versions:** Windows/Linux/Android _(64 bits)_
**Communication protocol:** UCI  
**ELO estimated:** 2690  
**License:** GPL  
**email:** NawitoChessEngine@yandex.com  
**github:** https://github.com/etorresf/Nawito64Bit  


# TABLE OF CONTENTS
1. General description
2. Content
3. Performance
4. Features
5. License
6. Credits


# 1. GENERAL DESCRIPTION

Nawito-22.07 is a free Cuban chess engine derived from Danasah5.07 compatible with the UCI protocol. 
To get the most out of it, it should be used under a graphical user interface _(GUI)_ like: 
Arena _(Linux/Windows)_, Winboard _(Windows)_, Xboard _(Linux)_, Pychess _(Linux/Windows)_, 
SCID _(Linux/Windows)_, DroidFish _(Android)_, Chess for Android _(Android)_ and others.

Its author has been based on the search function of the Danasah5.07 engine and inspired in other  
_(TSCP, CPW, Rebel, Mini Rodent, Fruit, others)_. To develop your own ideas and give it a particular style.

It is develop with free tools and open source code, under C language and Clang compiler.
Available versions for: Windows, Linux, Android.

Nawito use the polyglot opening book scheme, with the internal engine hash key synchronized to this schemes.

Please report any ideas, suggestions or errors about the engine.


# 2. CONTENT

* README.md  
  This file
* GPL-LICENSE.txt  
  Engine license
* history.txt  
  Release notes
* /Windows
  * Nawito-22.07-windows-x64.exe  
    Engine for Windows _(64 bits)_
  * Nawito.ini  
    Configuration file of Windows engine
  * logo.jpg  
    Logo of Windows program _(by Dusan Stamenkovic)_
* /Linux
  * Nawito-22.07-linux-x64  
    Engine for Linux _(64 bits)_
  * Nawito.ini  
    Configuration file of Linux engine
  * logo.jpg  
    Logo of Linux program _(by Dusan Stamenkovic)_
* /Android
  * Nawito-22.07-arm7-x64  
      Engine for Android _(64 bits)_
  * Nawito.ini  
    Configuration file of Android engine
  * logo.jpg  
    Logo of Android program _(by Dusan Stamenkovic)_
* /src  
  Program code
  * Makefile  
    Program compilation manager
* /Books  
  Chess engine opening books _(polyglot format)_
* /EvalParam  
  Configuration of evaluation parameters  
* /Logos  
  Various logos of the engine

# 3. PERFORMANCE

* perft <n>  
    Command to discover faults or measure move generator speed to the depth <n> _(by default 5)_  
    from this position: _r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -_
* bench
    Command to check the relative speed of the computer with respect to another.  
    Solve mate from this position: _r4nk1/4brp1/q2p1pQP/3P4/2p1NP2/P7/1B6/1KR3R1 w - - 1 34_


# 4. FEATURES

* Protocol
  * UCI
* Board representation
  * Bitboard
* Move Generator
  * Bitboard
* Search
  * Aspiration window
  * Iterative depth
  * PVS/Alpha-Beta
  * Quiescence
  * Internal iterative deepening _(IID)_
  * Reductions/Prune
    * Checkmate distance
    * Null moveme
    * Transposition table
    * Futility
    * Razoring
    * Captures
    * Late Move Reductions _(LMR)_
  * Extensions
    * Check
    * Checkmate threat
    * Pawns end
* Move ordering
  1. PV
  2. Transposition table
  3. MVV/LVA
  4. Killer heuristic
  5. Historical heuristic
* Evaluation
  * Tapered eval
  * Material
    * Piece material
    * Knight decreasing value as pawns disappear
    * Rook increasing value as pawns disappear
    * Bishop pair
  * Pawns struct
    * Backward pawn
    * Advanced pawn
    * Candidate pawns
    * Doubled pawns
    * Isolated pawn
    * Passed pawn
  * King seafty
    * King seafty
    * King shield
    * King tropism
  * Mobility
    * Knight mobility
    * Bishop mobility
    * Rook mobility
    * Queen mobility
    * Traped pieces
      * Traped knight
      * Traped bishop
      * Traped rook
      * Outpost knight
    * Bad bishop
    * Rook in open o semi-open file abierta
    * Rook in 7th
  * Endings _(KPK, KBNK, KRK, KBBK, KRPKR, others)_
  * Tables due to insufficient material
  * Others
* Time management
  * conventional
  * incremental
  * fixed time per movement
  * fixed time per depth
* Opening Book
  * Polyglot opening book scheme
* Others
  * Detection of the end of the game with the rules of FIDE, including triple repetition and 50 movements rule


# 5. LICENSE

This program is free software: you can redistribute it and/or modify it. Under the terms of the
GNU General Public License as published by the Free Software Foundation, be it the version 3 of the 
License, or (at your option) any later version

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. Watch the
GNU General Public License for more details

You should have received a copy of the GNU General Public License along with this program
If not, see _http://www.gnu.org/licenses/_


# 6. CREDIT

I am very grateful to the following people:

* To my family and friends, who are always there to help
* To all those who have decided to share the source code of their engines _(I have clarified functions and understanding of them)_
* To Pedro Castro for the search function of his DanaSah engine. For your cooperation and for clarifying legal issues regarding chess engines
* To the CPW website _http://chessprogramming.wikispaces.com_ for all the content on the techniques applicable to a chess engine
* To the web ajedrezmagno _http://ajedrezmagno.cubava.cu_ for disclosing and providing information about Nawito
* To Dusan Stamenkovic for make a logo for Nawito
* To all those who have played with the engine and have published their experience and opinions
