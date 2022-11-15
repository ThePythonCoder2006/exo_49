#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

typedef struct
{
  uint64_t state;
} prng_u64_state;

static inline uint64_t prng_u64(prng_u64_state *const p)
{
  uint64_t state = p->state;
  state ^= state >> 12;
  state ^= state << 25;
  state ^= state >> 27;
  p->state = state;
  return state * UINT64_C(2685821657736338717);
}

int main(int argc, char **argv)
{
  srand(time(NULL));

  prng_u64_state *const state = malloc(sizeof(prng_u64_state));
  state->state = rand();

  // __m256i a = _mm256_set_epi8(prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state));

  // __m256i result;

  uint8_t *res = (uint8_t *)&a;

  for (int i = 0; i < 32; ++i)
  {
    // printf("ok %i\n", i);
    printf("%" PRIu8 "\n", res[i]);
  }

  return EXIT_SUCCESS;
}