// Insert License here
// Written by Rade Vuckovac
// Small modifications by Steve Thomas

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "schvrch.h"

void stir(uint64_t *state, uint64_t statelen, int rounds)
{
	const uint64_t mixer = UINT64_C(0x5555555555555555);
	uint64_t carry = UINT64_C(1234567890123456789);

	for (int i = 0; i < rounds; i++)
	{
		for (uint64_t j = 0; j < statelen; j++)
		{
			uint64_t s1, s2, s3;

			s1 = state[(j + 1) % statelen];
			s2 = state[(j + 2) % statelen];
			s3 = state[(j + 3) % statelen];
			carry ^= s2 > s3 ? s1 : ~s1;
			state[j] ^= carry;
			carry += mixer;
		}
	}
}

void revolve(uint64_t *state, int statelen, uint64_t rounds)
{
	uint64_t carry = 0;
		
	for (uint64_t i = 0; i < rounds; i++)
	{
		for (int j = 0; j < statelen; j++)
		{
			uint64_t s1, s2, s3;

			s1 = state[(j + 1) % statelen];
			s2 = state[(j + 2) % statelen];
			s3 = state[(j + 3) % statelen];
			carry ^= s2 > s3 ? s1 : ~s1;
			state[j] ^= carry;
		}
	}
}

void evolve(uint64_t * state, int statelen)
{
	for (int i = 0; i < 2 * statelen; i++)
	{	
		for (int j = 0; j < statelen; j++)
		{
			uint64_t s1, s2, s3;

			s1 = state[(j + 1) % statelen];
			s2 = state[(j + 2) % statelen];
			s3 = state[(j + 3) % statelen];
			state[j] ^= (s1 > s3     ? s1 : ~s1) ^
			            (s2 > s3     ? s2 : ~s2) ^
			            (s3 % 2 == 1 ? s3 : ~s3);
		}
	}
}

int PHS(
	      void   *out,    size_t       outlen,
	const void   *in,     size_t       inlen,
	const void   *salt,   size_t       saltlen,
	unsigned int  t_cost, unsigned int m_cost)
{
	const uint64_t statelen = 256;
	const int      rounds   = 4;

	uint64_t  state[statelen] = {0};
	uint64_t  memcost    = (m_cost + UINT64_C(1)) * statelen;
	uint64_t *memstate;

	memmove(&state[0], in, inlen);
	memmove(&state[(inlen / 8) + 1], salt, saltlen);
	state[statelen - 3] = outlen;
	state[statelen - 2] = inlen;
	state[statelen - 1] = saltlen;
	stir(state, statelen, rounds * 2);

	if (t_cost > 0)
	{
		revolve(state, statelen, t_cost);
	}
	if (m_cost > 0)
	{
		memstate = (uint64_t *) calloc(memcost, sizeof(uint64_t));

		memmove(memstate, state, statelen * sizeof(uint64_t));
		stir(memstate, memcost, rounds);
/*
		// this is for t_cost=100000, m_cost=5000
		uint64_t *m = memstate;
		uint64_t x = m[5001]^m[10002]^m[15003]^m[20004]^m[25005]^m[70014]^m[75015]^m[80016]^m[85017]^m[90018]^m[95019]^m[100020]^m[105021];
		printf("hash[0] ^ hash[1] = %016" PRIx64 "\n",          x);
		printf("hash[0] ^ hash[1] = %016" PRIx64 " (not)\n",   ~x);
		x = m[30006]^m[70014]^m[110022];
		printf("hash[0] ^ hash[2] = %016" PRIx64 "\n",          x);
		printf("hash[0] ^ hash[2] = %016" PRIx64 " (not)\n",   ~x);
		x = m[5001]^m[10002]^m[15003]^m[20004]^m[25005]^m[35007]^m[70014]^m[80016]^m[85017]^m[90018]^m[95019]^m[100020]^m[105021]^m[115023];
		printf("hash[0] ^ hash[3] = %016" PRIx64 "\n",          x);
		printf("hash[0] ^ hash[3] = %016" PRIx64 " (not)\n\n", ~x);
*/
		for (uint64_t i = 0; i < memcost / statelen; i++)
		{
			for (int j = 0; j < statelen; j++)
			{
				state[j] = memstate[j * (i + 1)];
			}
			revolve(state, statelen, 2);
		}
		free(memstate);
	}
	evolve(state, statelen);
/*
	// this is for comparing the short cut with t_cost=100000, m_cost=5000
	printf("hash[0] ^ hash[1] = %016" PRIx64 "\n", state[0] ^ state[1]);
	printf("hash[0] ^ hash[2] = %016" PRIx64 "\n", state[0] ^ state[2]);
	printf("hash[0] ^ hash[3] = %016" PRIx64 "\n", state[0] ^ state[3]);
*/
	memmove(out, state, outlen);
	return 0;
}

int PHS_fixed(
	      void   *out,    size_t       outlen,
	const void   *in,     size_t       inlen,
	const void   *salt,   size_t       saltlen,
	unsigned int  t_cost, unsigned int m_cost)
{
	const uint64_t statelen = 256;
	const int      rounds   = 4;

	uint64_t  state[statelen] = {0};
	uint64_t  memcost    = (m_cost + UINT64_C(1)) * statelen;
	uint64_t *memstate;

	memmove(&state[0], in, inlen);
	memmove(&state[(inlen / 8) + 1], salt, saltlen);
	state[statelen - 3] = outlen;
	state[statelen - 2] = inlen;
	state[statelen - 1] = saltlen;
	stir(state, statelen, rounds * 2);

	if (t_cost > 0)
	{
		revolve(state, statelen, t_cost);
	}
	if (m_cost > 0)
	{
		memstate = (uint64_t*) calloc(memcost, sizeof(uint64_t));

		memmove(memstate, state, statelen * sizeof(uint64_t));
		stir(memstate, memcost, rounds);
		for (uint64_t i = 0; i < memcost / statelen; i++)
		{
			for (int j = 0; j < statelen; j++)
			{
				state[j] ^= memstate[j * (i + 1)];
			}
			revolve(state, statelen, 2);
		}
		free(memstate);
	}
	evolve(state, statelen);
	memmove(out, state, outlen);
	return 0;
}
