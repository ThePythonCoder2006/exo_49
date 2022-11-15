#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

typedef struct
{
  __m256i x;
  __m256i y;
  __m256i z;
  __m256i a;
} xorshift_state;

__m256i _mm256_srli_epi8(__m256i a, uint8_t count);
__m256i _mm256_slli_epi8(__m256i a, uint8_t count);

__m256i xorshift(xorshift_state *const state)
{
  // uint8_t t = state->x ^ (state->x >> 1);
  // state->x = state->y;
  // state->y = state->z;
  // state->z = state->a;
  // state->a = state->z ^ t ^ (state->z >> 3) ^ (t << 1);
  return state->a;
}

int main(int argc, char *argv)
{
  // srand(time(NULL));

  // xorshift_state *state = malloc(sizeof(xorshift_state));

  // __m256i a = _mm256_set_epi8(prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state),
  //                             prng_u64(state), prng_u64(state), prng_u64(state), prng_u64(state));

  __m256i result = _mm256_set1_epi8(UINT8_MAX);

  result = _mm256_srli_epi8(result, 1);

  uint8_t *res = (uint8_t *)&result;

  for (int i = 0; i < 32; ++i)
  {
    // printf("ok %i\n", i);
    printf("%" PRIu8 "\n", res[i]);
  }

  return EXIT_SUCCESS;
}

__m256i _mm256_srli_epi8(__m256i a, uint8_t count)
{
  if (count == 0)
    return a;

  __m256i val = _mm256_srli_epi16(a, count);
  val = _mm256_and_si256(val, _mm256_set1_epi8(0b01111111 >> (count - 1)));

  return val;
}

__m256i _mm256_slli_epi8(__m256i a, uint8_t count)
{
  if (count == 0)
    return a;

  __m256i val = _mm256_slli_epi16(a, count);
  val = _mm256_and_si256(val, _mm256_set1_epi8(0b11111110 << (count - 1)));

  return val;
}