/*

  Name: Shubhayu Shrestha
  ID:   1001724804

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h> /* Symbolic Constants */

#define MAX 5000000
#define NUMTHREADS 4

int total = 0;
int n1, n2;
char *s1, *s2;


FILE *fp;

// decalring the mutex
pthread_mutex_t mutexlock;

int readf(char *filename)
{
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("ERROR: can’t open %s!\n", filename);
        return 0;
    }

    s1 = (char *)malloc(sizeof(char) * MAX);

    if (s1 == NULL)
    {
        printf("ERROR: Out of memory!\n");
        return -1;
    }

    s2 = (char *)malloc(sizeof(char) * MAX);

    if (s1 == NULL)
    {
        printf("ERROR: Out of memory\n");
        return -1;
    }

    /*read s1 s2 from the file*/

    s1 = fgets(s1, MAX, fp);
    s2 = fgets(s2, MAX, fp);
    n1 = strlen(s1);     /*length of s1*/
    n2 = strlen(s2)-1; /*length of s2*/

    if (s1 == NULL || s2 == NULL || n1 < n2) /*when error exit*/
    {
        return -1;
    }
}

void *num_substring(void *arg)
{
    int i, j, k;
    int count;

    int threadID = *(int*)arg; // int that will store the thread ID

    int start = threadID * (n1/NUMTHREADS); // creating a start int that will store the
                                            // start value using pthread_self which will 
                                            // get the current thread's ID
    
    int end = (threadID + 1) * (n1/NUMTHREADS);   // create an end int that will store the 
                                                  // end value

    
    // checking for overlaps by checking the next set the string
    // to make sure the word is not split up
    if(threadID < NUMTHREADS-1)
    {
        end+=(n2-1);
    }    

    // multithread this code 
    for (i = start; i < end-1; i++)
    {
        count = 0;
        for (j = i, k = 0; k < n2; j++, k++)
        { /*search for the next string of size of n2*/
            if (*(s1 + j) != *(s2 + k))
            {
                break;
            }
            else
            {
                count++;
            }
            if (count == n2)
            {
                // using mutex lock as this variable is shared among threads
                pthread_mutex_lock(&mutexlock);
                total++; /*find a substring in this step*/
                pthread_mutex_unlock(&mutexlock);
            }
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Error: You must pass in the datafile as a commandline parameter\n");
    }

    readf(argv[1]);

    struct timeval start, end;
    float mtime;
    int secs, usecs;

    // creating thread
    pthread_t thread[NUMTHREADS];
    int array[NUMTHREADS];

    gettimeofday(&start, NULL);


    for(int i = 0; i < NUMTHREADS; i++)
    {
        array[i] = i; 
        pthread_create(&thread[i], NULL, num_substring, (void*)&array[i]);
    } 

    for(int j = 0; j < NUMTHREADS; j++)
    {
        pthread_join(thread[j], NULL);
    }

    gettimeofday(&end, NULL);

    secs = end.tv_sec - start.tv_sec;
    usecs = end.tv_usec - start.tv_usec;
    mtime = ((secs)*1000 + usecs / 1000.0) + 0.5;

    printf("The number of substrings is : %d\n", total);
    printf("Elapsed time is : %f milliseconds\n", mtime);
    if (s1)
    {
        free(s1);
    }

    if (s2)
    {
        free(s2);
    }

    return 0;
}
