/*
 * hash_selection.h
 *
 *  Created on: May 11, 2018
 *      Author: tri
 */

#ifndef helios_SELECTION_H_
#define helios_SELECTION_H_

#include "uint256.h"
#include <string>
#include <vector>

#include "crypto/sph_blake.h"
#include "crypto/sph_bmw.h"
#include "crypto/sph_groestl.h"
#include "crypto/sph_jh.h"
#include "crypto/sph_keccak.h"
#include "crypto/sph_skein.h"
#include "crypto/sph_luffa.h"
#include "crypto/sph_cubehash.h"
#include "crypto/sph_shavite.h"
#include "crypto/sph_simd.h"
#include "crypto/sph_echo.h"
#include "crypto/sph_hamsi.h"
#include "crypto/sph_fugue.h"
#include "crypto/sph_shabal.h"
#include "crypto/sph_whirlpool.h"
extern "C" {
#include "crypto/sph_sha2.h"
}

extern std::vector<std::vector<int>> INITIAL_GROUP;

struct ContextHash {
	void* contextObj;
	void (*initFunc)(void* contextObj);
	void (*calculateFunc)(void* contextObj, const void* toHash, size_t lenToHash);
	void (*closeFunc)(void* contextObj, void* hash);
	ContextHash(void* contextObj,
			void (*initFunc)(void* contextObj),
			void (*calculateFunc)(void* contextObj, const void* toHash, size_t lenToHash),
			void (*closeFunc)(void* contextObj, void* hash)){
		this->contextObj = contextObj;
		this->initFunc = initFunc;
		this->calculateFunc = calculateFunc;
		this->closeFunc = closeFunc;
	}

	void init(void* contextObj,
				void (*initFunc)(void* contextObj),
				void (*calculateFunc)(void* contextObj, const void* toHash, size_t lenToHash),
				void (*closeFunc)(void* contextObj, void* hash)){
			this->contextObj = contextObj;
			this->initFunc = initFunc;
			this->calculateFunc = calculateFunc;
			this->closeFunc = closeFunc;
		}
	ContextHash() {
	}
	~ContextHash() {

	}
	void calculateHash( const void* toHash, size_t lenToHash, void* hash) {
		initFunc(contextObj);
		calculateFunc(contextObj, toHash, lenToHash);
		closeFunc(contextObj, hash);
	}
};

struct ContextList {
	static void initContext(ContextList &contextList, int groupNum) {
		/*switch(groupNum) {
		case 1:
			contextList.initHashx16r();
			break;
		case 2:
			contextList.initGroup1();
			break;
		}*/
		contextList.initGroup(INITIAL_GROUP[groupNum]);
	}

