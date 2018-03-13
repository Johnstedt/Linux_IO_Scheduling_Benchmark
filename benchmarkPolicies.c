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
#include <math.h>

#define NUM_OF_THREADS 24

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

    double *timeSmallRead;
    double *timeSmallWrite;
    double *timeMediumRead;
    double *timeMediumWrite;
    double *timeLargeRead;
    double *timeLargeWrite;

    double timeSmallReadMedian;
    double timeSmallWriteMedian;
    double timeMediumReadMedian;
    double timeMediumWriteMedian;
    double timeLargeReadMedian;
    double timeLargeWriteMedian;

    double timeSmallReadVariance;
    double timeSmallWriteVariance;
    double timeMediumReadVariance;
    double timeMediumWriteVariance;
    double timeLargeReadVariance;
    double timeLargeWriteVariance;    

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

double median(int n, double x[]) 
{
    double temp;
    int i, j;
    
    for(i=0; i<n-1; i++) // the following two loops sort the array x in ascending order
    {
        for(j=i+1; j<n; j++) 
        {
            if(x[j] < x[i]) 
            {
                // swap elements
                temp = x[i];
                x[i] = x[j];
                x[j] = temp;
            }
        }
    }

    if( n%2 == 0 ) 
    {
        return((x[n/2] + x[n/2 - 1]) / 2.0);   // if there is an even number of elements, return mean of the two elements in the middle
    } else 
    {
        return x[n/2]; // else return the element in the middle
    }
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

        FILE *fp;

        char value[2];

        sprintf(value, "%d", i);

        char* fileName = malloc(12 + strlen(value)+1);

        strcpy(fileName, "garbage");
        strcat(fileName, value);

        printf("%s", fileName);

        fp = fopen(fileName, "w");

        if (*operations[i] == 'r')
        {
            printf("???\n" );
            fputs(benchData[i], fp);
        }else {
            printf("!!!\n");
        }

        fclose(fp);
    }

    total.timeSmallRead = calloc(NUM_OF_THREADS/6 , sizeof(double));
    total.timeSmallWrite = calloc(NUM_OF_THREADS/6 , sizeof(double));
    total.timeMediumRead = calloc(NUM_OF_THREADS/6 , sizeof(double));
    total.timeMediumWrite = calloc(NUM_OF_THREADS/6 , sizeof(double));
    total.timeLargeRead = calloc(NUM_OF_THREADS/6 , sizeof(double));
    total.timeLargeWrite = calloc(NUM_OF_THREADS/6 , sizeof(double));

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

    for(int i = 0; i < NUM_OF_THREADS/6; i++){
        total.timeSmallReadVariance = total.timeSmallReadVariance + pow((total.timeSmallRead[i] - total.timeSmallReadAVG/(NUM_OF_THREADS/6)), 2);
        total.timeSmallWriteVariance = total.timeSmallWriteVariance + pow((total.timeSmallWrite[i] - total.timeSmallWriteAVG/(NUM_OF_THREADS/6)), 2);

        total.timeMediumReadVariance = total.timeMediumReadVariance + pow((total.timeMediumRead[i] - total.timeMediumReadAVG/(NUM_OF_THREADS/6)), 2);
        total.timeMediumWriteVariance = total.timeMediumWriteVariance + pow((total.timeMediumWrite[i] - total.timeMediumWriteAVG/(NUM_OF_THREADS/6)), 2);

        total.timeLargeReadVariance = total.timeLargeReadVariance + pow((total.timeLargeRead[i] - total.timeLargeReadAVG/(NUM_OF_THREADS/6)), 2);
        total.timeLargeWriteVariance = total.timeLargeWriteVariance + pow((total.timeLargeWrite[i] - total.timeLargeWriteAVG/(NUM_OF_THREADS/6)), 2);
    }

    total.timeSmallReadMedian = median(NUM_OF_THREADS/6, total.timeSmallRead);
    total.timeSmallWriteMedian = median(NUM_OF_THREADS/6, total.timeSmallWrite);
    total.timeMediumReadMedian = median(NUM_OF_THREADS/6, total.timeMediumRead);
    total.timeMediumWriteMedian = median(NUM_OF_THREADS/6, total.timeMediumWrite);
    total.timeLargeReadMedian = median(NUM_OF_THREADS/6, total.timeLargeRead);
    total.timeLargeWriteMedian = median(NUM_OF_THREADS/6, total.timeLargeWrite);


    printf("WRITE LARGE MEDIAN %e \n READ LARGE MEDIAN %e \n WRITE MED MEDIAN %e \n READ MED MEDIAN %e \n WRITE SMALL MEDIAN %e\n READ SMALL MEDIAN %e\n",
               total.timeLargeWriteMedian,
               total.timeLargeReadMedian,
               total.timeMediumWriteMedian,
               total.timeMediumReadMedian,
               total.timeSmallWriteMedian,
               total.timeSmallReadMedian
               );

    printf("TOTAL %e \n WRITE LARGE %e \n READ LARGE %e \n WRITE MED %e \n READ MED %e \n WRITE SMALL %e\n READ SMALL %e\n",
               total.timeStop - total.timeStart,
               total.timeLargeWriteAVG/(NUM_OF_THREADS/6),
               total.timeLargeReadAVG/(NUM_OF_THREADS/6),
               total.timeMediumWriteAVG/(NUM_OF_THREADS/6),
               total.timeMediumReadAVG/(NUM_OF_THREADS/6),
               total.timeSmallWriteAVG/(NUM_OF_THREADS/6),
               total.timeSmallReadAVG/(NUM_OF_THREADS/6)
    );
    

    printf("\n WRITE LARGE VARIANCE %e \n READ LARGE VARIANCE %e \n WRITE MED VARIANCE %e \n READ MED VARIANCE %e \n WRITE SMALL VARIANCE %e\n READ SMALL VARIANCE %e\n",
               
               sqrt(total.timeLargeWriteVariance),
               sqrt(total.timeLargeReadVariance),
               sqrt(total.timeMediumWriteVariance),
               sqrt(total.timeMediumReadVariance),
               sqrt(total.timeSmallWriteVariance),
               sqrt(total.timeSmallReadVariance)
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

    
    fp = fopen(fileName, op);

    keep[myself].timeRun = getWallTime();

    if (*operations[myself] == 'r')
    {
        fgets(benchData[myself], intLen[myself], fp);
    }
    else 
    {
        fputs(benchData[myself], fp);
    }

    fclose(fp);
   

    keep[myself].timeFinish = getWallTime();

    if(myself < NUM_OF_THREADS / 3)
    {
        if (myself % 2 == 0) // EVEN READ
        {
            total.timeLargeReadAVG = total.timeLargeReadAVG + (keep[myself].timeFinish - keep[myself].timeRun);
            total.timeLargeRead[myself - (myself/2)] =  (keep[myself].timeFinish - keep[myself].timeRun);
        }
        else // ODD WRITE
        {
            total.timeLargeWriteAVG = total.timeLargeWriteAVG + (keep[myself].timeFinish - keep[myself].timeRun);
            total.timeLargeWrite[myself - ((myself)/2)-1] =  (keep[myself].timeFinish - keep[myself].timeRun);
        }
       
    }
    else if (myself >= NUM_OF_THREADS / 3 && myself < (2 * NUM_OF_THREADS / 3))
    {
        if (myself % 2 == 0) // EVEN READ
        {
            total.timeMediumReadAVG = total.timeMediumReadAVG + (keep[myself].timeFinish - keep[myself].timeRun);
            total.timeMediumRead[myself- NUM_OF_THREADS/3 - ((myself-(NUM_OF_THREADS/3))/2)] = (keep[myself].timeFinish - keep[myself].timeRun);
        }
        else // ODD WRITE
        {
            total.timeMediumWriteAVG = total.timeMediumWriteAVG + (keep[myself].timeFinish - keep[myself].timeRun);
            total.timeMediumWrite[myself- NUM_OF_THREADS/3 - ((myself-(NUM_OF_THREADS/3)-1)/2)-1] = (keep[myself].timeFinish - keep[myself].timeRun);
        }
    }
    else
    {
        if (myself % 2 == 0) // EVEN READ
        {
            total.timeSmallReadAVG = total.timeSmallReadAVG + (keep[myself].timeFinish - keep[myself].timeRun);
            total.timeSmallRead[myself - 2*NUM_OF_THREADS/3 - ((myself-(2*NUM_OF_THREADS/3))/2)] = (keep[myself].timeFinish - keep[myself].timeRun);
        }
        else // ODD WRITE
        {
            total.timeSmallWriteAVG = total.timeSmallWriteAVG + (keep[myself].timeFinish - keep[myself].timeRun);
            total.timeSmallWrite[myself- 2*NUM_OF_THREADS/3 - ((myself-(2*NUM_OF_THREADS/3)-1)/2)-1] = (keep[myself].timeFinish - keep[myself].timeRun);
        }
    }
}
