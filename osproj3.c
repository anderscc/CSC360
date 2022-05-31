//********************************************************************
//
// I'm a C. Programmer
// Operating Systems
// Programming Project #3: Using Pthreads: The Producer / Consumer Problem With Prime Number Detector
// September 26, 2021
// Instructor: Dr. Siming Liu
//
//********************************************************************

#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdbool.h>
#include "buffer.h"

bool checkPrime(int value);

bool runSim = true;
int simLength;
int maxSleepTime;
int numProducers;
int numConsumers;
int snapshot;
int timesFull = 0;
int timesEmpty = 0;
int tail;
int head;
int numberofItems;

struct thread_t
{
    pthread_t id;
    unsigned int num;
};

struct thread_t *thds;

void *producer(void *param);
void *consumer(void *param);

void initialize_threads();
void initialize_sync();
void initialize_buffer();
void printStats();
void print_buffer();

buffer_item buffer[BUFFER_SIZE];

sem_t sem_full;
sem_t sem_empty;
pthread_mutex_t mutex;

//main function
int main(int argc, char *argv[])
{
    int i;

    if (argc != 6)
    {
        printf("Enter exactly 5 paramaters.");
        exit(-1);
    }

    simLength = atoi(argv[1]);
    maxSleepTime = atoi(argv[2]);
    numProducers = atoi(argv[3]);
    numConsumers = atoi(argv[4]);
    snapshot = strcmp(argv[5], "yes") == 0;

    initialize_buffer();
    initialize_sync();

    if (snapshot)
    {
        printf("Starting Threads…\n");
        print_buffer();
    }

    initialize_threads();
    sleep(simLength);

    runSim = false;

    for (i = 0; i < numProducers + numConsumers; i++)
    {
        pthread_cancel(thds[i].id);
        pthread_join(thds[i].id, NULL);
    }

    printStats();
    free(thds);

    return 0;
}

//********************************************************************
//
// Producer Function
//
// This function serves as the producer. Generates a random number.
// It inserts random generated item into
// the buffer. Stores the number of items produced by each thread
// by incrementing num++. Also sleeps for required time frame.
//
// Return Value
// ------------
// void
//
// Reference Parameters
// ----------------
// param	void pointer		void pointer paramater
//
//
//
//*******************************************************************
void *producer(void *param)
{

    struct thread_t *t = (struct thread_t *)param;

    int id = (int)t->id;
    int seed = time(NULL) + id;

    while (runSim)
    {
        int value = rand_r(&seed) % 100;

        buffer_insert_item(value);
        t->num++;

        value = rand_r(&seed) % maxSleepTime;
        sleep(value);
    }

    return NULL;
}

//********************************************************************
//
// Consumer Function
//
// This function serves as the consumer. It removes items from
// the buffer, increments the number of items consumed by the thread
// and sleeps for the required time frame
//
// Return Value
// ------------
// void
//
// Reference Parameters
// ----------------
// void		void pointer
//
//
//*******************************************************************
void *consumer(void *param)
{
    struct thread_t *t = (struct thread_t *)param;
    int id = (int)t->id;
    int seed = time(NULL) + id;

    while (runSim)
    {
        int value;
        buffer_remove_item(&value);

        t->num++;

        /* sleep a random time */
        value = rand_r(&seed) % maxSleepTime;
        sleep(value);
    }

    return NULL;
}

//********************************************************************
//
// Initiasize Sync Function
//
// This function creates the mutex to be used as a "lock and key" item.
// This function also initializes the sempahores to signal when the buffer is
// empty or full. Full semaphore is set to 0 items. Empty is set to 5(BUFFER_SIZE).
//
// Return Value
// ------------
// void
//
// Value Parameters
// ----------------
//
// Reference Parameters
// --------------------
//
//*******************************************************************
void initialize_sync()
{
    pthread_mutex_init(&mutex, NULL);
    sem_init(&sem_full, 0, 0);
    sem_init(&sem_empty, 0, BUFFER_SIZE);
}

//********************************************************************
//
// Initialize Thread Function
//
// This function creates the number of threads required for
// the producer and consumer.
//
// Return Value
// ------------
// void
//
// Value Parameters
// ----------------
// void
//
// Reference Parameters
// --------------------
//
//*******************************************************************
void initialize_threads()
{
    int i;
    thds = (struct thread_t *)calloc(numProducers + numConsumers, sizeof(struct thread_t));

    for (i = 0; i < numProducers; i++)
    {
        pthread_create(&thds[i].id, NULL, producer, (void *)&thds[i]);
    }

    for (i = numProducers; i < numProducers + numConsumers; i++)
    {
        pthread_create(&thds[i].id, NULL, consumer, (void *)&thds[i]);
    }
}

