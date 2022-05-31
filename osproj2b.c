#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int n;       // size of fibonacci sequence.
int *fibseq; // arry holds the value of each fibonacci term.
int i;       // counter for the threads.

void *runfib(void *arg);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Must include two numbers when running program \n");
        return -1;
    }

    if (atoi(argv[1]) < 0)
    {
        printf("%d must be>=0\n", atoi(argv[1]));
        return -1;
    }

    n = atoi(argv[1]);
    fibseq = (int *)malloc(n * sizeof(int));
    pthread_t *threads = (pthread_t *)malloc(n * sizeof(pthread_t));
    pthread_attr_t attr;

    pthread_attr_init(&attr);

    for (i = 0; i < n; i++)
    {
        pthread_create(&threads[i], &attr, runfib, NULL);
    } // End of creating threads.

    int j;

    for (j = 0; j < n; j++)
    {
        pthread_join(threads[j], NULL);
    } // End of wating the threads to exit.

    // printing fibseq.
    printf("The Fibonacci sequence:");
    int k;

    for (k = 0; k < n; k++)
    {
        printf("%d,", fibseq[k]);
    } // End of printing fibseq.
    return 0;
} // End of main.

void *runfib(void *arg)
{
    if (i == 0)
    {
        fibseq[i] = 0;
        pthread_exit(0);
    } // first fib term

    if (i == 1)
    {
        fibseq[i] = 1;
        pthread_exit(0);
    } // seconed fib term
    else
    {
        fibseq[0] = 0;
        fibseq[1] = 1;

        int p, pp, fibp, fibpp;

        p = (i - 1);
        pp = (i - 2);
        fibp = fibseq[p];

        fibpp = fibseq[pp];

        fibseq[i] = fibseq[i - 1] + fibseq[i - 2];

        pthread_exit(0); // thread exit.
    }                    // End of else
}