#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

/** \brief storage region */
char *fileNamesRegion[3];

double results[3][1024];

int fileCurrentlyProcessed = 0;


typedef struct {
   int  fileId;
   int signalSize;
   double *x;
   double  *y;
   double *xy;
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

    signals = (Signal*)malloc(sizeof(Signal) * nFileNames);
    
    for (int i=0; i<nFileNames; i++) {

        fileNamesRegion[i] = fileNames[i];
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

        

        double elementsX[nElements];
        fread(elementsX, sizeof (double), nElements, fp);

        double elementsY[nElements];
        fread(elementsY, sizeof (double), nElements, fp);



        signals[i].signalSize = nElements;
        


        for(int t=0;t<nElements;t++) signals[i].x[t] = elementsX[t];



        for(int t=0;t<nElements;t++) signals[i].y[t] = elementsY[t];
        

        signals[i].point = 0;

        fclose(fp);

        
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

int processConvPoint(int threadId, int *fileId, int *n, double *x, double *y, int *point) {

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
    


    //*fileId = fileCurrentlyProcessed;


    int nFiles = 2;

    

    *point = signals[fileCurrentlyProcessed].point;

    *n = signals[fileCurrentlyProcessed].signalSize;
    
    for(int t=0;t<*n;t++) x[t] = signals[fileCurrentlyProcessed].x[t];
    for(int t=0;t<*n;t++) y[t] = signals[fileCurrentlyProcessed].y[t];

    signals[fileCurrentlyProcessed].point = signals[fileCurrentlyProcessed].point + 1;

    if (signals[fileCurrentlyProcessed].point == *n+1) {
        
         if (fileCurrentlyProcessed == nFiles - 1) return 1;
         else {
             fileCurrentlyProcessed++;
         }
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

    results[fileId][point] = val;

    FILE *fp;
    fp = fopen(fileNamesRegion[fileId], "ab");
    if (fp == NULL) { 
        printf("Cannot open file \n"); 
        exit(0); 
    } 
    fwrite((const void*) & val,sizeof(double),1,fp);

    fclose(fp);

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                           
        perror ("error on exiting monitor(CF)");
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    
}
