/*Author:       Brian Ekstrom
  Date:         5/4/20
  Assignment:   Threads
  Description:  Allow the user to input the number of threads they wish to run on
                and the size of the array. Then split the summation onto p amount 
                of threads and find the of each thread.  Join the threads and find 
                the total sum.  Compare that parallel sum to the serial sum that is
                found in main.
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

const long MAX_THREADS = 16;
long N;
long p;
int* A;

void*
sumArray(void* threadId);

int main (int argc, char* argv[])
{
    printf("p ==> ");
    scanf("%ld", &p);

    // Error checking.  p must be less than MAX_THREADS.
    if (p <= 0 || p > MAX_THREADS)
    {
        printf("p must be less than 16!\n");
        exit(0);
    }

    printf("N ==> ");
    scanf("%ld", &N);

    // Allocate the amount of space for A[]
    A = (int*)malloc(N * sizeof(long));
    
    // Fill A with random integers in range [0,4]
    for (long i = 0; i < N; ++i)
    {
        A[i] = rand() % 5;
    }
    
    // Creating threads based on MAX_THREADS, in this case it will be 16 threads.
    pthread_t* threads = malloc (p * sizeof(pthread_t));
    for (long threadId = 0; threadId < p; ++threadId)
    {
        pthread_create(&threads[threadId], NULL, sumArray, (void*) threadId);
    }

    // Join the threads.  Compute the parallel sum.
    long sum = 0;
    for (long threadId = 0; threadId < p; ++threadId)
    {  
        void* returnVal;
        pthread_join(threads[threadId], &returnVal);
        sum += (long) returnVal;
    }

    // Compute the sum serially
    long serialSum = 0;
    for (long i = 0; i < N; ++i)
    {
        serialSum += A[i];
    }

    // Print the parallell and serial sums so comparisons can be seen.
    printf("\nParallel sum:\t %ld\n", sum);
    printf("Serial sum:\t %ld\n", serialSum);
    
    free(threads);
    free (A);

    pthread_exit(EXIT_SUCCESS);
}

/*  Sum of the array on different threads based on user input 'p'.  Return the
    value back to main so the threads can be joined and find the sum.*/
void*
sumArray(void* threadId)
{   
    long myId = (long) threadId;
    long foo;
    for (long i = myId * (N/p); i < ((myId + 1) * (N/p)); ++i)
    {   
        foo += A[i];
    }
    void* retVal = (void *) foo;
    return retVal;
}