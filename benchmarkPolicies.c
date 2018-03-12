/**
 * Benchmarking linux policies
 * John-John Markstedt
 * 
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NUM_OF_THREADS 18

typedef struct keepTime
{
    double timeStart;
    double timeRun;
    double timeFinish;
} keepTime;

typedef struct totalTime
{
    double timeStart;
    double timeStop;
    double timeSmallReadAVG;
    double timeSmallWriteAVG;
    double timeMediumReadAVG;
    double timeMediumWriteAVG;
    double timeLargeReadAVG;
    double timeLargeWriteAVG;
} totalTime;

struct keepTime keep[NUM_OF_THREADS];
struct totalTime total;

void *pthEmpty(void *thread_data);
int intLen[NUM_OF_THREADS];
char *operations[NUM_OF_THREADS];
char *benchData[NUM_OF_THREADS];
char readWrite[2] = {'r', 'w'};
char *op;


double getWallTime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    
    double currently = t.tv_sec + t.tv_usec / 1000000.0;
    
    return currently;
}

int main(int argc, char *argv[])
{
    long threadIdent;
    pthread_t *threads;
    double start, stop;
    int large = 100000000; // 100MB
    int medium = 100000;  // 100KB
    int small = 100;     // 0.1KB
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

    total.timeStart = getWallTime();

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

    total.timeStop = getWallTime();

    //PRINT RESULTS
    for (int i = 0; i < NUM_OF_THREADS; i++)
    {
        printf("%d %d %.1s %e %e %e\n",
               intLen[i],
               i,
               operations[i],
               keep[i].timeRun - keep[i].timeStart,
               keep[i].timeFinish - keep[i].timeRun,
               keep[i].timeFinish - keep[i].timeStart);

    }
    printf("TOTAL %e \n WRITE LARGE %e \n READ LARGE %e \n \
            WRITE MED %e \n READ MED %e \n WRITE SMALL %e\n READ SMALL %e\n",
               total.timeStart - total.timeStop,
               total.timeLargeWriteAVG/NUM_OF_THREADS/6,
               total.timeLargeReadAVG/NUM_OF_THREADS/6,
               total.timeMediumWriteAVG/NUM_OF_THREADS/6,
               total.timeMediumReadAVG/NUM_OF_THREADS/6,
               total.timeSmallWriteAVG/NUM_OF_THREADS/6,
               total.timeSmallReadAVG/NUM_OF_THREADS/6
               );

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

    FILE *fp;

    char value[2];

    sprintf(value, "%d", myself);

    char* fileName = malloc(12 + strlen(value)+1);

    strcpy(fileName, "garbage");
    strcat(fileName, value);

    int fd = open(fileName, O_DIRECT | O_CREAT | O_RDWR | O_LARGEFILE);    
    fp = fdopen(fd, op);

    keep[myself].timeRun = getWallTime();

    if (*op == 'r')
    {
        fgets(benchData[myself], intLen[myself], fp);
    }
    else 
    {
        fputs(benchData[myself], fp);
    }

    fclose(fp);
    close(fd);

    keep[myself].timeFinish = getWallTime();

    if(myself < NUM_OF_THREADS / 3)
    {
        if (myself % 2 == 0) // EVEN READ
        {
            total.timeLargeReadAVG = total.timeLargeReadAVG + (keep[myself].timeFinish - keep[myself].timeRun);
        }
        else // ODD WRITE
        {
            total.timeLargeWriteAVG = total.timeLargeWriteAVG + (keep[myself].timeFinish - keep[myself].timeRun);
        }
       
    }
    else if (myself >= NUM_OF_THREADS / 3 && myself < (2 * NUM_OF_THREADS / 3))
    {
        if (myself % 2 == 0) // EVEN READ
        {
            total.timeMediumReadAVG = total.timeMediumReadAVG + (keep[myself].timeFinish - keep[myself].timeRun);
        }
        else // ODD WRITE
        {
            total.timeMediumWriteAVG = total.timeMediumWriteAVG + (keep[myself].timeFinish - keep[myself].timeRun);
        }
    }
    else
    {
        if (myself % 2 == 0) // EVEN READ
        {
            total.timeSmallReadAVG = total.timeSmallReadAVG + (keep[myself].timeFinish - keep[myself].timeRun);
        }
        else // ODD WRITE
        {
            total.timeSmallWriteAVG = total.timeSmallWriteAVG + (keep[myself].timeFinish - keep[myself].timeRun);
        }
    }


}