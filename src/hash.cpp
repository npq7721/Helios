// Copyright (c) 2013-2016 The Bitcoin Core developers
// Copyright (c) 2017 The Helios Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "hash.h"
#include "crypto/common.h"
#include "crypto/hmac_sha512.h"
#include "pubkey.h"

//TODO remove these
double algoHashTotal[16];
int algoHashHits[16];

inline uint32_t ROTL32(uint32_t x, int8_t r)
{
    return (x << r) | (x >> (32 - r));
}

unsigned int MurmurHash3(unsigned int nHashSeed, const std::vector<unsigned char>& vDataToHash)
{
    // The following is MurmurHash3 (x86_32), see http://code.google.com/p/smhasher/source/browse/trunk/MurmurHash3.cpp
    uint32_t h1 = nHashSeed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    const int nblocks = vDataToHash.size() / 4;

    //----------
    // body
    const uint8_t* blocks = vDataToHash.data();

    for (int i = 0; i < nblocks; ++i) {
        uint32_t k1 = ReadLE32(blocks + i*4);

        k1 *= c1;
        k1 = ROTL32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = ROTL32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    //----------
    // tail
    const uint8_t* tail = vDataToHash.data() + nblocks * 4;

    uint32_t k1 = 0;

    switch (vDataToHash.size() & 3) {
        case 3:
            k1 ^= tail[2] << 16;
        case 2:
            k1 ^= tail[1] << 8;
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = ROTL32(k1, 15);
            k1 *= c2;
            h1 ^= k1;
    }

    //----------
    // finalization
    h1 ^= vDataToHash.size();
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

void BIP32Hash(const ChainCode &chainCode, unsigned int nChild, unsigned char header, const unsigned char data[32], unsigned char output[64])
{
    unsigned char num[4];
    num[0] = (nChild >> 24) & 0xFF;
    num[1] = (nChild >> 16) & 0xFF;
    num[2] = (nChild >>  8) & 0xFF;
    num[3] = (nChild >>  0) & 0xFF;
    CHMAC_SHA512(chainCode.begin(), chainCode.size()).Write(&header, 1).Write(data, 32).Write(num, 4).Finalize(output);
}

#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define SIPROUND do { \
    v0 += v1; v1 = ROTL(v1, 13); v1 ^= v0; \
    v0 = ROTL(v0, 32); \
    v2 += v3; v3 = ROTL(v3, 16); v3 ^= v2; \
    v0 += v3; v3 = ROTL(v3, 21); v3 ^= v0; \
    v2 += v1; v1 = ROTL(v1, 17); v1 ^= v2; \
    v2 = ROTL(v2, 32); \
} while (0)

CSipHasher::CSipHasher(uint64_t k0, uint64_t k1)
{
    v[0] = 0x736f6d6570736575ULL ^ k0;
    v[1] = 0x646f72616e646f6dULL ^ k1;
    v[2] = 0x6c7967656e657261ULL ^ k0;
    v[3] = 0x7465646279746573ULL ^ k1;
    count = 0;
    tmp = 0;
}

CSipHasher& CSipHasher::Write(uint64_t data)
{
    uint64_t v0 = v[0], v1 = v[1], v2 = v[2], v3 = v[3];

    assert(count % 8 == 0);

    v3 ^= data;
    SIPROUND;
    SIPROUND;
    v0 ^= data;

    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = v3;

    count += 8;
    return *this;
}

CSipHasher& CSipHasher::Write(const unsigned char* data, size_t size)
{
    uint64_t v0 = v[0], v1 = v[1], v2 = v[2], v3 = v[3];
    uint64_t t = tmp;
    int c = count;

    while (size--) {
        t |= ((uint64_t)(*(data++))) << (8 * (c % 8));
        c++;
        if ((c & 7) == 0) {
            v3 ^= t;
            SIPROUND;
            SIPROUND;
            v0 ^= t;
            t = 0;
        }
    }

    v[0] = v0;
    v[1] = v1;
    v[2] = v2;
    v[3] = v3;
    count = c;
    tmp = t;

    return *this;
}

uint64_t CSipHasher::Finalize() const
{
    uint64_t v0 = v[0], v1 = v[1], v2 = v[2], v3 = v[3];

    uint64_t t = tmp | (((uint64_t)count) << 56);

    v3 ^= t;
    SIPROUND;
    SIPROUND;
    v0 ^= t;
    v2 ^= 0xFF;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    return v0 ^ v1 ^ v2 ^ v3;
}

uint64_t SipHashUint256(uint64_t k0, uint64_t k1, const uint256& val)
{
    /* Specialized implementation for efficiency */
    uint64_t d = val.GetUint64(0);

    uint64_t v0 = 0x736f6d6570736575ULL ^ k0;
    uint64_t v1 = 0x646f72616e646f6dULL ^ k1;
    uint64_t v2 = 0x6c7967656e657261ULL ^ k0;
    uint64_t v3 = 0x7465646279746573ULL ^ k1 ^ d;

    SIPROUND;
    SIPROUND;
    v0 ^= d;
    d = val.GetUint64(1);
    v3 ^= d;
    SIPROUND;
    SIPROUND;
    v0 ^= d;
    d = val.GetUint64(2);
    v3 ^= d;
    SIPROUND;
    SIPROUND;
    v0 ^= d;
    d = val.GetUint64(3);
    v3 ^= d;
    SIPROUND;
    SIPROUND;
    v0 ^= d;
    v3 ^= ((uint64_t)4) << 59;
    SIPROUND;
    SIPROUND;
    v0 ^= ((uint64_t)4) << 59;
    v2 ^= 0xFF;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    return v0 ^ v1 ^ v2 ^ v3;
}

uint64_t SipHashUint256Extra(uint64_t k0, uint64_t k1, const uint256& val, uint32_t extra)
{
    /* Specialized implementation for efficiency */
    uint64_t d = val.GetUint64(0);

    uint64_t v0 = 0x736f6d6570736575ULL ^ k0;
    uint64_t v1 = 0x646f72616e646f6dULL ^ k1;
    uint64_t v2 = 0x6c7967656e657261ULL ^ k0;
    uint64_t v3 = 0x7465646279746573ULL ^ k1 ^ d;

    SIPROUND;
    SIPROUND;
    v0 ^= d;
    d = val.GetUint64(1);
    v3 ^= d;
    SIPROUND;
    SIPROUND;
    v0 ^= d;
    d = val.GetUint64(2);
    v3 ^= d;
    SIPROUND;
    SIPROUND;
    v0 ^= d;
    d = val.GetUint64(3);
    v3 ^= d;
    SIPROUND;
    SIPROUND;
    v0 ^= d;
    d = (((uint64_t)36) << 56) | extra;
    v3 ^= d;
    SIPROUND;
    SIPROUND;
    v0 ^= d;
    v2 ^= 0xFF;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    return v0 ^ v1 ^ v2 ^ v3;
}


std::vector<ContextHash> initContextList() {
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
	//BasicAlgoHash<const void*> algoHashes[1];
	//algoHashes[0] = Blake512Algo();
	std::vector<ContextHash> algoHashes;
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

	algoHashes.push_back(blakeContext);
	algoHashes.push_back(bmwContext);
	algoHashes.push_back(groestlContext);
	algoHashes.push_back(jhContext);
	algoHashes.push_back(keccakConext);
	algoHashes.push_back(skeinContext);
	algoHashes.push_back(luffaContext);
	algoHashes.push_back(cubehashContext);
	algoHashes.push_back(shaviteContext);
	algoHashes.push_back(simdContext);
	algoHashes.push_back(echoContext);
	algoHashes.push_back(hamsiContext);
	algoHashes.push_back(fugueContext);
	algoHashes.push_back(shabalContext);
	algoHashes.push_back(whirlpoolContext);
	algoHashes.push_back(sha512Context);
	return algoHashes;
}

/*
void calculateBlake512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash){
	sph_blake512_init(&conextObj);
	sph_blake512 (&conextObj, toHash, lenToHash);
	sph_blake512_close(&conextObj, static_cast<void*>(hash));
}

void calculateBmw512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_bmw512_init(&conextObj);
	sph_bmw512 (&conextObj, toHash, lenToHash);
	sph_bmw512_close(&conextObj, static_cast<void*>(hash));
}
void calculateJh512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_jh512_init(&conextObj);
	sph_jh512 (&conextObj, toHash, lenToHash);
	sph_jh512_close(&conextObj, static_cast<void*>(hash));
}
void calculateGroest512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_groestl512_init(&conextObj);
	sph_groestl512 (&conextObj, toHash, lenToHash);
	sph_groestl512_close(&conextObj, static_cast<void*>(hash));
}
void calculateKeccak512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_keccak512_init(&conextObj);
	sph_keccak512 (&conextObj, toHash, lenToHash);
	sph_keccak512_close(&conextObj, static_cast<void*>(hash));
}
void calculateSkein512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_skein512_init(&conextObj);
	sph_skein512 (&conextObj, toHash, lenToHash);
	sph_skein512_close(&conextObj, static_cast<void*>(hash));
}
void calculateLuffa512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_luffa512_init(&conextObj);
	sph_luffa512 (&conextObj, toHash, lenToHash);
	sph_luffa512_close(&conextObj, static_cast<void*>(hash));
}
void calculateCubehash512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_cubehash512_init(&conextObj);
	sph_cubehash512 (&conextObj, toHash, lenToHash);
	sph_cubehash512_close(&conextObj, static_cast<void*>(hash));
}
void calculateShavite512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_shavite512_init(&conextObj);
	sph_shavite512(&conextObj, toHash, lenToHash);
	sph_shavite512_close(&conextObj, static_cast<void*>(hash));
}
void calculateSimd512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_simd512_init(&conextObj);
	sph_simd512 (&conextObj, toHash, lenToHash);
	sph_simd512_close(&conextObj, static_cast<void*>(hash));
}
void calculateEcho512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_echo512_init(&conextObj);
	sph_echo512 (&conextObj, toHash, lenToHash);
	sph_echo512_close(&conextObj, static_cast<void*>(hash));
}
void calculateHamsi512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_hamsi512_init(&conextObj);
	sph_hamsi512 (&conextObj, toHash, lenToHash);
	sph_hamsi512_close(&conextObj, static_cast<void*>(hash));
}
void calculateFugue512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash){
	sph_fugue512_init(&conextObj);
	sph_fugue512 (&conextObj, toHash, lenToHash);
	sph_fugue512_close(&conextObj, static_cast<void*>(hash));
}
void calculateShabal512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_shabal512_init(&conextObj);
	sph_shabal512 (&conextObj, toHash, lenToHash);
	sph_shabal512_close(&conextObj, static_cast<void*>(hash));
}
void calculateWhirlpool512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_whirlpool_init(&conextObj);
	sph_whirlpool(&conextObj, toHash, lenToHash);
	sph_whirlpool_close(&conextObj, static_cast<void*>(hash));
}
void calculateSha512Hash(void* conextObj, const void* toHash, int lenToHash, uint512* hash) {
	sph_sha512_init(&conextObj);
	sph_sha512 (&conextObj, toHash, lenToHash);
	sph_sha512_close(&conextObj, static_cast<void*>(hash));
}*/
/*
void Blake512Algo::calculateHash(const void* toHash, int lenToHash, uint512* hash) {
	sph_blake512_init(&context);
	sph_blake512 (&context, toHash, lenToHash);
	sph_blake512_close(&context, static_cast<void*>(hash));
}*/
/*
class Blake512Algo : public BasicAlgoHash<sph_blake512_context> {
public:
	Blake512Algo() {

	}
	void calculateHash(const void* toHash, int lenToHash, uint512* hash) {
		sph_blake512_init(&context);
		sph_blake512 (&context, toHash, lenToHash);
		sph_blake512_close(&context, static_cast<void*>(hash));
	}
};*/
/*
template<typename T1>
inline uint256 SolisHash::HashSolis(const T1 pbegin, const T1 pend, const uint256 PrevBlockHash)
{
//	static std::chrono::duration<double>[16];
    int hashSelection;

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
    BasicAlgoHash<const void*> algoHashes[1];
    algoHashes[0] = Blake512Algo();
    static unsigned char pblank[1];

    uint512 hash[16];

    for (int i=0;i<16;i++)
    {
        const void *toHash;
        int lenToHash;
        if (i == 0) {
            toHash = (pbegin == pend ? pblank : static_cast<const void*>(&pbegin[0]));
            lenToHash = (pend - pbegin) * sizeof(pbegin[0]);
        } else {
            toHash = static_cast<const void*>(&hash[i-1]);
            lenToHash = 64;
        }

        hashSelection = GetHashSelection(PrevBlockHash, i);

        switch(hashSelection) {
            case 0:
            	algoHashes[0].calculateHash(&toHash,lenToHash, hash);
                break;
            case 1:
                sph_bmw512_init(&ctx_bmw);
                sph_bmw512 (&ctx_bmw, toHash, lenToHash);
                sph_bmw512_close(&ctx_bmw, static_cast<void*>(hash));
                break;
            case 2:
                sph_groestl512_init(&ctx_groestl);
                sph_groestl512 (&ctx_groestl, toHash, lenToHash);
                sph_groestl512_close(&ctx_groestl, static_cast<void*>(hash));
                break;
            case 3:
                sph_jh512_init(&ctx_jh);
                sph_jh512 (&ctx_jh, toHash, lenToHash);
                sph_jh512_close(&ctx_jh, static_cast<void*>(hash));
                break;
            case 4:
                sph_keccak512_init(&ctx_keccak);
                sph_keccak512 (&ctx_keccak, toHash, lenToHash);
                sph_keccak512_close(&ctx_keccak, static_cast<void*>(hash));
                break;
            case 5:
                sph_skein512_init(&ctx_skein);
                sph_skein512 (&ctx_skein, toHash, lenToHash);
                sph_skein512_close(&ctx_skein, static_cast<void*>(hash));
                break;
            case 6:
                sph_luffa512_init(&ctx_luffa);
                sph_luffa512 (&ctx_luffa, toHash, lenToHash);
                sph_luffa512_close(&ctx_luffa, static_cast<void*>(hash));
                break;
            case 7:
                sph_cubehash512_init(&ctx_cubehash);
                sph_cubehash512 (&ctx_cubehash, toHash, lenToHash);
                sph_cubehash512_close(&ctx_cubehash, static_cast<void*>(hash));
                break;
            case 8:
                sph_shavite512_init(&ctx_shavite);
                sph_shavite512(&ctx_shavite, toHash, lenToHash);
                sph_shavite512_close(&ctx_shavite, static_cast<void*>(hash));
                break;
            case 9:
                sph_simd512_init(&ctx_simd);
                sph_simd512 (&ctx_simd, toHash, lenToHash);
                sph_simd512_close(&ctx_simd, static_cast<void*>(hash));
                break;
            case 10:
                sph_echo512_init(&ctx_echo);
                sph_echo512 (&ctx_echo, toHash, lenToHash);
                sph_echo512_close(&ctx_echo, static_cast<void*>(hash));
                break;
           case 11:
                sph_hamsi512_init(&ctx_hamsi);
                sph_hamsi512 (&ctx_hamsi, toHash, lenToHash);
                sph_hamsi512_close(&ctx_hamsi, static_cast<void*>(hash));
                break;
           case 12:
                sph_fugue512_init(&ctx_fugue);
                sph_fugue512 (&ctx_fugue, toHash, lenToHash);
                sph_fugue512_close(&ctx_fugue, static_cast<void*>(hash));
                break;
           case 13:
                sph_shabal512_init(&ctx_shabal);
                sph_shabal512 (&ctx_shabal, toHash, lenToHash);
                sph_shabal512_close(&ctx_shabal, static_cast<void*>(hash));
                break;
           case 14:
                sph_whirlpool_init(&ctx_whirlpool);
                sph_whirlpool(&ctx_whirlpool, toHash, lenToHash);
                sph_whirlpool_close(&ctx_whirlpool, static_cast<void*>(hash));
                break;
           case 15:
                sph_sha512_init(&ctx_sha512);
                sph_sha512 (&ctx_sha512, toHash, lenToHash);
                sph_sha512_close(&ctx_sha512, static_cast<void*>(hash));
                break;
        }
    }

    return hash[15].trim256();
}*/
