#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "sph/extra.h"
#include "sph/sph_blake.h"
#include "sph/sph_bmw.h"
#include "sph/sph_groestl.h"
#include "sph/sph_keccak.h"
#include "sph/sph_skein.h"
#include "sph/sph_luffa.h"
#include "sph/sph_cubehash.h"
#include "sph/sph_shavite.h"
#include "sph/sph_simd.h"
#include "sph/sph_echo.h"
#include "sph/sph_hamsi.h"
#include "sph/sph_fugue.h"
#include "sph/sph_shabal.h"
#include "sph/sph_whirlpool.h"
#include "cryptonote/cryptonight_dark.h"
#include "cryptonote/cryptonight_dark_lite.h"
#include "cryptonote/cryptonight_fast.h"
#include "cryptonote/cryptonight.h"
#include "cryptonote/cryptonight_lite.h"
#include "cryptonote/cryptonight_turtle.h"
#include "cryptonote/cryptonight_turtle_lite.h"

enum Algo {
        BLAKE = 0,
        BMW,
        GROESTL,
        KECCAK,
        SKEIN,
        LUFFA,
        CUBEHASH,
        SHAVITE,
        SIMD,
        ECHO,
        HAMSI,
        FUGUE,
        SHABAL,
        WHIRLPOOL,
        HASH_FUNC_COUNT
};

enum CNAlgo {
	CNDark = 0,
	CNDarklite,
	CNFast,
	CNLite,
	CNTurtle,
	CNTurtlelite,
	CN_HASH_FUNC_COUNT
};

static void selectAlgo(unsigned char nibble, bool* selectedAlgos, uint8_t* selectedIndex, int algoCount, int* currentCount) {
	uint8_t algoDigit = (nibble & 0x0F) % algoCount;
	if(!selectedAlgos[algoDigit]) {
		selectedAlgos[algoDigit] = true;
		selectedIndex[currentCount[0]] = algoDigit;
		currentCount[0] = currentCount[0] + 1;
	}
	algoDigit = (nibble >> 4) % algoCount;
	if(!selectedAlgos[algoDigit]) {
		selectedAlgos[algoDigit] = true;
		selectedIndex[currentCount[0]] = algoDigit;
		currentCount[0] = currentCount[0] + 1;
	}
}

static void getAlgoString(void *mem, unsigned int size, uint8_t* selectedAlgoOutput, int algoCount) {
  int i;
  unsigned char *p = (unsigned char *)mem;
  unsigned int len = size/2;
  bool selectedAlgo[15] = {false};
  int selectedCount = 0;
  for (i=0;i<len; i++) {
	  selectAlgo(p[i], selectedAlgo, selectedAlgoOutput, algoCount, &selectedCount);
	  if(selectedCount == algoCount) {
		  break;
	  }
  }
  if(selectedCount < algoCount) {
	for(uint8_t i = 0; i < algoCount; i++) {
		if(!selectedAlgo[i]) {
			selectedAlgoOutput[selectedCount] = i;
			selectedCount++;
		}
	}
  }
}

void print_hex_memory(void *mem, unsigned int size) {
  int i;
  unsigned char *p = (unsigned char *)mem;
  unsigned int len = size/2;
  for (i=0;i<len; i++) {
    printf("%02x", p[(len - i - 1)]);
  }
  printf("\n");
}

void SwapBytes(void *pv, unsigned int n)
{
    char *p = pv;
    unsigned int lo, hi;
    for(lo=0, hi=n-1; hi>lo; lo++, hi--)
    {
        char tmp=p[lo];
        p[lo] = p[hi];
        p[hi] = tmp;
    }
}

