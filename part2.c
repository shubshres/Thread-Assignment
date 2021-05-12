/*

  Name: Shubhayu Shrestha
  ID:   1001724804

*/

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdbool.h>

#define INITIAL_STRING_SIZE 1
#define BUFFER_SIZE 5
#define NONSHARED 1

// declaring the semaphores
sem_t charRead, charAdd;

// creating the filepointer for opening the file
FILE *fp;

// creating the buffer array that will hold 5 characters at a time
char bufferArray[BUFFER_SIZE];

// using a bool to check if we reached the end of file
bool endOfFile = false;

int readFile(char *filename)
{
    // opening file and handling checking if the file opened
    if ((fp = fopen(filename, "r")) == NULL)
    {
        printf("ERROR: can’t open %s!\n", filename);
        return 0;
    }
}

void *Producer(void *arg)
{
    // creating a count variable that will hold the location of the buffer
    int bufferLocation = 0;

    // opening file named message.txt by sending it to function ReadFile
    readFile("message.txt");

    // creating a char variable that will hold the char that were examining
    char c;

    while (1)
    {
        //wait for a char to be added, and if it is consumed, we write to the buffer
        sem_wait(&charRead);

        // holding the char from the text array into another char so we can store
        // into the buffer
        c = fgetc(fp);

        // if the end of the file is reached, it will break from the while loop
        // and we will go to consumer and break out of that loop as well
        if (feof(fp))
        {
            // changing the boolean of endOfFile to true
            endOfFile = true;

            // incrementing sem_post so we move to the consumer section
            sem_post(&charAdd);

            // breaking out of the infinite while loop
            break;
        }

        // filling in the buffer array with the position variable.
        bufferArray[bufferLocation++] = c;

        //resetting the buffer position once we reach the 5th position
        if (bufferLocation == BUFFER_SIZE)
        {
            bufferLocation = 0;
        }

        sem_post(&charAdd);
    }
}

void *Consumer(void *arg)
{
    // creating a count variable that will hold the location of the buffer
    int bufferLocation = 0;

    while (1)
    {
        // wait for the character
        sem_wait(&charAdd);

        //breaking out of the loop if were at the end of the file
        if (endOfFile == true)
        {
            printf("\n"); 
            break;
        }

        // printing out what is in the buffer
        printf("%c", bufferArray[bufferLocation++]);

        // resetting buffer location
        if (bufferLocation == BUFFER_SIZE)
        {
            bufferLocation = 0;
        }

        sem_post(&charRead);
    }
}

int main(int argc, char *argv[])
{
    pthread_t producerThreadID;
    pthread_t consumerThreadID;

    sem_init(&charRead, NONSHARED, INITIAL_STRING_SIZE);
    sem_init(&charAdd, NONSHARED, 0);

    pthread_create(&producerThreadID, NULL, Producer, NULL);
    pthread_create(&consumerThreadID, NULL, Consumer, NULL);

    pthread_join(producerThreadID, NULL);
    pthread_join(consumerThreadID, NULL);

    sem_close(&charRead);
    sem_close(&charAdd);

    fclose(fp);

    return 0;
}
