/**
 *  \file main.c
 *
 *  \brief Problem: Assignment 1 - Multithreading (circular cross-correlation)
 *
 *  Synchronization based on monitors (Lampson / Redell)  - Main program
 *
 *  \author Alina Yanchuk e Sofia Moniz
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#include "sharedRegion.h"

/** \brief worker life cycle routine */
static void *worker (void *id);


/**
 *  \brief Main thread.
 *
 *  Its role is store the filenames in the shared region; starting the simulation by generating the intervening workers and
 *  waiting for their termination; and also check and show the results;
 * 
 */

int main(int argc, char *argv[]){

    int nThreads = atoi(argv[1]);                          /* number of workers to create */

    char *filenames[argc-2];                         /* file names */

    for(int i=0; i<argc; i++) filenames[i] = argv[i+2];

    double tStart = ((double) clock ()) / CLOCKS_PER_SEC;                                  /* start counting time of execution */

    pthread_t tIdWork[nThreads];                            /* workers internal thread id array */                           
    unsigned int work[nThreads];                      /* workers application defined thread id array */
    int *status_p;                                  /* pointer to execution status */

    int t;

    for (int t=0; t<nThreads; t++) work[t] = t;   

    storeFileNames(argc-2, filenames); 

    printf("File names stored in the shared region.\n");
                           
    for(t=0; t<nThreads; t++) {
        if (pthread_create (&tIdWork[t], NULL, worker, &work[t]) != 0)                              /* create thread worker */
        { perror ("error on creating thread producer");
            exit (EXIT_FAILURE);
        }
    }

    printf("Workers created and start processing.\n");

    for(t=0; t<nThreads; t++)   { 
        if (pthread_join (tIdWork[t], (void *) &status_p) != 0)                                  /* wait for thread worker to terminate */
        { perror ("error on waiting for thread producer");
            exit (EXIT_FAILURE);
        }
        printf ("thread worker, with id %u, has terminated. \n", t);
    }

    printf("Terminated.\n");

    double tStop = ((double) clock ()) / CLOCKS_PER_SEC;                           /* end of execution time */
    printf ("\nElapsed time = %.6f s\n", tStop - tStart);

    checkProcessingResults();            /* check processed results and print the incorrect ones ( if any ) */

    return 0; 
}


/**
 *  \brief Function computeValue.
 *
 *  Formula to calculate the circular cross-correlation of a point
 *
 */

double computeValue(int nElements, double X[], double Y[], int point) {

    double value = 0.0;               /* will be the result for this point */
    
    for (int k = 0; k<nElements; k++) {
        value = value + (X[k] * Y[(point+k) % nElements]);              /* formula */
    }
    
    return value;
}


/**
 *  \brief Function worker.
 *
 *  Its role is to iterate by the signals and process the circular cross-correlation 
 *
 */

static void *worker (void *par) {

    unsigned int id = *((unsigned int *) par);              /* worker id */

    int nElements;           /* lenght of signals */

    int fileID;           /* current file id being processed */

    int point;              /* current point of processing */

    double *elementsX;             /* signal X */
    double *elementsY;             /* signal Y */

    while (processConvPoint (id, &fileID, &nElements, &elementsX, &elementsY, &point) != 1) {     /* while data available */
        double xy = computeValue(nElements, elementsX, elementsY, point);
        savePartialResult(id, fileID, point, xy);             /* save in shared region */
    }

    int status = EXIT_SUCCESS;
    pthread_exit(&status);

}





        