	sph_blake512_context     ctx_blake;      //0
	sph_bmw512_context       ctx_bmw;        //1
	sph_groestl512_context   ctx_groestl;    //2
	sph_jh512_context        ctx_jh;         //3
	sph_keccak512_context    ctx_keccak;     //4
	sph_skein512_context     ctx_skein;      //5
	sph_luffa512_context     ctx_luffa;      //6
	sph_cubehash512_context  ctx_cubehash;   //7
	sph_shavite512_context   ctx_shavite;    //8
	sph_simd512_context      ctx_simd;       //9
	sph_echo512_context      ctx_echo;       //A
	sph_hamsi512_context     ctx_hamsi;      //B
	sph_fugue512_context     ctx_fugue;      //C
	sph_shabal512_context    ctx_shabal;     //D
	sph_whirlpool_context    ctx_whirlpool;  //E
	sph_sha512_context       ctx_sha512;     //F
	std::vector<ContextHash> algoHashes;
	void initGroup(std::vector<int> groupIndexes) {
		ContextHash blakeContext(&ctx_blake, sph_blake512_init, sph_blake512, sph_blake512_close);
		ContextHash bmwContext(&ctx_bmw, sph_bmw512_init, sph_bmw512, sph_bmw512_close);
		ContextHash groestlContext(&ctx_groestl, sph_groestl512_init,sph_groestl512, sph_groestl512_close);
		ContextHash jhContext(&ctx_jh,sph_jh512_init,sph_jh512,sph_jh512_close);
		ContextHash keccakConext(&ctx_keccak, sph_keccak512_init, sph_keccak512, sph_keccak512_close);
		ContextHash skeinContext(&ctx_skein, sph_skein512_init,sph_skein512,sph_skein512_close);
		ContextHash luffaContext(&ctx_luffa, sph_luffa512_init,sph_luffa512,sph_luffa512_close);
		ContextHash cubehashContext(&ctx_cubehash, sph_cubehash512_init,sph_cubehash512,sph_cubehash512_close);
		ContextHash shaviteContext(&ctx_shavite, sph_shavite512_init, sph_shavite512, sph_shavite512_close);
		ContextHash simdContext(&ctx_simd, sph_simd512_init, sph_simd512, sph_simd512_close);
		ContextHash echoContext(&ctx_echo, sph_echo512_init, sph_echo512, sph_echo512_close);
		ContextHash hamsiContext(&ctx_hamsi, sph_hamsi512_init, sph_hamsi512, sph_hamsi512_close);
		ContextHash fugueContext(&ctx_fugue, sph_fugue512_init, sph_fugue512, sph_fugue512_close);
		ContextHash shabalContext(&ctx_shabal, sph_shabal512_init, sph_shabal512, sph_shabal512_close);
		ContextHash whirlpoolContext(&ctx_whirlpool, sph_whirlpool_init, sph_whirlpool, sph_whirlpool_close);
		ContextHash sha512Context(&ctx_sha512, sph_sha512_init, sph_sha512, sph_sha512_close);
		int size = groupIndexes.size();
		for(int i=0; i < size; i++) {
			int index = groupIndexes[i];
			switch(index) {
			case 0:
				algoHashes.push_back(blakeContext);
				break;
			case 1:
				algoHashes.push_back(bmwContext);
				break;
			case 2:
				algoHashes.push_back(groestlContext);
				break;
			case 3:
				algoHashes.push_back(jhContext);
				break;
			case 4:
				algoHashes.push_back(keccakConext);
				break;
			case 5:
				algoHashes.push_back(skeinContext);
				break;
			case 6:
				algoHashes.push_back(luffaContext);
				break;
			case 7:
				algoHashes.push_back(cubehashContext);
				break;
			case 8:
				algoHashes.push_back(shaviteContext);
				break;
			case 9:
				algoHashes.push_back(simdContext);
				break;
			case 10:
				algoHashes.push_back(echoContext);
				break;
			case 11:
				algoHashes.push_back(hamsiContext);
				break;
			case 12:
				algoHashes.push_back(fugueContext);
				break;
			case 13:
				algoHashes.push_back(shabalContext);
				break;
			case 14:
				algoHashes.push_back(whirlpoolContext);
				break;
			case 15:
				algoHashes.push_back(sha512Context);
				break;
			}
		}
	}

	void initHashx16r(void) {
		std::vector<int> groupIndexes = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
		initGroup(groupIndexes);
	}

	void initGroup1() {
		std::vector<int> groupIndexes = {6,7,8,9,10};
		initGroup(groupIndexes);
	}

};


class HashSelection {
public:
/*HashSelection(const uint256 preBlockHash) {
		this->PrevBlockHash = preBlockHash;
		this->size = 5;
		this->algoMap = {
				"Blake",      //0
				"Bmw",        //1
				"Groestl",    //2
				"Jh",         //3
				"Keccak",     //4
				"Skein",      //5
				"Luffa",      //6
				"Cubehash",   //7
				"Shavite",    //8
				"Simd",       //9
				"Echo",       //A
				"Jamsi",      //B
				"Fugue",      //C
				"Shabal",     //D
				"Whirlpool",  //E
				"Sha512"     //F
		};
		int blockheight = chainActive.Height();
	}*/
	HashSelection(const uint256 prevBlockHash, const int size, const std::vector<int> algoGroups) {
		this->PrevBlockHash = prevBlockHash;
		this->size = size;
		this->algoMap = {
				"Blake",      //0
				"Bmw",        //1
				"Groestl",    //2
				"Jh",         //3
				"Keccak",     //4
				"Skein",      //5
				"Luffa",      //6
				"Cubehash",   //7
				"Shavite",    //8
				"Simd",       //9
				"Echo",       //A
				"Jamsi",      //B
				"Fugue",      //C
				"Shabal",     //D
				"Whirlpool",  //E
				"Sha512"     //F
		};
		this->algoGroups = algoGroups;
	}
	int getHashSelection(int index);
	static int getGroupHashSelection(uint256 blockHash);
	std::string getHashSelectionString();
protected:
	uint256 PrevBlockHash;
	int size;
	std::vector<int> algoGroups;
	std::vector<std::string> algoMap;

};

#endif /* helios_SELECTION_H_ */
