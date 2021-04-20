#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

/** \brief storage region */
static char ** fileNamesRegion;

int fileCurrentlyProcessed = 0;

static int nFiles;


typedef struct {
   int  fileId;
   int signalSize;
   double *x;
   double  *y;
   double *xy;
   double *xyCorrect;
   int point;
} Signal;

static Signal * signals; 

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

static void initialization (void) {                                                                            
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

    nFiles = nFileNames;

    signals = (Signal*)malloc(sizeof(Signal) * nFileNames);

    fileNamesRegion = malloc(nFileNames * sizeof(char*));

    for (int i=0; i<nFileNames; i++) {

        fileNamesRegion[i] = malloc((12) * sizeof(char));
        strcpy(fileNamesRegion[i], fileNames[i]);
        signals[i].fileId = i;

        FILE *fp;

        fp = fopen(fileNames[i], "rb");

        

        if (fp == NULL) { 
            printf("Cannot open file \n"); 
            exit(0); 
        } 

        int nElements;
        fread(&nElements, sizeof(int), 1, fp);

        signals[i].x = (double*)malloc(sizeof(double) * nElements);
        signals[i].y = (double*)malloc(sizeof(double) * nElements);
        signals[i].xy = (double*)malloc(sizeof(double) * nElements);
        signals[i].xyCorrect = (double*)malloc(sizeof(double) * nElements);

    
        double elementsX[nElements];
        fread(elementsX, sizeof (double), nElements, fp);

        double elementsY[nElements];
        fread(elementsY, sizeof (double), nElements, fp);



        signals[i].signalSize = nElements;
        


        for(int t=0;t<nElements;t++) signals[i].x[t] = elementsX[t];



        for(int t=0;t<nElements;t++) signals[i].y[t] = elementsY[t];
        

        signals[i].point = 0;

        fclose(fp);

        FILE *fp2;

        char name[16];
        sprintf(name, "newSigVal0%c.bin", fileNames[i][7]);
;
        fp2 = fopen(name, "rb");

        if (fp2 == NULL) { 
            printf("Error %s",name);
            exit(0); 
        } 
        int n;
        fread(&n, sizeof(int), 1, fp);
        double X[n];
        fread(X, sizeof (double), n, fp);
        double Y[n];
        fread(Y, sizeof (double), n, fp);
        double XY[n];
        fread(XY, sizeof (double), n, fp);

        for(int t=0;t<nElements;t++) signals[i].xyCorrect[t] = XY[t];

        fclose(fp2);
   
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

    //free(signals);
    //free(signals[0].x);

    

    printf("Done storing files.\n");

    
}

int processConvPoint(int threadId, int *fileId, int *n, double **x, double **y, int *point) {


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

    if (signals[fileCurrentlyProcessed].point == signals[fileCurrentlyProcessed].signalSize) {
        
        if (fileCurrentlyProcessed == nFiles - 1) {
            if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                           
                perror ("error on exiting monitor(CF)");
                int status = EXIT_FAILURE;
                pthread_exit(&status);
            }
            return 1;
        }
        else {
            fileCurrentlyProcessed++;
            
        }
    }   

    double* tmpX = calloc(signals[fileCurrentlyProcessed].signalSize, sizeof(double));
    double* tmpY = calloc(signals[fileCurrentlyProcessed].signalSize, sizeof(double));


    *point = signals[fileCurrentlyProcessed].point;   

    //printf("worker: %d, point: %d\n", threadId, signals[fileCurrentlyProcessed].point);

    *n = signals[fileCurrentlyProcessed].signalSize;


    
    *fileId = fileCurrentlyProcessed;
    
    for(int t=0;t<*n;t++) *(tmpX+t) = signals[fileCurrentlyProcessed].x[t];
    for(int t=0;t<*n;t++) *(tmpY+t) = signals[fileCurrentlyProcessed].y[t];

    signals[fileCurrentlyProcessed].point++;

    free(*x);
    *x = tmpX;

    free(*y);
    *y = tmpY;

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                           
        perror ("error on exiting monitor(CF)");
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    return 0;

}


void savePartialResult (int threadId, int fileId, int point, double val) {

    if ((pthread_mutex_lock (&accessCR)) != 0) {                                                           
        perror ("error on entering monitor(CF)");
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    pthread_once (&init, initialization);                                           

    signals[fileId].xy[point] = val;


    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                           
        perror ("error on exiting monitor(CF)");
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }
    
}

void checkProcessingResults() {

    for(int i=0;i<nFiles;i++) {
        for(int t=0;t<signals[i].signalSize;t++) {
            if(signals[i].xy[t]!=signals[i].xyCorrect[t]) printf("Incorrect: %f\n",signals[i].xyCorrect[t]);
        }
    }

    /**
    FILE *fp;
    for(int i=0;i<nFiles;i++) {
        
        fp = fopen(fileNamesRegion[i], "ab");
        if (fp == NULL) { 
            printf("Cannot open file \n"); 
            exit(0); 
        } 

        for(int t=0;t<signals[i].signalSize;t++) fwrite((const void*) & signals[i].xy[t],sizeof(double),1,fp);

        fclose(fp);
    }
    **/
}
