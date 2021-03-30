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


#ifndef HASH_MAT_H
#define HASH_MAT_H

#include "piece.h"



typedef	I64			HASH_MAT;

typedef	enum		HASH_MATERIAL_TYPE		{
	KK,
	KPK=1<<0,	KKP=1<<4,
	KNK=1<<8,	KKN=1<<11,
	KBK=1<<14,	KKB=1<<17,
	KRK=1<<20,	KKR=1<<23,
	KQK=1<<26,	KKQ=1<<29,

	KQKQ=KQK+KKQ,KRKR=KRK+KKR, KBKB=KBK+KKB, KNKN=KNK+KKN, KPKP=KPK+KKP, KQKR=KQK+KKR, KRKQ=KRK+KKQ, KRRK=KRK+KRK, KKRR=KKR+KKR, 
	KNKR=KNK+KKR, KBKR=KBK+KKR, KNKB=KNK+KKB,KPKN=KPK+KKN, KPKQ=KPK+KKQ, KPPKQ=KPKQ+KPK, KPPPKQ=KPPKQ+KPK,KPKB=KPK+KKB, KKNN=KKN+KKN, 
	KKBN=KKB+KKN, KKBP=KKB+KKP, KPKR=KPK+KKR, KPPKR=KPKR+KPK,KRKN=KRK+KKN, KRKB=KRK+KKB, KBKN=KBK+KKN, KNKP=KNK+KKP,KQKP=KQK+KKP, 
	KQKPP=KQKP+KKP, KQKPPP=KQKPP+KKP, KBKP=KBK+KKP, KNNK=KNK+KNK, KBNK=KBK+KNK, KBPK=KBK+KPK, KRKP=KRK+KKP, KRKPP=KRKP+KKP, 
	KKQB=KKQ+KKB, KQBK=KQK+KBK, KRBK=KRK+KBK, KKBB=KKB+KKB, KKBBB=KKBB+KKB, KKBBBB=KKBB+KKBB, KKQN=KKQ+KKN, KKRB=KKR+KKB, 
	KQKRR=KQK+KKRR, KQKQB=KQK+KKQB, KRBKQ=KRBK+KKQ, KBKBB=KBK+KKBB, KQKQN=KQK+KKQN, KRKBN=KRK+KKBN, KBKBN=KBK+KKBN, KNKBB=KNK+KKBB, 
	KBBK=KBK+KBK, KBBBK=KBBK+KBK, KBBBBK=KBBK+KBBK, KQNK=KQK+KNK, KRRKQ=KRRK+KKQ, KQBKQ=KQBK+KKQ, KQKRB=KQK+KKRB, KBBKB=KBBK+KKB, 
	KQNKQ=KQNK+KKQ, KBNKR=KBNK+KKR, KBNKB=KBNK+KKB, KBBKN=KBBK+KKN, KRNK=KRK+KNK, KKRN=KKR+KKN, KRNKQ=KRNK+KKQ, KRKRB=KRK+KKRB, 
	KRKBB=KRK+KKBB, KRKRN=KRK+KKRN, KRKNN=KRK+KKNN, KNKBN=KNK+KKBN, KNKNN=KNK+KKNN, KBKNN=KBK+KKNN, KKNP=KKN+KKP, KKRP=KKR+KKP, 
	KQKRN=KQK+KKRN, KRBKR=KRBK+KKR, KBBKR=KBBK+KKR, KRNKR=KRNK+KKR, KNNKR=KNNK+KKR, KBNKN=KBNK+KKN, KNNKN=KNNK+KKN, KNNKB=KNNK+KKB, 
	KKPP=KKP+KKP, KRPK=KRK+KPK, KRPKR=KRPK+KKR, KRKBP=KRK+KKBP, KBKBP=KBK+KKBP, KBKBPP=KBKBP+KKP, KNKBP=KNK+KKBP, KNKBPP=KNKBP+KKP, 
	KBKNP=KBK+KKNP, KRKNP=KRK+KKNP, KNKPP=KNK+KKPP, KPKNN=KPK+KKNN, KPPK=KPK+KPK, KNPK=KNK+KPK, KRKRP=KRK+KKRP, KBPKR=KBPK+KKR, 
	KBPKB=KBPK+KKB, KBPPKB=KBPKB+KPK, KBPKN=KBPK+KKN, KBPPKN=KBPKN+KPK, KNPKB=KNPK+KKB, KNPKR=KNPK+KKR, KPPKN=KPPK+KKN, 
	KNNKP=KNNK+KKP, KKPPP=KKPP+KKP, KKPPPP=KKPP+KKPP, KPPPK=KPPK+KPK, KPPPPK=KPPK+KPPK, KBKPP=KBK+KKPP, KPKPP=KPK+KKPP, 
	KNNKNN=KNNK+KKNN, KRRKRR=KRRK+KKRR, KPPKPP=KPPK+KKPP, KPPKB=KPPK+KKB, KPPKP=KPPK+KKP, KBBKBB=KBBK+KKBB, KRBNK=KRBK+KNK, 
	KPPPKPPP=KPPPK+KKPPP, KRBKBB=KRBK+KKBB, KQNKRR=KQNK+KKRR, KRBKNN=KRBK+KKNN, KBNKRB=KBNK+KKRB, KRBNKQ=KRBNK+KKQ, KBBKRN=KBBK+KKRN, 
	KKBNN=KKBN+KKN, KKRBN=KKRB+KKN, KBBKRB=KBBK+KKRB, KRRKQN=KRRK+KKQN, KNNKRB=KNNK+KKRB, KRBKBN=KRBK+KKBN, KQKRBN=KQK+KKRBN, 
	KRNKBB=KRNK+KKBB, KBNNK=KBNK+KNK, KKQP=KKQ+KKP, KQPK=KQK+KPK, KBBNK=KBBK+KNK, KQPKQ=KQPK+KKQ, KBNKNN=KBNK+KKNN, KKBBN=KKBB+KKN, 
	KQKQP=KQK+KKQP, KNNKBN=KNNK+KKBN, KKRNN=KKRN+KKN, KRNNK=KRNK+KNK, KKRBB=KKRB+KKB, KQKRNN=KQK+KKRNN, KQKBNN=KQK+KKBNN, 
	KRKBNN=KRK+KKBNN, KBBNKQ=KBBNK+KKQ, KBNKRN=KBNK+KKRN, KNNKRN=KNNK+KKRN, KBNKBB=KBNK+KKBB, KPKRN=KPK+KKRN, KRBBK=KRBK+KBK, 
	KRNNKQ=KRNNK+KKQ, KBNNKQ=KBNNK+KKQ, KBNNKR=KBNNK+KKR, KQKBBN=KQK+KKBBN, KRNKBN=KRNK+KKBN, KRNKNN=KRNK+KKNN, KBBKBN=KBBK+KKBN, 
	KRNKP=KRNK+KKP, KKRBP=KKRB+KKP, KQPKQB=KQPK+KKQB, KRPKRB=KRPK+KKRB, KBBKRP=KBBK+KKRP, KBPKBB=KBPK+KKBB, KBBKNP=KBBK+KKNP, 
	KQPKQN=KQPK+KKQN, KBBKNN=KBBK+KKNN, KRPKRN=KRPK+KKRN, KQBPK=KQBK+KPK, KQBKQP=KQBK+KKQP, KRBKRP=KRBK+KKRP, KRPKBB=KRPK+KKBB, 
	KBBKBP=KBBK+KKBP, KNPKBB=KNPK+KKBB, KQNKQP=KQNK+KKQP, KNNKBB=KNNK+KKBB, KRNKRP=KRNK+KKRP, KKBBP=KKBB+KKP, KKBBPP=KKBBP+KKP, 
	KKBBPPP=KKBBPP+KKP, KBNKRP=KBNK+KKRP, KBPKBN=KBPK+KKBN, KNPKBN=KNPK+KKBN, KNNKRP=KNNK+KKRP, KNNKBP=KNNK+KKBP, KNPKNN=KNPK+KKNN, 
	KPPKNN=KPPK+KKNN, KQKRBB=KQK+KKRBB, KBNPK=KBNK+KPK, KRPKBN=KRPK+KKBN, KBNKBP=KBNK+KKBP, KBNKNP=KBNK+KKNP, KRPKNN=KRPK+KKNN, 
	KBPKNN=KBPK+KKNN, KNNKNP=KNNK+KKNP, KNNKPP=KNNK+KKPP, KRBBKQ=KRBBK+KKQ, KKQBP=KKQB+KKP, KRBKRB=KRBK+KKRB, KBBNKRR=KBBNK+KKRR, 
	KBNNKRR=KBNNK+KKRR, KQBPKQB=KQBPK+KKQB, KRBKRBP=KRBK+KKRBP, KBBKBBP=KBBK+KKBBP, KBNPKNN=KBNPK+KKNN, KRBPK=KRBK+KPK, 
	KBBPK=KBBK+KPK, KBBPPK=KBBPK+KPK, KBBPPPK=KBBPPK+KPK, KKBNP=KKBN+KKP, KKQNP=KKQN+KKP, KRNPK=KRNK+KPK, KQNPK=KQNK+KPK, 
	KQBBK=KQBK+KBK, KKRNP=KKRN+KKP, KNNPK=KNNK+KPK, KKNNP=KKNN+KKP, KRPPK=KRPK+KPK, KQBKQB=KQBK+KKQB, KRRKBBN=KRRK+KKBBN, 
	KRRKBNN=KRRK+KKBNN, KQBKQBP=KQBK+KKQBP, KRBPKRB=KRBPK+KKRB, KBBPKBB=KBBPK+KKBB, KNNKBNP=KNNK+KKBNP, KBPPK=KBPK+KPK, 
	KBPPPK=KBPPK+KPK, KQBKQNP=KQBK+KKQNP, KRNPKRB=KRNPK+KKRB, KBNPKBB=KBNPK+KKBB, KQNKQBP=KQNK+KKQBP, KRNKRBP=KRNK+KKRBP, 
	KRNKBBP=KRNK+KKBBP, KQNPKQN=KQNPK+KKQN, KQPPK=KQPK+KPK, KQNPKQB=KQNPK+KKQB, KRBKRNP=KRBK+KKRNP, KBBKBNP=KBBK+KKBNP, 
	KQBPKQN=KQBPK+KKQN, KRBPKRN=KRBPK+KKRN, KBBPKRN=KBBPK+KKRN, KQNKQNP=KQNK+KKQNP, KKRPP=KKRP+KKP, KRNPKRN=KRNPK+KKRN, 
	KBNPKRN=KBNPK+KKRN, KBNPKBN=KBNPK+KKBN, KNNPKRN=KNNPK+KKRN, KNNPKBN=KNNPK+KKBN, KBBPKNN=KBBPK+KKNN, KRPKBNN=KRPK+KKBNN, 
	KKQPP=KKQP+KKP, KRNKRNP=KRNK+KKRNP, KRNKBNP=KRNK+KKBNP, KBNKBNP=KBNK+KKBNP, KRNKNNP=KRNK+KKNNP, KBNKNNP=KBNK+KKNNP, 
	KNNKBBP=KNNK+KKBBP, KBNNKRP=KBNNK+KKRP, KKBPP=KKBP+KKP, KKBPPP=KKBPP+KKP, KNNPKNN=KNNPK+KKNN, KQPPKQB=KQPPK+KKQB, 
	KRPPKRB=KRPPK+KKRB, KBPPKBB=KBPPK+KKBB, KQPPKQN=KQPPK+KKQN, KRPPKRN=KRPPK+KKRN, KBPPKBN=KBPPK+KKBN, KNPPK=KNPK+KPK, 
	KNNKNNP=KNNK+KKNNP, KQBKQPP=KQBK+KKQPP, KRBKRPP=KRBK+KKRPP, KBBKBPP=KBBK+KKBPP, KQNKQPP=KQNK+KKQPP, KRNKRPP=KRNK+KKRPP, 
	KBNKBPP=KBNK+KKBPP, KKNPP=KKNP+KKP, KNPPKBN=KNPPK+KKBN, KRPPKBNN=KRPPK+KKBNN, KNPPKNN=KNPPK+KKNN, KPPPKNN=KPPPK+KKNN, 
	KRBKBBP=KRBK+KKBBP, KBNKBBP=KBNK+KKBBP, KBBKNNP=KBBK+KKNNP, KBNKNPP=KBNK+KKNPP, KBNNKRPP=KBNNK+KKRPP, KNNKNPP=KNNK+KKNPP, 
	KNNKPPP=KNNK+KKPPP, KBBPKRB=KBBPK+KKRB, KBBPKBN=KBBPK+KKBN, KNNPKBB=KNNPK+KKBB, KPPPKPP=KPPPK+KKPP, KPPKPPP=KPPK+KKPPP,
}	HASH_MATERIAL;


// Números pseudo aleatorios para la hash material
static const HASH_MAT	HashMatKey[16]		= {
	0,		0,
	KKP,	KPK,
	KKN,	KNK,
	KKB,	KBK,
	KKR,	KRK,
	KKQ,	KQK,
	 KK,	KK,
	0,		0,
};


#endif // HASH_MAT_H
