#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include "work.h"

typedef int Result;


static void usage(const char *me)
{
    fprintf(stderr, "Usage: %s work_units\n", me);
}


int main(int argc, const char* const argv[])
{
    if(argc<2)
    {
        usage(argv[0]);
        return 1;
    }
    const int work_units = atoi(argv[1]);
    if(work_units <= 0)
    {
        usage(argv[0]);
        return 1;
    }
    printf("Will process %d unit(s) of work\n", work_units);

    Result results[work_units];
    int result_prev_idx = -1;
    for(Work work_unit = 1; work_unit <= work_units; ++work_unit)
    {
        sleep_by_work_unit(work_unit);
        Result result = work_unit*1000;

        results[++result_prev_idx] = result;
    }

    // Print the results
    printf("Got %d result(s):", result_prev_idx+1);
    for(int i=0; i<=result_prev_idx; ++i)
    {
        printf(" %d", results[i]);
    }
    printf("\n");
    return 0;
}
