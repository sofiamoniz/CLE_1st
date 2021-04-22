/**
 *  \file sharedRegion.c
 *
 *  \brief Problem: Assignment 1 - Multithreading (counting problem)
 *
 *  Synchronization based on monitors (Lampson / Redell)  - SHARED REGION (accessed by main thread and one or multiple workers)
 *
 *  Definition of the operations carried out by the main thread:
 *     \li storeFileNames.
 *     \li printProcessingResults
 * 
 *  Definition of the operations carried out by the producers / consumers:
 *     \li processConvPoint
 *     \li getDataChunk
 *     \li savePartialResults
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
#include <wchar.h>
#include <locale.h>

/** \brief file names storage region */
static char ** fileNamesRegion;

/** \brief total number of files to process */
static int nFiles;

/** \brief variable to control the first time of processing each file */
bool firstProcessing = true;

/** \brief file being currently processed */
int fileCurrentlyProcessed = 0;


/** \brief struct to store data of one file*/
typedef struct {
   int  fileId;    /* file with data */  
   int n_words;    /* number words */
   int n_chars;
   int n_consonants;
   int in_word;
   int max_size;
   int max_chars;
   int **counting_array;
   bool done;        /* to control the end of processing */ 
} PARTFILEINFO;

/** \brief to control the position of file reading */
static long pos;

/** \brief all partial file infos */
static PARTFILEINFO * partfileinfos;

/** \brief locking flag which warrants mutual exclusion inside the monitor */
static pthread_mutex_t accessCR = PTHREAD_MUTEX_INITIALIZER;

/** \brief workers synchronization point when previously processed partial info was stored in region */
static pthread_cond_t store;

/** \brief workers synchronization point when next chunk was obtained and processed */
static pthread_cond_t process;

/** \brief flag signaling the previously processed partial info was stored */
static bool stored;

/** \brief flag signaling the next chunk was obtained/processed */
static bool processed;

/** \brief flag which warrants that the data transfer region is initialized exactly once */
static pthread_once_t init = PTHREAD_ONCE_INIT;


/**
 *  \brief Initialization of the data transfer region.
 *
 *  Internal monitor operation.
 */

static void initialization (void) {
    
    processed = false;                                                         /* next chunk was not processed/obtained */
    stored = false;                                                           /* previous partial info was not stored */
    pthread_cond_init (&process, NULL);                                 /* initialize workers synchronization point */
    pthread_cond_init (&store, NULL);                                 /* initialize workers synchronization point */
    setlocale(LC_CTYPE, "");    

}

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

    fileNamesRegion = malloc(nFiles * sizeof(char*));   /* memory allocation for the region storing the filenames*/

    partfileinfos = (PARTFILEINFO*)malloc(sizeof(PARTFILEINFO) * nFiles);   /* memory allocation for the partial infos per file*/

    for (int i=0; i<nFileNames; i++) {
        fileNamesRegion[i] = malloc((12) * sizeof(char));       /* memory allocation for the filenames*/
        strcpy(fileNamesRegion[i], fileNames[i]);
        partfileinfos[i].done = false;                         /* initialization variable done of each partial info */
    }

    pthread_once (&init, initialization);    

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                   /* exit monitor */                                                
       perror ("error on exiting monitor(CF)");                     /* save error in errno */
       int status = EXIT_FAILURE;
       pthread_exit(&status);
    }

}


/**
 *  \brief Obtain next data chunk (currently just one char) of the current file being processed
 *
 *  Operation carried out by workers.
 */