void flex_hash(const char* input, unsigned char* output) {
	uint32_t hash[64/4];
	sph_blake512_context ctx_blake;
	sph_bmw512_context ctx_bmw;
	sph_groestl512_context ctx_groestl;
	sph_keccak512_context ctx_keccak;
	sph_skein512_context ctx_skein;
	sph_luffa512_context ctx_luffa;
	sph_cubehash512_context ctx_cubehash;
	sph_shavite512_context ctx_shavite;
	sph_simd512_context ctx_simd;
	sph_echo512_context ctx_echo;
	sph_hamsi512_context ctx_hamsi;
	sph_fugue512_context ctx_fugue;
	sph_shabal512_context ctx_shabal;
	sph_whirlpool_context ctx_whirlpool;

	void *in = (void*) input;
	int size = 80;
    sph_keccak512_init(&ctx_keccak);
    sph_keccak512(&ctx_keccak, in, size);
    sph_keccak512_close(&ctx_keccak, hash);
	uint8_t selectedAlgoOutput[15] = {0};
	uint8_t selectedCNAlgoOutput[6] = {0};
	getAlgoString(&hash, 64, selectedAlgoOutput, HASH_FUNC_COUNT);
	getAlgoString(&hash, 64, selectedCNAlgoOutput, CN_HASH_FUNC_COUNT);
	//printf("previous hash=");
	//print_hex_memory(selectedAlgoOutput, 30);
	//print_hex_memory(selectedCNAlgoOutput, 12);
	int i;
	for (i = 0; i < 18; i++)
	{
		uint8_t algo;
		uint8_t cnAlgo;
		int coreSelection;
		int cnSelection = -1;
		if(i < 5) {
			coreSelection = i;
		} else if(i < 11) {
			coreSelection = i-1;
		} else {
			coreSelection = i-2;
		}
		if(i==5) {
			coreSelection = -1;
			cnSelection = 0;
		}
		if(i==11) {
			coreSelection = -1;
			cnSelection = 1;
		}
		if(i==17) {
			coreSelection = -1;
			cnSelection = 2;
		}
		if(coreSelection >= 0) {
			algo = selectedAlgoOutput[(uint8_t)coreSelection];
		} else {
			algo = HASH_FUNC_COUNT + 1; // skip core hashing for this loop iteration
		}
		if(cnSelection >=0) {
			cnAlgo = selectedCNAlgoOutput[(uint8_t)cnSelection];
		} else {
			cnAlgo = CN_HASH_FUNC_COUNT + 1; // skip cn hashing for this loop iteration
		}
		//selection cnAlgo. if a CN algo is selected then core algo will not be selected
		switch(cnAlgo)
		{
		 case CNDark:
            //printf("... CNDark\n");
			cryptonightdark_hash(in, hash, size, 1);
			break;
		 case CNDarklite:
            //printf("... CNDarklite\n");
			cryptonightdarklite_hash(in, hash, size, 1);
			break;
		 case CNFast:
            //printf("... CNFast\n");
			cryptonightfast_hash(in, hash, size, 1);
			break;
		 case CNLite:
            //printf("... CNLite\n");
			cryptonightlite_hash(in, hash, size, 1);
			break;
		 case CNTurtle:
            //printf("... CNTurtle\n");
			cryptonightturtle_hash(in, hash, size, 1);
			break;
		 case CNTurtlelite:
            //printf("... CNTurtlelite\n");
			cryptonightturtlelite_hash(in, hash, size, 1);
			break;
		}
		//selection core algo
		switch (algo) {
		case BLAKE:
                //printf("BLAKE\n");
				sph_blake512_init(&ctx_blake);
				sph_blake512(&ctx_blake, in, size);
				sph_blake512_close(&ctx_blake, hash);
				break;
		case BMW:
                //printf("BMW\n");
				sph_bmw512_init(&ctx_bmw);
				sph_bmw512(&ctx_bmw, in, size);
				sph_bmw512_close(&ctx_bmw, hash);
				break;
		case GROESTL:
                //printf("GROESTL\n");
				sph_groestl512_init(&ctx_groestl);
				sph_groestl512(&ctx_groestl, in, size);
				sph_groestl512_close(&ctx_groestl, hash);
				break;
		case KECCAK:
                //printf("KECCAK\n");
				sph_keccak512_init(&ctx_keccak);
				sph_keccak512(&ctx_keccak, in, size);
				sph_keccak512_close(&ctx_keccak, hash);
				break;
		case SKEIN:
                //printf("SKEIN\n");
				sph_skein512_init(&ctx_skein);
				sph_skein512(&ctx_skein, in, size);
				sph_skein512_close(&ctx_skein, hash);
				break;
		case LUFFA:
                //printf("LUFFA\n");
				sph_luffa512_init(&ctx_luffa);
				sph_luffa512(&ctx_luffa, in, size);
				sph_luffa512_close(&ctx_luffa, hash);
				break;
		case CUBEHASH:
                //printf("CUBEHASH\n");
				sph_cubehash512_init(&ctx_cubehash);
				sph_cubehash512(&ctx_cubehash, in, size);
				sph_cubehash512_close(&ctx_cubehash, hash);
				break;
		case SHAVITE:
                //printf("SHAVITE\n");
				sph_shavite512_init(&ctx_shavite);
				sph_shavite512(&ctx_shavite, in, size);
				sph_shavite512_close(&ctx_shavite, hash);
				break;
		case SIMD:
                //printf("SIMD\n");
				sph_simd512_init(&ctx_simd);
				sph_simd512(&ctx_simd, in, size);
				sph_simd512_close(&ctx_simd, hash);
				break;
		case ECHO:
                //printf("ECHO\n");
				sph_echo512_init(&ctx_echo);
				sph_echo512(&ctx_echo, in, size);
				sph_echo512_close(&ctx_echo, hash);
				break;
		case HAMSI:
                //printf("HAMSI\n");
				sph_hamsi512_init(&ctx_hamsi);
				sph_hamsi512(&ctx_hamsi, in, size);
				sph_hamsi512_close(&ctx_hamsi, hash);
				break;
		case FUGUE:
                //printf("FUGUE\n");
				sph_fugue512_init(&ctx_fugue);
				sph_fugue512(&ctx_fugue, in, size);
				sph_fugue512_close(&ctx_fugue, hash);
				break;
		case SHABAL:
                //printf("SHABAL\n");
				sph_shabal512_init(&ctx_shabal);
				sph_shabal512(&ctx_shabal, in, size);
				sph_shabal512_close(&ctx_shabal, hash);
				break;
		case WHIRLPOOL:
                //printf("WHIRLPOOL\n");
				sph_whirlpool_init(&ctx_whirlpool);
				sph_whirlpool(&ctx_whirlpool, in, size);
				sph_whirlpool_close(&ctx_whirlpool, hash);
				break;
		}
/* 		if(cnSelection >= 0) {
			memset(&hash[8], 0, 32);
		} */
		in = (void*) hash;
		size = 64;
	}
    sph_keccak256_init(&ctx_keccak);
    sph_keccak256(&ctx_keccak, in, size);
    sph_keccak256_close(&ctx_keccak, hash);
	memcpy(output, hash, 32);
}
