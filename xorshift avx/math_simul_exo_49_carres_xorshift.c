#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <windows.h>

#define GRID_SIZE 4
#define ITER 20000
#define SAMPLE_SIZE 1000
#define LOGFILE "logs"
#define BUFF_SIZE 64

typedef struct
{
	__m256i a;
} xorshift_state;

void xorshift(__m256i *const dst, xorshift_state *const s);

uint8_t print_grid(uint8_t *grd);

uint8_t update_grid(void);

__m256i grid_arr[GRID_SIZE];
__m256i *grid = &(grid_arr[0]);

xorshift_state state;

int main(int argc, char **argv)
{
	// I do not care about the args
	(void)argc, (void)argv;

	// getting the run number to not overwrite the previous run
	uint64_t run_count;

	FILE *f = fopen("run_numb", "r");
	if (f != NULL)
		fread(&run_count, sizeof(run_count), 1, f);

	fclose(f);

	// update the run number
	f = fopen("run_numb", "w");

	++run_count;

	fwrite(&run_count, sizeof(run_count), 1, f);

	fclose(f);

	// create a new dir to hold the log of the current run
	char dir_name[BUFF_SIZE], file_name[BUFF_SIZE];
	snprintf(dir_name, BUFF_SIZE, LOGFILE "/run %" PRIu64, run_count);
	mkdir(dir_name);

	strncpy(file_name, dir_name, BUFF_SIZE);
	strcat(file_name, "/runlog.txt");
	// file to save global data like time
	FILE *run_log = fopen(file_name, "w");

	fprintf(run_log, "####################################################\n"
									 "THE LOG FILE OF THE RUN %" PRIu64 "\n"
									 "####################################################\n"
									 "CONFIG OF THIS RUN :\n"
									 "ITER = %u\n"
									 "SAMPLE_SIZE = %u\n"
									 "####################################################\n"
									 "SAMPLE TIMES :\n",
					run_count, ITER, SAMPLE_SIZE);

	// vars to measure the execution time
	LARGE_INTEGER frequency;
	LARGE_INTEGER run_start;
	LARGE_INTEGER run_end;
	LARGE_INTEGER sample_start;
	LARGE_INTEGER sample_end;
	double interval;

	// init the xorshift64 pseudorandom number genrator
	srand(time(NULL));
	state.a = _mm256_set_epi64x((rand() + 1ULL) << 16,
															(rand() + 1ULL) << 16,
															(rand() + 1ULL) << 16,
															(rand() + 1ULL) << 16);
	__m256i tmp;
	xorshift(&tmp, &state);

	uint32_t counts[SAMPLE_SIZE] = {0};

	// start to mesure time of the whole run
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&run_start);

	// repeat the exeperiment multiple times to calculate SD
	for (uint64_t k = 0; k < SAMPLE_SIZE; ++k)
	{
		// a simple (and fast) way to check execution ((1 >> 7) - 1) = (128 - 1) = 127 = 0b00000000 00000000 00000000 01111111
		// <=> k % 256 = 255
		if (k % 100 == 0)
		{
			// printf(">\n");
			QueryPerformanceCounter(&sample_start);
		}

		for (uint64_t j = 0; j < ITER; ++j)
		{

			grid[0] = _mm256_set1_epi8(UINT8_MAX);
			grid[1] = _mm256_set1_epi8(0);
			grid[2] = _mm256_set1_epi8(0);
			grid[3] = _mm256_set1_epi8(0);

			for (uint8_t i = 0; i < 2; ++i)
				update_grid();

			if (grid[3] == 1)
				++(counts[k]);
		}

		if (k % 100 == 99)
		{
			printf("%f\n", (double)k / SAMPLE_SIZE * 100.0);
			QueryPerformanceCounter(&sample_end);
			interval = (double)(sample_end.QuadPart - sample_start.QuadPart) / frequency.QuadPart;
			fprintf(run_log, "sample %" PRIu64 "-%" PRIu64 ": %f\n", k - (k >= 100 ? 100 : k), k, interval);
		}
	}

	QueryPerformanceCounter(&run_end);
	interval = (double)(run_end.QuadPart - run_start.QuadPart) / frequency.QuadPart;
	fprintf(run_log, "the run took %fs to run\n", interval);

	// creating files for logging data
	// creating the binary log file
	strncpy(file_name, dir_name, BUFF_SIZE);
	strcat(file_name, "/counts.log");
	FILE *flogs = fopen(file_name, "w");

	// creating the text log file
	strncpy(file_name, dir_name, BUFF_SIZE);
	strcat(file_name, "/counts.txt");
	FILE *ftxt = fopen(file_name, "w");

	// calculating standart deviation

	fwrite(counts, sizeof(*counts), SAMPLE_SIZE, flogs);

	uint64_t num = 0;
	// mean data points
	for (int i = 0; i < SAMPLE_SIZE; ++i)
	{
		num += counts[i];
		fprintf(ftxt, "%" PRIu32 " ", counts[i]);
	}

	double mean = (double)num / SAMPLE_SIZE;

	// printf("la moyennes des valeur obtenues est de %f\n", mean);
	printf("La probalbilite moyenne que le carre 3 soit noir a la troisieme etape est de %f = %f%%\n", mean / ITER, mean / ITER * 100);
	fprintf(run_log, "mean : %f = %f%%\n", mean / ITER, mean / ITER * 100);

	double pt_dev[SAMPLE_SIZE];

	// deviation for each point
	for (int i = 0; i < SAMPLE_SIZE; ++i)
	{
		pt_dev[i] = (counts[i] - mean) * (counts[i] - mean);
		// printf("(%i - %f)**2 = %f, %f\n", counts[i], mean, pt_dev[i], sqrt(pt_dev[i]));
	}

	double numerator = 0;

	// mean of per point deviation
	for (int i = 0; i < SAMPLE_SIZE; ++i)
		numerator += pt_dev[i];

	double std_deviation = sqrt(numerator / SAMPLE_SIZE);

	printf("la deviation standart est de %f = %f%%", std_deviation / ITER, std_deviation / ITER * 100);
	fprintf(run_log, "SD : %f = %f%%\n", std_deviation / ITER, std_deviation / ITER * 100);

	fclose(flogs);
	fclose(ftxt);

	fclose(run_log);

	return EXIT_SUCCESS;
}

uint8_t print_grid(uint8_t *grd)
{
	// printf("0x%p\n", grd);

	printf("|---|---|\n| %c | %c |\n|---|---|\n| %c | %c |\n|---|---|\n", grd[0] == 1 ? 'N' : 'B', grd[1] == 1 ? 'N' : 'B', grd[2] == 1 ? 'N' : 'B', grd[3] == 1 ? 'N' : 'B');
	return 0;
}

uint8_t update_grid(void)
{
	uint8_t new_grid_arr[GRID_SIZE] = {0, 0, 0, 0};
	uint8_t *new_grid = &(new_grid_arr[0]);

	memcpy(new_grid, grid, GRID_SIZE * sizeof(uint8_t));

	// print_grid(new_grid);

	__m256i rng;

	// printf("0x%p, 0x%p\n", grid, new_grid);

	// printf("print new_grid\n");

	// print_grid(new_grid);

	memcpy(grid, new_grid, GRID_SIZE * sizeof(uint8_t));

	// printf("0x%p, 0x%p\n", grid, new_grid);

	// print_grid(grid);

	return 0;
}

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