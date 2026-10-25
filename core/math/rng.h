/**
 * @file rng.h
 * @brief A small deterministic random number generator (xorshift32), so a
 * seed always gives the same level, the same fight and the same test run.
 * Games own their generators; nothing here is global.
 */

#ifndef CORE_MATH_RNG_H_
#define CORE_MATH_RNG_H_

#include <stdint.h>

typedef struct {
  uint32_t state;
} rng_t;

void rng_seed(rng_t* rng, uint32_t seed);
uint32_t rng_next(rng_t* rng);

/** In [0, 1). */
float rng_float(rng_t* rng);

/** In [low, high], both included. */
int rng_range(rng_t* rng, int low, int high);

/** Non-zero with the given probability. */
int rng_chance(rng_t* rng, float probability);

#endif  // CORE_MATH_RNG_H_
