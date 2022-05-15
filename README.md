# Nawito2205 - Cuban Chess Engine by Ernesto Torres

*Engine version \
  22.05
*Release date \
  15-05-2022
*Available versions \
  Windows/Linux/Android, 64 bits
*Communication protocol \
  UCI
*ELO estimated:
  2650
*License: \
  GPL
*email: \
  NawitoChessEngine@yandex.com
*github: \
  https://github.com/etorresf/Nawito64Bit


# TABLE OF CONTENTS
1. General description
2. Content
3. Performance
4. Features
5. License
6. Credits


# 1. GENERAL DESCRIPTION

Nawito22.05 is a free Cuban chess engine derived from Danasah5.07 compatible with the UCI protocol. 
To get the most out of it, it should be used under a graphical user interface(*GUI*) like: 
Arena *(Linux/Windows)*, Winboard *(Windows)*, Xboard *(Linux)*, Pychess *(Linux/Windows)*, 
SCID *(Linux/Windows)*, DroidFish *(Android)*.

Its author has been based on the search function of the Danasah5.07 engine and inspired in other  
*(TSCP, CPW, Rebel, Mini Rodent, Fruit, others)*. To develop your own ideas and give it a particular style.

It is develop with free tools and open source code, under C language and Clang compiler.
Available versions for: Windows, Linux, Android.

Nawito use the polyglot opening book scheme, with the internal engine hash key synchronized to this schemes.

Please report any ideas, suggestions or errors about the engine.


# 2. CONTENT

* README.md \
  This file
* GPL-LICENSE.txt \
  Engine license
* history.txt \
  Release notes
* /Windows
  * Nawito-22.05-windows-x64.exe \
    Engine for Windows *(64 bits)*
  * Nawito.ini \
    Configuration file of Windows engine
  * logo.jpg \
    Logo of Windows program *(by Dusan Stamenkovic)*
* /Linux
  * Nawito-22.05-linux-x64 \
    Engine for Linux *(64 bits)*
  * Nawito.ini \
    Configuration file of Linux engine
  * logo.jpg \
    Logo of Linux program *(by Dusan Stamenkovic)*
* /Android
  * Nawito-22.05-arm7-x64 \
      Engine for Android *(64 bits)*
  * Nawito.ini \
    Configuration file of Android engine
  * logo.jpg \
    Logo of Android program *(by Dusan Stamenkovic)*
* /src \
  Program code
  * Makefile \
    Program compilation manager
* /Books \
  Chess engine opening books *(polyglot format)*
* /Logos \
  Various logos of the engine

# 3. PERFORMANCE

* perft <n>  Command to discover faults or measure move generator speed to the depth <n> *(by default 5)*
    from this position: *r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -*
* bench
    Command to check the relative speed of the computer with respect to another.
    Solve mate from this position: *r4nk1/4brp1/q2p1pQP/3P4/2p1NP2/P7/1B6/1KR3R1 w - - 1 34*


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
  * Internal iterative deepening *(IID)*
  * Reductions/Prune
    * Checkmate distance
    * Null moveme
    * Transposition table
    * Futility
    * Razoring
    * Captures
    * Late Move Reductions *(LMR)*
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
  * Endings *(KPK, KBNK, KRK, KBBK, KRPKR, others)*
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
If not, see *http://www.gnu.org/licenses/*


# 6. CREDIT

I am very grateful to the following people:

* To my family and friends, who are always there to help
* To all those who have decided to share the source code of their engines *(I have clarified functions and understanding of them)*
* To Pedro Castro for the search function of his DanaSah engine. For your cooperation and for clarifying legal issues regarding chess engines
* To the CPW website *http://chessprogramming.wikispaces.com* for all the content on the techniques applicable to a chess engine
* To the web ajedrezmagno *http://ajedrezmagno.cubava.cu* for disclosing and providing information about Nawito
* To Dusan Stamenkovic for make a logo for Nawito
* To all those who have played with the engine and have published their experience and opinions