//********************************************************************
//
// Print Stats Function
//
// Function to print the statistics at the end of the simulation.
//
// Return Value
// ------------
// void
//
// Value Parameters
// ----------------
//
// Reference Parameters
// --------------------
//
//*******************************************************************
void printStats()
{
    int totalProduced = 0;
    int totalConsumed = 0;
    int i;

    char cache[64];

    for (i = 0; i < numProducers; i++)
    {
        totalProduced += thds[i].num;
    }

    for (i = numProducers; i < numProducers + numConsumers; i++)
    {
        totalConsumed += thds[i].num;
    }

    printf("\nPRODUCER / CONSUMER SIMULATION COMPLETE\n");

    printf("== == == == == == == == == == == == == == == == == == == =\n");

    printf("%-40s %d\n", "Simulation Time:", simLength);
    printf("%-40s %d\n", "Maximum Thread Sleep Time:", maxSleepTime);
    printf("%-40s %d\n", "Number of Producer Time:", numProducers);
    printf("%-40s %d\n", "Number of Consumer Time:", numConsumers);
    printf("%-40s %d\n", "Size of Buffer:", BUFFER_SIZE);
    printf("%-40s %d\n", "Total Number of Items Produced:", totalProduced);

    for (i = 0; i < numProducers; i++)
    {
        sprintf(cache, " Thread %d    :", i + 1);
        printf("%-40s %d\n", cache, thds[i].num);
    }

    printf("\n %-40s %d\n", "Total Number of Items Consumed:", totalConsumed);

    for (i = numProducers; i < numProducers + numConsumers; i++)
    {
        sprintf(cache, " Thread %d :", i + 1);
        printf("%-40s %d\n", cache, thds[i].num);
    }

    printf("\n");
    printf("% -40s% d\n", "Number Of Items Remaining in Buffer :", numberofItems);
    printf("% -40s% d\n", "Number Of Times Buffer Was Full   :", timesFull);
    printf("%-40s% d\n", "Number Of Times Buffer Was Empty   :", timesEmpty);
}

//********************************************************************
//
// Initialize Buffer Function
//
// Function to initialize all buffer values to -1 and
// sets head, tail and number of item integers to 0.
// Resets all values.
//
// Return Value
// ------------
// void
//
// Value Parameters
// ----------------
//
// Reference Parameters
// --------------------
//
//*******************************************************************
void initialize_buffer()
{
    int i;
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = -1;
    }

    tail = 0;
    head = 0;
    numberofItems = 0;
}

//********************************************************************
//
// Buffer Insert Item Function
//
// This function inserts an item into the buffer and
// increments the number of items in buffer. If buffer is full, then a wait
// is implement until the buffers are empty. Locks mutex when writing items and unlocks
// after use. Sempaphore signals before and after use
//
// Return Value
// ------------
// bool                         True/False if inserted successfully
//
// Value Parameters
// ----------------
// item		buffer_item		buffer item to be inserted
//
// Reference Parameters
// --------------------
//
//*******************************************************************
bool buffer_insert_item(buffer_item item)
{
    long id = (long)pthread_self();

    if (numberofItems >= BUFFER_SIZE)
    {
        printf("\n All buffers full.Producer % ld waits.\n", id);
        timesFull++;
    }

    sem_wait(&sem_empty);
    pthread_mutex_lock(&mutex);

    numberofItems++;
    buffer[head] = item;

    head = (head + 1) % BUFFER_SIZE;

    if (snapshot)
    {
        printf("\nProducer % ld writes %d\n", id, item);
        print_buffer();
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&sem_full);

    return 1;
}

//********************************************************************
//
// Buffer Remove Item Function
//
// This function performs a linear search on an list of student
// records.  If the search key is found in the list, the return
// value is true and false otherwise.
//
// Return Value
// ------------
// bool                 true or false return type if removed successfully
//
// Value Parameters
// ----------------
// buffer item		item		buffer item to be removed
//
// Reference Parameters
// --------------------
//
//*******************************************************************
bool buffer_remove_item(buffer_item *item)
{
    buffer_item value;
    long id = (long)pthread_self();

    if (numberofItems == 0)
    {
        printf("\n Buffer is empty. Consumer % ld waits.\n", id);
        timesEmpty++;
    }

    sem_wait(&sem_full);
    pthread_mutex_lock(&mutex);

    numberofItems--;
    value = buffer[tail];

    tail = (tail + 1) % BUFFER_SIZE;

    if (snapshot)
    {

        if (checkPrime(value))
        {
            printf("\nConsumer % ld reads %d * **PRIME * **\n", id, value);
        }
        else
        {
            printf("\nConsumer % ld reads % d\n", id, value);
        }

        print_buffer();
    }

    pthread_mutex_unlock(&mutex);
    sem_post(&sem_empty);

    *item = value;

    return 1;
}

//********************************************************************
//
// Check Prime Function
//
// This function takes an integer and checks the to see if
// the value is a prime number or not.
//
// Return Value
// ------------
// bool                         True/False if successfully checked
//
// Value Parameters
// ----------------
// value	int		value to be checked if prime
//
// Reference Parameters
// --------------------
//
//*******************************************************************
bool checkPrime(int value)
{
    int i;
    for (i = 2; i <= value; i++)
    {
        if (value % i == 0)
        {
            return 0;
        }
    }

    return 1;
}

//********************************************************************
//
// Print Buffer Function
//
// This function prints the contents of the buffer at a given point in time.
// Prints buffer occupancy and also Read and Write status.
//
// Return Value
// ------------
// void
//
// Value Parameters
// ----------------
//
// Reference Parameters
// --------------------
//
//*******************************************************************

void print_buffer()
{
    printf("(Buffers occupied : %d)\n", numberofItems);
    printf("Buffers:");

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf(" %3d ", buffer[i]);
    }
    printf("\n");

    // Third line of output. puts dashes under numbers
    printf("        ");
    for (int i = 0; i < BUFFER_SIZE; i++)
        printf(" ----");
    printf("\n");

    // Fourth line of output. Shows position of in & out indexes
    printf("         ");
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        printf(" ");

        if (head == i || tail == i)
        {
            if (numberofItems == 0)
                printf("WR");
            else if (numberofItems == BUFFER_SIZE)
                printf("RW");
            else if (head == i)
                printf("W ");
            else
                printf("R ");

            printf(" ");
        }
        else
            printf("    ");
    }
    printf("\n \n");
}
