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

int main(int argc, char *argv[]){

    int nThreads = atoi(argv[1]);

    char *filenames[argc-2];

    for(int i=0; i<argc; i++) filenames[i] = argv[i+2];

    double tStart = ((double) clock ()) / CLOCKS_PER_SEC;

    pthread_t tIdWork[nThreads];                                                       
    unsigned int work[nThreads]; 
    int *status_p;

    for (int t=0; t<nThreads; t++) work[t] = t;   

    storeFileNames(argc-2, filenames); 
                           

    for(int t=0; t<nThreads; t++) {
        if (pthread_create (&tIdWork[t], NULL, worker, &work[t]) != 0)                              /* thread worker */
        { perror ("error on creating thread producer");
            exit (EXIT_FAILURE);
        }
    }


    for(int t=0; t<nThreads; t++)   { 
        if (pthread_join (tIdWork[t], (void *) &status_p) != 0)                                       
        { perror ("error on waiting for thread producer");
            exit (EXIT_FAILURE);
        }
    printf ("thread worker, with id %u, has terminated: ", t);
    printf ("its status was %d\n", *status_p);
    }

    double tStop = ((double) clock ()) / CLOCKS_PER_SEC;
    printf ("\nElapsed time = %.6f s\n", tStop - tStart);

    checkProcessingResults();

    return 0; 
}



double computeValue(int nElements, double X[], double Y[], int point) {
    double value = 0.0;
    for (int k = 0; k<nElements; k++) {
        value = value + (X[k] * Y[(point+k) % nElements]); 
    }
    return value;
}


static void *worker (void *par) {

    unsigned int id = *((unsigned int *) par); 

    int nElements;

    int fileID;

    int point;

    double *elementsX;
    double *elementsY;


    while (processConvPoint (id, &fileID, &nElements, &elementsX, &elementsY, &point) != 1) {
        double xy = computeValue(nElements, elementsX, elementsY, point);
        //printf("%f\n",xy);
        savePartialResult(id, fileID, point, xy);
    }

    int status = EXIT_SUCCESS;
    pthread_exit(&status);
}






        