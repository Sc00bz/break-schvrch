/*
Copyright (c) 2014, Steve "Sc00bz" Thomas
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS
#define __STDC_LIMIT_MACROS
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include "schvrch.h"
#include "breakschvrch.h"

#define SWAP_ENDIAN_32_(x) \
	( \
		 ((x) << 24) | \
		(((x) <<  8) & 0x00ff0000) | \
		(((x) >>  8) & 0x0000ff00) | \
		 ((x) >> 24) \
	)
#define SWAP_ENDIAN_32(x)           SWAP_ENDIAN_32_((uint32_t) (x))
#define SWAP_ENDIAN_64_(x) \
	( \
		 ((x) << 56) | \
		(((x) << 40) & UINT64_C(0x00ff000000000000)) | \
		(((x) << 24) & UINT64_C(0x0000ff0000000000)) | \
		(((x) <<  8) & UINT64_C(0x000000ff00000000)) | \
		(((x) >>  8) & UINT64_C(0x00000000ff000000)) | \
		(((x) >> 24) & UINT64_C(0x0000000000ff0000)) | \
		(((x) >> 40) & UINT64_C(0x000000000000ff00)) | \
		 ((x) >> 56) \
	)
#define SWAP_ENDIAN_64(x)           SWAP_ENDIAN_64_((uint64_t) (x))

#ifdef _WIN32
	#include <inttypes.h>
	#include <time.h>
	#include <windows.h>
	typedef LARGE_INTEGER TIMER_TYPE;
	#define TIMER_FUNC(t)             QueryPerformanceCounter(&t)
	double TIMER_DIFF(LARGE_INTEGER s, LARGE_INTEGER e)
	{
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		return ((double) (e.QuadPart - s.QuadPart)) / f.QuadPart;
	}
#else
	#include <inttypes.h>
	#include <sys/time.h>
	#include <unistd.h>
	#define TIMER_TYPE                timeval
	#define TIMER_FUNC(t)             gettimeofday(&t, NULL)
	#define TIMER_DIFF(s,e)           ((e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec) / (double)1000000.0)
#endif

int PHS_Fast(
	      void   *out,    size_t       outlen,
	const void   *in,     size_t       inlen,
	const void   *salt,   size_t       saltlen,
	unsigned int  t_cost, unsigned int m_cost)
{
	const uint64_t statelen = 256;
	const int      rounds   = 4;

	uint64_t s[statelen] = {0};

	if (outlen > 32 || t_cost != 100000 || m_cost != 0)
	{
		return 1;
	}

	memmove(s, in, inlen);
	memmove(s + (inlen / 8) + 1, salt, saltlen);
	s[statelen - 3] = outlen;
	s[statelen - 2] = inlen;
	s[statelen - 1] = saltlen;
	stir(s, statelen, rounds * 2);

	uint64_t hash[4];
	// generated from calling "PHS_break(t_cost=100000, m_cost=0, outlen=32, printXors=true)"
	hash[0] = s[0]^s[2]^s[3]^s[7]^s[8]^s[9]^s[12]^s[13]^s[16]^s[20]^s[22]^s[23]^s[24]^s[25]^s[26]^s[27]^s[33]^s[34]^s[35]^s[36]^s[37]^s[38]^s[39]^s[40]^s[42]^s[48]^s[49]^s[54]^s[56]^s[57]^s[59]^s[62]^s[63]^s[64]^s[65]^s[66]^s[67]^s[68]^s[69]^s[79]^s[81]^s[85]^s[88]^s[90]^s[91]^s[92]^s[97]^s[98]^s[99]^s[101]^s[102]^s[103]^s[104]^s[108]^s[110]^s[112]^s[113]^s[116]^s[117]^s[118]^s[119]^s[120]^s[121]^s[122]^s[123]^s[124]^s[125]^s[126]^s[127]^s[128]^s[129]^s[131]^s[132]^s[133]^s[134]^s[135]^s[136]^s[137]^s[138]^s[139]^s[142]^s[144]^s[145]^s[146]^s[148]^s[151]^s[152]^s[153]^s[154]^s[156]^s[157]^s[158]^s[159]^s[160]^s[162]^s[169]^s[171]^s[172]^s[173]^s[175]^s[179]^s[180]^s[186]^s[188]^s[196]^s[202]^s[205]^s[207]^s[210]^s[211]^s[212]^s[213]^s[214]^s[216]^s[217]^s[218]^s[219]^s[221]^s[225]^s[227]^s[229]^s[233]^s[234]^s[235]^s[236]^s[239]^s[241]^s[242]^s[248]^s[249]^s[255];
	hash[1] = s[0]^s[3]^s[4]^s[8]^s[9]^s[10]^s[13]^s[14]^s[17]^s[21]^s[23]^s[24]^s[25]^s[26]^s[27]^s[28]^s[34]^s[35]^s[36]^s[37]^s[38]^s[39]^s[40]^s[41]^s[43]^s[49]^s[50]^s[55]^s[57]^s[58]^s[60]^s[63]^s[64]^s[65]^s[66]^s[67]^s[68]^s[69]^s[70]^s[80]^s[82]^s[86]^s[89]^s[91]^s[92]^s[93]^s[98]^s[99]^s[100]^s[102]^s[103]^s[104]^s[105]^s[109]^s[111]^s[113]^s[114]^s[117]^s[118]^s[119]^s[120]^s[121]^s[122]^s[123]^s[124]^s[125]^s[126]^s[127]^s[128]^s[129]^s[130]^s[132]^s[133]^s[134]^s[135]^s[136]^s[137]^s[138]^s[139]^s[140]^s[143]^s[145]^s[146]^s[147]^s[149]^s[152]^s[153]^s[154]^s[155]^s[157]^s[158]^s[159]^s[160]^s[161]^s[163]^s[170]^s[172]^s[173]^s[174]^s[176]^s[180]^s[181]^s[187]^s[189]^s[197]^s[203]^s[206]^s[208]^s[211]^s[212]^s[213]^s[214]^s[215]^s[217]^s[218]^s[219]^s[220]^s[222]^s[226]^s[228]^s[230]^s[234]^s[235]^s[236]^s[237]^s[240]^s[242]^s[243]^s[249]^s[250];
	hash[2] = s[4]^s[5]^s[9]^s[10]^s[11]^s[14]^s[15]^s[18]^s[22]^s[24]^s[25]^s[26]^s[27]^s[28]^s[29]^s[35]^s[36]^s[37]^s[38]^s[39]^s[40]^s[41]^s[42]^s[44]^s[50]^s[51]^s[56]^s[58]^s[59]^s[61]^s[64]^s[65]^s[66]^s[67]^s[68]^s[69]^s[70]^s[71]^s[81]^s[83]^s[87]^s[90]^s[92]^s[93]^s[94]^s[99]^s[100]^s[101]^s[103]^s[104]^s[105]^s[106]^s[110]^s[112]^s[114]^s[115]^s[118]^s[119]^s[120]^s[121]^s[122]^s[123]^s[124]^s[125]^s[126]^s[127]^s[128]^s[129]^s[130]^s[131]^s[133]^s[134]^s[135]^s[136]^s[137]^s[138]^s[139]^s[140]^s[141]^s[144]^s[146]^s[147]^s[148]^s[150]^s[153]^s[154]^s[155]^s[156]^s[158]^s[159]^s[160]^s[161]^s[162]^s[164]^s[171]^s[173]^s[174]^s[175]^s[177]^s[181]^s[182]^s[188]^s[190]^s[198]^s[204]^s[207]^s[209]^s[212]^s[213]^s[214]^s[215]^s[216]^s[218]^s[219]^s[220]^s[221]^s[223]^s[227]^s[229]^s[231]^s[235]^s[236]^s[237]^s[238]^s[241]^s[243]^s[244]^s[250]^s[251];
	hash[3] = s[1]^s[5]^s[6]^s[10]^s[11]^s[12]^s[15]^s[16]^s[19]^s[23]^s[25]^s[26]^s[27]^s[28]^s[29]^s[30]^s[36]^s[37]^s[38]^s[39]^s[40]^s[41]^s[42]^s[43]^s[45]^s[51]^s[52]^s[57]^s[59]^s[60]^s[62]^s[65]^s[66]^s[67]^s[68]^s[69]^s[70]^s[71]^s[72]^s[82]^s[84]^s[88]^s[91]^s[93]^s[94]^s[95]^s[100]^s[101]^s[102]^s[104]^s[105]^s[106]^s[107]^s[111]^s[113]^s[115]^s[116]^s[119]^s[120]^s[121]^s[122]^s[123]^s[124]^s[125]^s[126]^s[127]^s[128]^s[129]^s[130]^s[131]^s[132]^s[134]^s[135]^s[136]^s[137]^s[138]^s[139]^s[140]^s[141]^s[142]^s[145]^s[147]^s[148]^s[149]^s[151]^s[154]^s[155]^s[156]^s[157]^s[159]^s[160]^s[161]^s[162]^s[163]^s[165]^s[172]^s[174]^s[175]^s[176]^s[178]^s[182]^s[183]^s[189]^s[191]^s[199]^s[205]^s[208]^s[210]^s[213]^s[214]^s[215]^s[216]^s[217]^s[219]^s[220]^s[221]^s[222]^s[224]^s[228]^s[230]^s[232]^s[236]^s[237]^s[238]^s[239]^s[242]^s[244]^s[245]^s[251]^s[252];
	memmove(out, hash, outlen);
	return 0;
}

void benchmarkNoMem(uint32_t times)
{
	uint64_t out[4];
	TIMER_TYPE s, e;

	TIMER_FUNC(s);
	for (uint32_t i = 0; i < times; i++)
	{
		PHS(out, sizeof(out), "password", 8, "salt", 4, 100000, 0);
	}
	TIMER_FUNC(e);
	printf("Normal method took average time of %0.9f seconds:\n", TIMER_DIFF(s, e) / times);
	printf("%016llx %016llx %016llx %016llx\n\n", SWAP_ENDIAN_64(out[0]), SWAP_ENDIAN_64(out[1]), SWAP_ENDIAN_64(out[2]), SWAP_ENDIAN_64(out[3]));

	TIMER_FUNC(s);
	for (uint32_t i = 0; i < times; i++)
	{
		PHS_Fast(out, sizeof(out), "password", 8, "salt", 4, 100000, 0);
	}
	TIMER_FUNC(e);
	printf("Fast method took average time of %0.9f seconds:\n", TIMER_DIFF(s, e) / times);
	printf("%016llx %016llx %016llx %016llx\n",  SWAP_ENDIAN_64(out[0]),  SWAP_ENDIAN_64(out[1]),  SWAP_ENDIAN_64(out[2]),  SWAP_ENDIAN_64(out[3]));
	printf("%016llx %016llx %016llx %016llx\n",     ~SWAP_ENDIAN_64(out[0]), ~SWAP_ENDIAN_64(out[1]), ~SWAP_ENDIAN_64(out[2]), ~SWAP_ENDIAN_64(out[3]));
}

void accessPattern(uint32_t m_cost)
{
	uint64_t  memcost = (m_cost + UINT64_C(1)) * UINT64_C(256), count = 0;
	std::vector<bool> memAccess(memcost);

	for (uint64_t i = 0; i < memcost / 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			memAccess[j * (i + 1)] = true;
		}
	}
	for (uint64_t i = 0; i < memcost; i++)
	{
		if (memAccess[i])
		{
			count++;
		}
	}
	printf("Accessed %" PRIu64 " / %" PRIu64 " (%0.3f%%) of memstate elements\n", count, memcost, 100.0 * count / memcost);
}

int main()
{
	TIMER_TYPE s, e;

	// Generate shortcut for m_cost=0
	printf("**** Generating shortcut for t_cost=100000 and m_cost=0 ****\n");
	TIMER_FUNC(s);
	PHS_break(100000, 0, 32, false);
	TIMER_FUNC(e);
	printf("Took %0.2f seconds\n", TIMER_DIFF(s, e));

	// Benchmark m_cost=0
	printf("\n\n**** Benchmarking constant time t_cost=100000 and m_cost=0 ****\n");
	benchmarkNoMem(4);

	// Access pattern
	printf("\n\n**** Access pattern m_cost=50000 ****\n");
	accessPattern(50000);

	// Generate shortcut for m_cost=5000
	printf("\n\n**** Generating shortcut for m_cost=5000 ****\n");
	printf("Current version...\n");
	TIMER_FUNC(s);
	PHS_break(0, 5000, 32, false);
	TIMER_FUNC(e);
	printf("Took %0.2f seconds\n\n", TIMER_DIFF(s, e));
	printf("Fixed version...\n");
	TIMER_FUNC(s);
	PHS_fixed_break(0, 5000, 32, false);
	TIMER_FUNC(e);
	printf("Took %0.2f seconds\n", TIMER_DIFF(s, e));

	return 0;
}
