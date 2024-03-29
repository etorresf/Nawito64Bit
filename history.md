# Release Notes - Nawito-22.07
* Add new eval terms.  
* Tuning eval with 'Texel Tuning'.  
* Minors corrections to time control.  
* Minors corrections to endgames.  


# Release Notes - Nawito-22.05
* Replaced Xboard by UCI protocol.  
* Replaced own book scheme by polygot scheme.  
* New book 'Asere2205.bin', based on CCRl games.  
* New file for the random numbers used on generation of de plygot key.  
* New make file with clang compiler by default.  
* New formula for delta prunning values.  
* New formula for delta null reduction.  
* Removed legals moves on check extension.  
* Unified move ordering functions for PVS and Qsearch.  
* The Principal Variation are on a unidimensional array now.  
* Minors corrections of history algorithm.  
* New eval more simple controlled by 144 parameters.  
* New formula for calculate the lazy eval margen.  
* New pawn hash.  
* New eval configuration file, see 'eval_param.ini'.  
* New file for the 'see' function.  
* Replaced code of time control by approximated formula found by differential evolution.  
* New types of distances betwen squares.  
* Removed multiples profiles of configuration file.  
* More simple configuration file.  
* Removed file of engine configuration, now the other files have your own configuration manager.  
* Removed some innecesary code.  
* More readability and organization.  
* More compact code.  
* Others minors corrections.  


# Release Notes - Nawito2103
* New code style.  
* More compact code.  
* Removed the sts test.  
* New PutPiece, QuitPiece, MovePiece for update the: board, hash, hash_material, number of pieces, etc.  
* More detailed transposition table replacement strategy.  
* New hash material support.  
* New sort quiet moves by SEE, in the search not in move generator.  
* Correction to some eval terms like: piece material, material exchange, bishop pair, bad bishop, pieces mobility and king safety.  
* New game phase detection.  
* Separate de piece value of PST.  
* New book scheme.  
* New book 'Asere2103.nbk', based on CCRl games.  
* Others minors corrections.  


# Release Notes - Nawito2012
* Solved bugs in the communication protocol.  
* New time control based on average of material and moves.  
* New book scheme.  
* New board representation(Bitboard).  
* New transposition table replacement strategy.  
* New hash age update.  
* New hash material values.  
* Solved bug with the values of hash material.  
* Solved bugs in some supported endgames.  
* Removed center control.  
* New bishop mobility.  
* New condition for bad bishops.  
* Add tropism to the King.  
* Changes on King Security.  
* Add advanced pawn.  
* New knowledge off pawn struct.  
* News supported endgames.  
* Some others minors change on the evaluation.  
* New Search Root.  
* New LMR formula.  
* New razoring formula and conditions.  
* Minors changes on Null Move.  
* Minors changes on history heuristic, matekillers and Killers.  


# Release Notes - Nawito1906
* New condition for bad captures.  
* New own book scheme, with learning based on ant colony algorithm.  
* New position for the bench test.  
* New values for MATE and game phases.  
* More big hash material value.  
* New transposition table replacement strategy based on Bucket systems.  
* The calculation of transposition table index now is based on power of two.  
* Now the transposition table delete the old entries.  
* New output for the STS test.  
* Solved bug with a empty profile.  
* Solved bugs with some supported endgames.  
