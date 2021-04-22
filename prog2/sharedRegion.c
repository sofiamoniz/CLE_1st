/**
 *  \file sharedRegion.c
 *
 *  \brief Problem: Assignment 1 - Multithreading (circular cross-correlation)
 *
 *  Synchronization based on monitors (Lampson / Redell)  - SHARED REGION (accessed by main thread and one or multiple workers)
 *
 *  Definition of the operations carried out by the main thread:
 *     \li storeFileNames.
 * 
 *  Definition of the operations carried out by the producers / consumers:
 *     \li processConvPoint
 *     \li savePartialResult
 *     \li checkProcessingResults
 * 
 * 
 *
 *  \author Alina Yanchuk e Sofia Moniz
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

/** \brief file names storage region */
static char ** fileNamesRegion;

/** \brief total number of files to process */
static int nFiles;

/** \brief file being currently processed */
int fileCurrentlyProcessed = 0;

/** \brief variable to control the first time of processing each file */
bool firstProcessing = true;

/** \brief struct to store data to process signal of one file*/
typedef struct {
   int  fileId;    /* file with data */  
   int signalLenght;  /* length of signals */  
   double *x;   /* signal X */          
   double  *y;   /* signal Y */ 
   double *xy;   /* will store the calculated signal XY */ 
   double *xyCorrect;   /* correct signal XY */ 
   int point;   /* point of processing */ 
   bool done;   /* to control the end of processing */ 
} Signal;

/** \brief all signals */
static Signal * signals; 

/** \brief locking flag which warrants mutual exclusion inside the monitor */
static pthread_mutex_t accessCR = PTHREAD_MUTEX_INITIALIZER;


/**
 *  \brief Store the filenames in the file names region.
 *
 *  Operation carried out by main thread.
 */

void storeFileNames(int nFileNames, char *fileNames[]) {
  
    if ((pthread_mutex_lock (&accessCR)) != 0) {                             /* enter monitor */                       
       perror ("error on entering monitor(CF)");                            /* save error in errno */
       int status = EXIT_FAILURE;
       pthread_exit(&status);
    }
    
    nFiles = nFileNames;                     /* number of files */

    signals = (Signal*)malloc(sizeof(Signal) * nFiles);   /* memory allocation for the signals*/

    fileNamesRegion = malloc(nFiles * sizeof(char*));   /* memory allocation for the region storing the filenames*/

    for (int i=0; i<nFileNames; i++) {
        fileNamesRegion[i] = malloc((12) * sizeof(char));    /* memory allocation for the filenames*/
        strcpy(fileNamesRegion[i], fileNames[i]);
        signals[i].done = false;                            /* initialization variable done of each signal  */
    }

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                   /* exit monitor */                                                
       perror ("error on exiting monitor(CF)");                     /* save error in errno */
       int status = EXIT_FAILURE;
       pthread_exit(&status);
    }

}


/**
 *  \brief Process one convolution point of the current file being processed
 *
 *  Operation carried out by workers.
 */

