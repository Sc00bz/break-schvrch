// Insert License here

#pragma once
#include <stdint.h>

void stir(uint64_t *state, uint64_t statelen, int rounds);

void revolve(uint64_t *state, int statelen, uint64_t rounds);

void evolve(uint64_t * state, int statelen);

int PHS(
	      void   *out,    size_t       outlen,
	const void   *in,     size_t       inlen,
	const void   *salt,   size_t       saltlen,
	unsigned int  t_cost, unsigned int m_cost);

int PHS_fixed(
	      void   *out,    size_t       outlen,
	const void   *in,     size_t       inlen,
	const void   *salt,   size_t       saltlen,
	unsigned int  t_cost, unsigned int m_cost);
