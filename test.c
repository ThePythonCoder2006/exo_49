#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>

__m256i ret();

int main(int argc, char **argv)
{
    __m256i t = ret();

    return EXIT_SUCCESS;
}

__m256i ret()
{
    return _mm256_set1_epi8(0);
}