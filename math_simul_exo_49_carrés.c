#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define GRID_SIZE 4
#define ITER 100000
#define SAMPLE_SIZE 20000

uint8_t print_grid(uint8_t *grd);

uint8_t update_grid(void);

uint8_t grid_arr[GRID_SIZE] = {1, 0, 0, 0};
uint8_t *grid = &(grid_arr[0]);

int main(int argc, char **argv)
{
    --argc, ++argv;

    srand(time(NULL));

    uint64_t counts[SAMPLE_SIZE] = {0};

    for (int k = 0; k < SAMPLE_SIZE; ++k)
    {
        if (k % 100 == 0)
            printf("%i\n", k);

        for (uint64_t j = 0; j < ITER; ++j)
        {
            // printf("etape %i: \n", index);

            // print_grid(grid);

            grid[0] = 1;
            grid[1] = 0;
            grid[2] = 0;
            grid[3] = 0;

            // print_grid(grid);

            for (uint8_t i = 0; i < 3; ++i)
                update_grid();

            // print_grid(grid);

            if (grid[3] == 1)
                ++(counts[k]);

            // printf("%i\n",count);
        }

        // printf("la proba que la case 3 soit noir au bout de la 3eme etape est de : %lf\n", (double)counts[k] / ITER);
    }

    // calculating standart deviation

    uint64_t num = 0;

    // mean of data points
    for (int i = 0; i < SAMPLE_SIZE; ++i)
        num += counts[i];

    double mean = (double)num / SAMPLE_SIZE;

    // printf("la moyennes des valeur obtenues est de %f\n", mean);

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

    printf("la deviation standart est de %f sigma", std_deviation);

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
        if (rand() % 2 == 0)
            new_grid[1] = 1;
        else
            new_grid[1] = 0;

        if (rand() % 2 == 0)
            new_grid[2] = 1;
        else
            new_grid[2] = 0;
    }
    else
    {
        if (rand() % 10 == 0)
            new_grid[1] = 1;
        else
            new_grid[1] = 0;

        if (rand() % 10 == 0)
            new_grid[2] = 1;
        else
            new_grid[2] = 0;
    }

    if (grid[1] || grid[2])
    {
        if (rand() % 2 == 0)
            new_grid[0] = 1;
        else
            new_grid[0] = 0;

        if (rand() % 2 == 0)
            new_grid[3] = 1;
        else
            new_grid[3] = 0;
    }
    else
    {
        if (rand() % 10 == 0)
            new_grid[0] = 1;
        else
            new_grid[0] = 0;

        if (rand() % 10 == 0)
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