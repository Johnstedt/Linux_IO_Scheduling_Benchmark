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
#include <string.h>

#define NUM_OF_THREADS 18

typedef struct keepTime
{
    double timeStart;
    double timeRun;
    double timeFinish;
} keepTime;

struct keepTime keep[NUM_OF_THREADS];

void *pthEmpty(void *thread_data);
int intLen[NUM_OF_THREADS];
char *operations[NUM_OF_THREADS];
char *benchData[NUM_OF_THREADS];
char readWrite[2] = {'r', 'w'};
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
};

double getWallTime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    printf("%e\n", (t.tv_usec / 1000000.0));
    double currently = t.tv_sec + t.tv_usec / 1000000.0;
    printf("then dubble %e\n", currently);
    return currently;
}

int main(int argc, char *argv[])
{
    long threadIdent;
    pthread_t *threads;
    double start, stop;
    int large = 10000000; // 10 million
    int medium = 100000;  // 100K
    int small = 1000;     // K
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
            op = &readWrite[0];
        }
        else // ODD WRITE
        {
            op = &readWrite[1];
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
        ret = pthread_create(&threads[threadIdent], NULL, pthEmpty, &thread_num[threadIdent]);
        keep[threadIdent].timeStart = getWallTime();
        if (ret)
        {
            fprintf(stderr, "return value pthread_create: %d\n", ret);
            exit(1);
        }
    }

    //EXIT THREADS
    for (threadIdent = 0; threadIdent < NUM_OF_THREADS; threadIdent++)
    {
        ret = pthread_join(threads[threadIdent], NULL);
        if (ret)
        {
            fprintf(stderr, "return value pthread_join: %d\n", ret);
            exit(1);
        }
    }

    //PRINT RESULTS
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        printf("%d %d %.1s %e %e\n",
               intLen[i],
               i,
               operations[i],
               keep[i].timeRun - keep[i].timeStart,
               keep[i].timeFinish - keep[i].timeStart);
    }

    free(threads);

    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        free(benchData[i]);
    }

    return 0;
}

void *pthEmpty(void *self)
{
    int myself = *(int *)self; // I cant handle pointers

    keep[myself].timeRun = getWallTime();

    FILE *fp;

    char value;

    sprintf(&value, "%d", myself);

    char* fileName = malloc(12 + strlen(&value));

    strcpy(fileName, "garbage");
    strcat(fileName, &value);

    fp = fopen(fileName, op);
    if (*op == 'r')
    {
        fgets(benchData[myself], intLen[myself], fp);
    }
    else 
    {
        fputs(benchData[myself], fp);
    }
    fclose(fp);

    keep[myself].timeFinish = getWallTime();
}