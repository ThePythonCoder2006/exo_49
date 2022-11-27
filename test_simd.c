#include <stdio.h>
#include <immintrin.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>

/* --- PRINTF_BYTE_TO_BINARY macro's --- */
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c "
#define PRINTF_BYTE_TO_BINARY_INT8(i) \
	(((i)&0x80ll) ? '1' : '0'),         \
			(((i)&0x40ll) ? '1' : '0'),     \
			(((i)&0x20ll) ? '1' : '0'),     \
			(((i)&0x10ll) ? '1' : '0'),     \
			(((i)&0x08ll) ? '1' : '0'),     \
			(((i)&0x04ll) ? '1' : '0'),     \
			(((i)&0x02ll) ? '1' : '0'),     \
			(((i)&0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
	PRINTF_BINARY_PATTERN_INT8 PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) \
	PRINTF_BYTE_TO_BINARY_INT8((i) >> 8), PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32 \
	PRINTF_BINARY_PATTERN_INT16 PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i) \
	PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
#define PRINTF_BINARY_PATTERN_INT64 \
	PRINTF_BINARY_PATTERN_INT32 PRINTF_BINARY_PATTERN_INT32
#define PRINTF_BYTE_TO_BINARY_INT64(i) \
	PRINTF_BYTE_TO_BINARY_INT32((i) >> 32), PRINTF_BYTE_TO_BINARY_INT32(i)
/* --- end printf binary macros --- */

typedef struct
{
	__m256i a;
} xorshift_state;

void xorshift(__m256i *const dst, xorshift_state *const s)
{
	__m256i x = s->a;																	 // uint64_t x = state->a;
	x = _mm256_xor_si256(x, _mm256_slli_epi64(x, 13)); // x ^= x << 13;
	x = _mm256_xor_si256(x, _mm256_srli_epi64(x, 7));	 // x ^= x >> 7;
	x = _mm256_xor_si256(x, _mm256_slli_epi64(x, 17)); // x ^= x << 17;
	*dst = s->a = x;																	 // return state->a = x;
}

__m256i popcount_pshufb32(__m256i v)
{

	__m256i lookup = _mm256_setr_epi8(0, 1, 1, 2, 1, 2, 2, 3, 1, 2,
																		2, 3, 2, 3, 3, 4, 0, 1, 1, 2, 1, 2, 2, 3,
																		1, 2, 2, 3, 2, 3, 3, 4);
	__m256i low_mask = _mm256_set1_epi8(0x0f);
	__m256i lo = _mm256_and_si256(v, low_mask);
	__m256i hi = _mm256_and_si256(_mm256_srli_epi16(v, 4), low_mask);
	__m256i popcnt1 = _mm256_shuffle_epi8(lookup, lo);
	__m256i popcnt2 = _mm256_shuffle_epi8(lookup, hi);
	__m256i sum8 = _mm256_add_epi8(popcnt1, popcnt2);
	return _mm256_madd_epi16(_mm256_maddubs_epi16(sum8, _mm256_set1_epi8(1)),
													 _mm256_set1_epi16(1));
}

xorshift_state state;

int main(int argc, char **argv)
{
	srand(time(NULL));

	__m256i rng;

	state.a = _mm256_set1_epi64x(1);

	for (uint8_t i = 0; i < 16; ++i)
		xorshift(&rng, &state);

	rng = _mm256_set1_epi64x(1);

	printf("%i", popcount_pshufb32)

			uint64_t *res = (uint64_t *)&rng;

	for (int i = 0; i < 4; ++i)
	{
		// printf("ok %i\n", i);
		printf(PRINTF_BINARY_PATTERN_INT64, PRINTF_BYTE_TO_BINARY_INT64(res[i]));
	}

	return EXIT_SUCCESS;
}

__m256i f()
{
	return _mm256_set1_epi8(0);
}

// __m256i _mm256_srli_epi8(__m256i a, uint8_t count)
// {
// 	if (count == 0)
// 		return a;

// 	__m256i val = _mm256_srli_epi16(a, count);
// 	val = _mm256_and_si256(val, _mm256_set1_epi8(0b01111111 >> (count - 1)));

// 	return val;
// }

// __m256i _mm256_slli_epi8(__m256i a, uint8_t count)
// {
// 	if (count == 0)
// 		return a;

// 	__m256i val = _mm256_slli_epi16(a, count);
// 	val = _mm256_and_si256(val, _mm256_set1_epi8(0b11111110 << (count - 1)));

// 	return val;
// }