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
#include <string.h>
#include <stdlib.h>
#include "breakschvrch.h"

void printState(uint64_t **state, int statelen, uint64_t stateCount, int outlen, bool printXors);
void revolve_break(uint64_t **state, int statelen, uint64_t stateCount, uint64_t rounds);
void evolve_break(uint64_t **state, int statelen, uint64_t stateCount, bool outputStatus);

void printState(uint64_t **state, int statelen, uint64_t stateCount, int outlen, bool printXors)
{
	const char *xorChar;
	int         outCount = (outlen + 7) / 8;

	for (int i = 0; i < outCount; i++)
	{
		if (printXors)
		{
			printf("hash[%d] = ", i);
			xorChar = "";
		}
		uint64_t count = 0;
		uint64_t highest = 0;
		for (uint64_t j = 0; j < stateCount; j++)
		{
			uint64_t s = state[i][j];

			if (s != 0)
			{
				for (uint64_t bit = 1, bitPos = 64 * j; s != 0; bit <<= 1, bitPos++)
				{
					if ((s & bit) != 0)
					{
						highest = bitPos;
						count++;
						s ^= bit;
						if (printXors)
						{
							if (bitPos < statelen)
							{
								printf("%ss[%" PRIu64 "]", xorChar, bitPos); // state
							}
							else
							{
								printf("%sm[%" PRIu64 "]", xorChar, bitPos - statelen); // mem
							}
							xorChar = "^";
						}
					}
				}
			}
		}
		if (printXors)
		{
			if (xorChar[0] == 0)
			{
				printf("0");
			}
			printf("\n");
		}
		printf("hash[%d]: %" PRIu64 " elements, highest is %" PRIu64 "\n", i, count, highest);
		if (printXors)
		{
			printf("\n");
		}
	}
	for (int i = 0; i < outCount; i++)
	{
		for (int k = i + 1; k < outCount; k++)
		{
			if (printXors)
			{
				printf("hash[%d] ^ hash[%d] = ", i, k);
				xorChar = "";
			}
			uint64_t count = 0;
			uint64_t highest = 0;
			for (uint64_t j = 0; j < stateCount; j++)
			{
				uint64_t s = state[i][j] ^ state[k][j];

				if (s != 0)
				{
					for (uint64_t bit = 1, bitPos = 64 * j; s != 0; bit <<= 1, bitPos++)
					{
						if ((s & bit) != 0)
						{
							highest = bitPos;
							count++;
							s ^= bit;
							if (printXors)
							{
								if (bitPos < statelen)
								{
									printf("%ss[%" PRIu64 "]", xorChar, bitPos); // state
								}
								else
								{
									printf("%sm[%" PRIu64 "]", xorChar, bitPos - statelen); // mem
								}
								xorChar = "^";
							}
						}
					}
				}
			}
			if (printXors)
			{
				if (xorChar[0] == 0)
				{
					printf("0");
				}
				printf("\n");
			}
			printf("hash[%d] ^ hash[%d]: %" PRIu64 " elements, highest is %" PRIu64 "\n", i, k, count, highest);
			if (printXors)
			{
				printf("\n");
			}
		}
	}
}

void revolve_break(uint64_t **state, int statelen, uint64_t stateCount, uint64_t rounds)
{
	uint64_t *carry = new uint64_t[stateCount];

	for (int i = 0; i < stateCount; i++)
	{
		carry[i] = 0;
	}
	for (uint64_t i = 0; i < rounds; i++)
	{
		for (int j = 0; j < statelen; j++)
		{
			uint64_t *state0, *state1;

			state0 = state[j];
			state1 = state[(j + 1) % statelen];
			for (uint64_t k = 0; k < stateCount; k++)
			{
				carry[k]  ^= state1[k];
				state0[k] ^= carry[k];
			}
		}
	}
	delete [] carry;
}

void evolve_break(uint64_t **state, int statelen, uint64_t stateCount, bool outputStatus)
{
	if (outputStatus)
	{
		printf("evolve_break\n");
	}
	for (int i = 0; i < 2 * statelen; i++)
	{
		if (i % 64 == 0)
		{
			if (outputStatus)
			{
				printf("round %d/%d\n", i, 2 * statelen);
			}
		}
		for (int j = 0; j < statelen; j++)
		{
			uint64_t *state0, *state1, *state2, *state3;

			state0 = state[j];
			state1 = state[(j + 1) % statelen];
			state2 = state[(j + 2) % statelen];
			state3 = state[(j + 3) % statelen];
			for (uint64_t k = 0; k < stateCount; k++)
			{
				state0[k] ^= state1[k] ^ state2[k] ^ state3[k];
			}
		}
	}
	if (outputStatus)
	{
		printf("evolve_break done\n");
	}
}

