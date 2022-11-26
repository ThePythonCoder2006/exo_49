#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <windows.h>

typedef struct xorshift_state
{
	uint64_t a;
} xorshift_state;

uint64_t xorshift(xorshift_state *s)
{
	uint64_t x = s->a;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	s->a = x;
	return x * 0x2545F4914F6CDD1DULL;
}

xorshift_state *state;

#define ITER 5000000

int main(int argc, char **argv)
{
	--argc, ++argv;

	state = malloc(sizeof(*state));
	srand(time(NULL));
	state->a = rand() + 1ULL;

	uint64_t count = 0;

	for (size_t i = 0; i < ITER; ++i)
	{
		if (xorshift(state) <= (UINT64_MAX / 4 * 3))
			++count;
	}

	printf("%" PRIu64 ", %f\n", count, (double)count / ITER);

	return EXIT_SUCCESS;
}