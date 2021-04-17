#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

/** \brief storage region */
char *fileNamesRegion[3];

double results[3][1024];

/** \brief flag signaling the data transfer region is full */
static bool empty;

/** \brief locking flag which warrants mutual exclusion inside the monitor */
static pthread_mutex_t accessCR = PTHREAD_MUTEX_INITIALIZER;

/** \brief flag which warrants that the data transfer region is initialized exactly once */
static pthread_once_t init = PTHREAD_ONCE_INIT;;

/** \brief producers synchronization point when the data transfer region is full */
static pthread_cond_t regionFull;

/** \brief consumers synchronization point when the data transfer region is empty */
static pthread_cond_t regionEmpty;

/**
 *  \brief Initialization of the data transfer region.
 *
 *  Internal monitor operation.
 */

static void initialization (void)
{                                                                            
  empty = true;                                                                                  

  pthread_cond_init (&regionFull, NULL);                                 
  pthread_cond_init (&regionEmpty, NULL);                                
}


void storeFileNames (int nFileNames, char *fileNames[]) {
   
    if ((pthread_mutex_lock (&accessCR)) != 0) {                                                           
       perror ("error on entering monitor(CF)");
       int status = EXIT_FAILURE;
       pthread_exit(&status);
    }

    printf("Storing files.\n");
    
    pthread_once(&init, initialization);
    
    for (int i=0; i<nFileNames; i++) {
        fileNamesRegion[i] = fileNames[i];
    }


    empty = false;                                                                      

    if ((pthread_cond_signal (&regionEmpty)) != 0) {                                                           
       perror ("error on signaling regionEmpty");
       int status = EXIT_FAILURE;
       pthread_exit(&status);
    }


    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                           
       perror ("error on exiting monitor(CF)");
       int status = EXIT_FAILURE;
       pthread_exit(&status);
    }

    printf("Done storing files.\n");
}

int processConvPoint(int threadId, int *fileId, int *n, double *x,double *y, int *point) {

    if ((pthread_mutex_lock (&accessCR)) != 0) {                                                           
        perror ("error on entering monitor(CF)");
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    pthread_once (&init, initialization);                                           

    while (empty)                                          
    { if ((pthread_cond_wait (&regionEmpty, &accessCR)) != 0) {                                                           
        perror ("error waiting on regionEmpty");
        int status = EXIT_FAILURE;
        pthread_exit(&status);
        }
    }

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                           
        perror ("error on exiting monitor(CF)");
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }
    
    if (*point != *n) return 0 ;
    else return 1;
}


void savePartialResult (int threadId, int fileId, int point, double val) {

    if ((pthread_mutex_lock (&accessCR)) != 0) {                                                           
        perror ("error on entering monitor(CF)");
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    pthread_once (&init, initialization);                                           

    results[fileId][point]=val;

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                           
        perror ("error on exiting monitor(CF)");
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }
    
}
