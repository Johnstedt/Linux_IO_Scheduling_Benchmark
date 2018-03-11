/**
 * Benchmarking linux policies
 * John-John Markstedt
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#define NUM_OF_THREADS 16

typedef struct keepTime
{
    double timeStart;
    double timeRun;
    double timeFinish
} keepTime;

struct keepTime keep[NUM_OF_THREADS];

void *pthEmpty(void *thread_data);
int intLen[NUM_OF_THREADS];
char *operations[NUM_OF_THREADS];
char *benchData[NUM_OF_THREADS];
char *readWrite[] = {'R', 'W'};
char *op;

// 3 MUST | LENGTH
char *tests[] = {
    "1garbage.txt",
    "2garbage.txt",
    "3garbage.txt",
    "4garbage.txt",
    "5garbage.txt",
    "6garbage.txt",
    "7garbage.txt",
    "8garbage.txt",
    "9garbage.txt",
    "10garbage.txt",
    "11garbage.txt",
    "12garbage.txt",
    "13garbage.txt",
    "14garbage.txt",
    "15garbage.txt",
    "16garbage.txt",
    "17garbage.txt",
    "18garbage.txt",
}

void getWallTime(currently)
{
    struct timeval t;
    gettimeofday(&t, NULL);
    currently = t.tv_sec + t.tv_usec / 1000000.0;
}

main(int argc, char *argv[])
{
    int threadIdent;
    pthread *threads;
    double start, stop;
    int large = 1000000; // One million
    int medium = 10000;  // 10K
    int small = 100;     // Hundred
    int ret = 0;

    threads = calloc(NUM_OF_THREADS, sizeof(threads));

    int thread_num[NUM_OF_THREADS];
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        thread_num[i] = i;
    }

    // DIVIDE WORK BETWEEN THREADS
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        if (i < NUM_OF_THREADS / 3)
        {
            intLen[i] = large;
        }
        else if (i >= NUM_OF_THREADS / 3 && i < (2 * NUM_OF_THREADS / 3))
        {
            intLen[i] = medium;
        }
        else
        {
            intLen[i] = small;
        }

        if (i % 2 == 0) // EVEN READ
        {
            op = readWrite[0];
        }
        else // ODD WRITE
        {
            op = readWrite[1];
        }

        operations[i] = malloc(sizeof(char));
        operations[i] = op;

        benchData[i] = malloc(sizeof(char) * intLen[i]);
        for (int j = 0; j < intLen[i]; j++)
        {
            benchData[i][j] = 'd';
        }
    }

    //INIT THREADS
    for (threadIdent = 0; threadIdent < NUM_OF_THREADS; threadIdent++)
    {
        retVal = pthread_create(&threads[threadIdent], NULL, pthEmpty, &thread_num[threadIdent]);
        getWallTime(keep[thread].timeStart);
        if (retVal)
        {
            fprintf(stderr, "return value pthread_create: %d\n", retVal);
            exit(1);
        }
    }

    //EXIT THREADS
    for (threadIdent = 0; threadIdent < NUMBER_THREADS; threadIdent++)
    {
        retVal = pthread_join(threads[threadIdent], NULL);
        if (retVal)
        {
            fprintf(stderr, "return value pthread_join: %d\n", retVal);
            exit(1);
        }
    }

    //PRINT RESULTS
    for (int i = 0; i < NUMBER_THREADS; i++)
    {
        printf("%d %d %s %e %e\n",
               intLen[i],
               i,
               operation[i],
               keep[i].timeFinish - keep[i].timeRun,
               keep[i].timeFinish - keep[i].timeStart);
    }

    free(threads);

    for (int i = 0; i < NUMBER_THREADS; i++)
    {
        free(benchData[i]);
    }

    return 0;
}

void *pthEmpty(void *self)
{
    int myself = *(int *)self; // I cant handle pointers

    getWallTime(keep[mySelf].timeRun);

    FILE *fp;
    fp = fopen(tests[myself], op);
    if (*op == 'R')
    {
        fgets(benchData[myself], intLen[myself], fp);
    }
    else 
    {
        fputs(benchData[myself], fp);
    }
    fclose(fp);

    getWallTime(keep[mySelf].timeFinish);
}