int processConvPoint(int threadId, int *fileId, int *n, double **x, double **y, int *point) {

    if ((pthread_mutex_lock (&accessCR)) != 0) {                     /* enter monitor */        
        perror ("error on entering monitor(CF)");                   /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    int t;

    if (signals[fileCurrentlyProcessed].done == true) {  /* if no more data to process in current file */  
        
        if (fileCurrentlyProcessed == nFiles - 1) {       /* if current file is the last file to be processed */
            if ((pthread_mutex_unlock (&accessCR)) != 0) {              /* exit monitor */                                         
                perror ("error on exiting monitor(CF)");                /* save error in errno */
                int status = EXIT_FAILURE;                              
                pthread_exit(&status);
            }
            return 1;                                                      /* end */
        }
        
        fileCurrentlyProcessed++;       /* next file to process */
        firstProcessing = true;
    }  

    if (firstProcessing == true) {               /* if first time processing the current file */

        FILE *fp;                                                     /* file to process */
        fp = fopen(fileNamesRegion[fileCurrentlyProcessed], "rb");   
        if (fp == NULL) { 
            printf("Cannot open file \n"); 
            exit(0); 
        } 

        signals[fileCurrentlyProcessed].fileId = fileCurrentlyProcessed;

        int nElements;                                   /* length of the signals */
        fread(&nElements, sizeof(int), 1, fp);

        signals[fileCurrentlyProcessed].signalLenght = nElements;
        signals[fileCurrentlyProcessed].point = 0;
        signals[fileCurrentlyProcessed].x = (double*)malloc(sizeof(double) * nElements);
        signals[fileCurrentlyProcessed].y = (double*)malloc(sizeof(double) * nElements);
        signals[fileCurrentlyProcessed].xy = (double*)malloc(sizeof(double) * nElements);
        signals[fileCurrentlyProcessed].xyCorrect = (double*)malloc(sizeof(double) * nElements);
        
        double elementsX[nElements];                                           /* signal X */
        fread(elementsX, sizeof (double), nElements, fp);
        for(t=0; t<nElements; t++) signals[fileCurrentlyProcessed].x[t] = elementsX[t];

        double elementsY[nElements];                                  /* signal Y */
        fread(elementsY, sizeof (double), nElements, fp);
        for(t=0; t<nElements; t++) signals[fileCurrentlyProcessed].y[t] = elementsY[t];
        
        fclose(fp);

        FILE *fpResult;                                  /* file with correct result */
        char fileResult[16];
        sprintf(fileResult, "newSigVal0%c.bin", fileNamesRegion[fileCurrentlyProcessed][7]);
        fpResult= fopen(fileResult, "rb");
        if (fpResult == NULL) { 
            printf("Error %s",fileResult);
            exit(0); 
        } 

        fseek(fpResult, sizeof (int) + 2 * (nElements * sizeof (double)), SEEK_SET );  /* fg to where XY is stored */
        
        double elementsXY[nElements];                                  /* correct signal XY */
        fread(elementsXY, sizeof (double), nElements, fp);
        for(t=0; t<nElements; t++) signals[fileCurrentlyProcessed].xyCorrect[t] = elementsXY[t];
        
        fclose(fpResult);

        firstProcessing = false;
    }

 
    *point = signals[fileCurrentlyProcessed].point;  
    *n = signals[fileCurrentlyProcessed].signalLenght; 
    *fileId = fileCurrentlyProcessed;

    //printf("worker: %d, point: %d\n", threadId, signals[fileCurrentlyProcessed].point);
    
    *x = signals[fileCurrentlyProcessed].x;
    *y = signals[fileCurrentlyProcessed].y;

    signals[fileCurrentlyProcessed].point++;                                 /* increment point of process */

    if (signals[fileCurrentlyProcessed].point == signals[fileCurrentlyProcessed].signalLenght) signals[fileCurrentlyProcessed].done = true; /* if last point of process */

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                  /* exit */    
        perror ("error on exiting monitor(CF)");                                        /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    return 0;
}


/**
 *  \brief Save the convolution point that was processed
 *
 *  Operation carried out by workers.
 */

void savePartialResult(int threadId, int fileId, int point, double val) {

    if ((pthread_mutex_lock (&accessCR)) != 0) {                     /* enter monitor */        
        perror ("error on entering monitor(CF)");                   /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    signals[fileId].xy[point] = val;           /* store value */

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                  /* exit */    
        perror ("error on exiting monitor(CF)");                                        /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

}


/**
 *  \brief check if all the convolution points processed were correctly computed
 *
 *  Operation carried out by main thread.
 */

void checkProcessingResults() {

    if ((pthread_mutex_lock (&accessCR)) != 0) {                     /* enter monitor */        
        perror ("error on entering monitor(CF)");                   /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    for(int i=0; i<nFiles; i++) {   /* check results for all files */
        for(int t=0; t<signals[i].signalLenght; t++) {
            if(signals[i].xy[t]!=signals[i].xyCorrect[t]) printf("Incorrect: %f\n",signals[i].xyCorrect[t]);  /* confirm if calculated value is correct */
        }
    }

    free(signals);                    /* free allocated memory */
    free(fileNamesRegion);            /* free allocated memory */

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                  /* exit */    
        perror ("error on exiting monitor(CF)");                                        /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

}