int getDataChunk(int threadId, wchar_t **buf, PARTFILEINFO *partialInfo) {

    if ((pthread_mutex_lock (&accessCR)) != 0) {                     /* enter monitor */        
        perror ("error on entering monitor(CF)");                   /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    if (firstProcessing == false) {                                  /* no need to wait in the first processing as there is no values to be stored */
        while (stored==false) {                                       /* wait if the previous partial info was no stored */
            if ((pthread_cond_wait (&store, &accessCR)) != 0) {                                                       
                perror ("error on waiting in fifoEmpty");                 /* save error in errno */
                int status = EXIT_FAILURE;
                pthread_exit (&status);
            }
        }
    }

    if (partfileinfos[fileCurrentlyProcessed].done == true) {     /* if no more data to process in current file */  
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
        partfileinfos[fileCurrentlyProcessed].fileId = fileCurrentlyProcessed; /* initialize variables */
        partfileinfos[fileCurrentlyProcessed].n_words = 0;
        partfileinfos[fileCurrentlyProcessed].n_chars = 0;
        partfileinfos[fileCurrentlyProcessed].n_consonants = 0;
        partfileinfos[fileCurrentlyProcessed].in_word = 0;
        partfileinfos[fileCurrentlyProcessed].max_size = 50;
        partfileinfos[fileCurrentlyProcessed].max_chars = 0;
        partfileinfos[fileCurrentlyProcessed].counting_array = (int **)calloc(50, sizeof(int *));
		for (int j = 0; j<50; j++){
			partfileinfos[fileCurrentlyProcessed].counting_array[j] = (int *)calloc(j+2, sizeof(int));
		}
    }

    FILE *f = fopen(fileNamesRegion[fileCurrentlyProcessed], "r");

    if (firstProcessing==false) fseek(f, pos, SEEK_SET );  /* go to position where stopped read last time */
    if (firstProcessing==true) firstProcessing = false;
    
    wchar_t c;
    c = fgetwc(f);    /* get next char */
    pos = ftell(f);   /* current position of file reading */

    fclose(f);

    *buf = c;  /* o buffer supostamente tem de ser um array de carateres mas eu so passei 1 aqui , temos de mudar */

    if ( c == WEOF)  { /* if last character of current file */
        partfileinfos[fileCurrentlyProcessed].done = true;   /* done processing current file */
    }

    *partialInfo = partfileinfos[fileCurrentlyProcessed];

    processed = true;     /* obtained chunk */
    if ((pthread_cond_signal (&process)) != 0) {      /* let a worker know that the next chunk has been obtainedd */                                                                                                                         
        perror ("error on waiting in fifoEmpty");                 /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit (&status);
    }
    stored = false; /* this chunk was not stored yet */

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                  /* exit */    
        perror ("error on exiting monitor(CF)");                                        /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    return 0;
}


/**
 *  \brief Save the partial results last processed.
 *
 *  Operation carried out by workers.
 */

void savePartialResults(int threadId, PARTFILEINFO *partialInfo) {

    if ((pthread_mutex_lock (&accessCR)) != 0) {                     /* enter monitor */        
        perror ("error on entering monitor(CF)");                   /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    while (processed == false) {                                               /* wait if the next chunk was not obtained/processed */
        if ((pthread_cond_wait (&process, &accessCR)) != 0) {
            perror ("error on waiting in fifoEmpty");                  /* save error in errno */
            int status = EXIT_FAILURE;
            pthread_exit (&status);
        }
    }

    partfileinfos[fileCurrentlyProcessed] = *partialInfo;                   /* save new partial info */
    
    stored = true;                                /* new partial info saved */
    if ((pthread_cond_signal (&store)) != 0) {
        perror ("error on waiting in fifoEmpty");                  /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit (&status);
    }
    processed = false;                   /* next chunk was not processed yet */
     
    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                  /* exit */    
        perror ("error on exiting monitor(CF)");                                        /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

}


/**
 *  \brief Print all final results.
 *
 *  Operation carried out by main thread.
 */

void printProcessingResults() {

    if ((pthread_mutex_lock (&accessCR)) != 0) {                     /* enter monitor */        
        perror ("error on entering monitor(CF)");                   /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

    for (int i=0; i<nFiles; i++) {                  /* each partial file info */

        printf("\nFile name: %s\n", fileNamesRegion[i]);

        printf("Total number of words = %d\n", partfileinfos[i].n_words);

        printf("Word lenght\n");

		printf("   ");
		for(int j = 0; j<partfileinfos[i].max_chars; j++){
			printf("%5d ", j+1);
		}
		printf("\n");

        //Print  number words each word length
		printf("   ");
		int *soma = (int *)calloc(partfileinfos[i].max_chars, sizeof(int));
		int tmp = 0;
		for(int j = 0; j<partfileinfos[i].max_chars; j++){
			int ind_sum = 0;
			for(int k = 0; k<j+2; k++){
				ind_sum = ind_sum + partfileinfos[i].counting_array[j][k];
			}
			tmp = tmp + ind_sum;
			soma[j] = ind_sum;
			printf("%5d ", soma[j]);
		}
		printf("\n");


		//FINAL PRINT
		printf("   ");
		for(int j = 0; j<partfileinfos[i].max_chars; j++){
			double s = (double)soma[j];
			double st = (double)tmp;
			double r = (double)(s/st*100);
			printf("%5.2f ", r);
		}
		printf("\n");

	
		for(int j = 0; j<partfileinfos[i].max_chars+1; j++){ 
			printf("%2d ", j);
			for(int k = 0; k<partfileinfos[i].max_chars; k++){ 
				if(k<j-1){
					printf("      ");
				}
				else if(soma[k]==0){ 
					double r = 0;
					printf("%5.1f ", r);
				}
				else{
					double cell = (double)partfileinfos[i].counting_array[k][j];
					double s = (double)soma[k];
					double r = (double)(cell/s*100);
					printf("%5.1f ", r);
				}
			}
			printf("\n");
		}
		printf("\n");
    }

    if ((pthread_mutex_unlock (&accessCR)) != 0) {                                                  /* exit */    
        perror ("error on exiting monitor(CF)");                                        /* save error in errno */
        int status = EXIT_FAILURE;
        pthread_exit(&status);
    }

}



