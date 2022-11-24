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
#define ITER 200000
#define SAMPLE_SIZE 100000
#define LOGFILE "logs"
#define BUFF_SIZE 64

typedef struct xorshift_state
{
	uint64_t a;
} xorshift_state;

uint8_t print_grid(uint8_t *grd);

uint8_t update_grid(void);

uint64_t xorshift64(xorshift_state *s);

uint8_t grid_arr[GRID_SIZE] = {1, 0, 0, 0};
uint8_t *grid = &(grid_arr[0]);

xorshift_state *state;

int main(int argc, char **argv)
{
	// getting the run number to not overwrite the previous run
	uint64_t run_count;

	FILE *f = fopen("run_numb", "r");
	if (f != NULL)
		fread(&run_count, sizeof(run_count), 1, f);

	fclose(f);

	printf("%" PRIu64 "\n", run_count);

	// update the run number
	f = fopen("run_numb", "w");

	++run_count;

	printf("%" PRIu64 "\n", run_count);

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

	// I do not care about the args
	(void)argc, (void)argv;

	srand(time(NULL));
	state = malloc(sizeof(*state));
	state->a = (rand() + 1ULL) << 16;
	xorshift64(state);

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

			grid[0] = 1;
			grid[1] = 0;
			grid[2] = 0;
			grid[3] = 0;

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

	if (grid[0] || grid[3])
	{
		if (xorshift64(state) & 1)
			new_grid[1] = 1;
		else
			new_grid[1] = 0;

		if (xorshift64(state) & 1)
			new_grid[2] = 1;
		else
			new_grid[2] = 0;
	}
	else
	{
		if (xorshift64(state) <= (UINT64_MAX / 10))
			new_grid[1] = 1;
		else
			new_grid[1] = 0;

		if (xorshift64(state) <= (UINT64_MAX / 10))
			new_grid[2] = 1;
		else
			new_grid[2] = 0;
	}

	if (grid[1] || grid[2])
	{
		if (xorshift64(state) & 1)
			new_grid[0] = 1;
		else
			new_grid[0] = 0;

		if (xorshift64(state) & 1)
			new_grid[3] = 1;
		else
			new_grid[3] = 0;
	}
	else
	{
		if (xorshift64(state) <= (UINT64_MAX / 10))
			new_grid[0] = 1;
		else
			new_grid[0] = 0;

		if (xorshift64(state) <= (UINT64_MAX / 10))
			new_grid[3] = 1;
		else
			new_grid[3] = 0;
	}

	// printf("0x%p, 0x%p\n", grid, new_grid);

	// printf("print new_grid\n");

	// print_grid(new_grid);

	memcpy(grid, new_grid, GRID_SIZE * sizeof(uint8_t));

	// printf("0x%p, 0x%p\n", grid, new_grid);

	// print_grid(grid);

	return 0;
}

uint64_t xorshift64(xorshift_state *s)
{
	uint64_t x = s->a;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	return s->a = x;
}