void PHS_break(unsigned int t_cost, unsigned int m_cost, int outlen, bool printXors)
{
	int       statelen   = 256;
	uint64_t *state[256];
	uint64_t  memcost    = (m_cost + 1) * statelen;
	uint64_t  rounds     = 4;

	if (m_cost > 0)
	{
		// m_cost != 0
		for (int i = 0; i < statelen; i++)
		{
			state[i] = new uint64_t[(statelen + memcost) / 64];
			for (uint64_t j = 0; j < (statelen + memcost) / 64; j++)
			{
				state[i][j] = 0;
			}
		}
		uint64_t i = memcost / statelen - 1;
		for (int j = 0; j < statelen; j++)
		{
			uint64_t bit = j * (i + 1) + statelen;
			state[j][bit / 64] |= UINT64_C(1) << (bit % 64);
		}
		revolve_break(state, statelen, (statelen + memcost) / 64, 2);
		evolve_break(state, statelen, (statelen + memcost) / 64, true);
		printState(state, statelen, (statelen + memcost) / 64, outlen, printXors);
	}
	else if (t_cost > 0)
	{
		// t_cost != 0, m_cost == 0
		for (int i = 0; i < statelen; i++)
		{
			state[i] = new uint64_t[statelen / 64];
			for (int j = 0; j < statelen / 64; j++)
			{
				state[i][j] = 0;
			}
			state[i][i / 64] |= UINT64_C(1) << (i % 64);
		}
		revolve_break(state, statelen, statelen / 64, t_cost);
		evolve_break(state, statelen, statelen / 64, false);
		printState(state, statelen, statelen / 64, outlen, printXors);
	}
	else
	{
		// t_cost = 0, m_cost = 0
		for (int i = 0; i < statelen; i++)
		{
			state[i] = new uint64_t[statelen / 64];
			for (int j = 0; j < statelen / 64; j++)
			{
				state[i][j] = 0;
			}
			state[i][i / 64] |= UINT64_C(1) << (i % 64);
		}
		evolve_break(state, statelen, statelen / 64, false);
		printState(state, statelen, statelen / 64, outlen, printXors);
	}
	for (int i = 0; i < statelen; i++)
	{
		delete [] state[i];
	}
}

void PHS_fixed_break(unsigned int t_cost, unsigned int m_cost, int outlen, bool printXors)
{
	const uint64_t statelen = 256;
	const int      rounds   = 4;

	uint64_t *state[256];
	uint64_t  memcost = (m_cost + UINT64_C(1)) * statelen;

	if (m_cost > 0)
	{
		// m_cost != 0
		for (int i = 0; i < statelen; i++)
		{
			state[i] = new uint64_t[(statelen + memcost) / 64];
			for (int j = 0; j < (statelen + memcost) / 64; j++)
			{
				state[i][j] = 0;
			}
			state[i][i / 64] |= UINT64_C(1) << (i % 64);
		}
		for (uint64_t i = 0; i < memcost / statelen; i++)
		{
			if (i % 100 == 0)
			{
				printf("revolve_break %" PRIu64 "/%" PRIu64 "\n", i, memcost / statelen);
			}
			for (int j = 0; j < statelen; j++)
			{
				uint64_t bit = j * (i + 1) + statelen;
				state[j][bit / 64] |= UINT64_C(1) << (bit % 64);
			}
			revolve_break(state, statelen, (statelen + memcost) / 64, 2);
		}
		evolve_break(state, statelen, (statelen + memcost) / 64, true);
		printState(state, statelen, (statelen + memcost) / 64, outlen, printXors);
	}
	else if (t_cost > 0)
	{
		// t_cost != 0, m_cost = 0
		for (int i = 0; i < statelen; i++)
		{
			state[i] = new uint64_t[statelen / 64];
			for (int j = 0; j < statelen / 64; j++)
			{
				state[i][j] = 0;
			}
			state[i][i / 64] |= UINT64_C(1) << (i % 64);
		}
		revolve_break(state, statelen, statelen / 64, t_cost);
		evolve_break(state, statelen, statelen / 64, false);
		printState(state, statelen, statelen / 64, outlen, printXors);
	}
	else
	{
		// t_cost = 0, m_cost = 0
		for (int i = 0; i < statelen; i++)
		{
			state[i] = new uint64_t[statelen / 64];
			for (int j = 0; j < statelen / 64; j++)
			{
				state[i][j] = 0;
			}
			state[i][i / 64] |= UINT64_C(1) << (i % 64);
		}
		evolve_break(state, statelen, statelen / 64, false);
		printState(state, statelen, statelen / 64, outlen, printXors);
	}
	for (int i = 0; i < statelen; i++)
	{
		delete [] state[i];
	}
}
