/**
 *  \file main.c
 *
 *  \brief Problem: Assignment 1 - Multithreading (counting problem)
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
#include "functions.h"

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

    printProcessingResults();    /* print results */

    printf("Terminated.\n");
    
    double tStop = ((double) clock ()) / CLOCKS_PER_SEC;                           /* end of execution time */
    printf ("\nElapsed time = %.6f s\n", tStop - tStart);

    return 0; 
}


/**
 *  \brief Function processDataChunk.
 *
 *  Processing of a data chunk. Precisa de comentários. 
 *
 */

void processDataChunk(char buff[12], PARTFILEINFO *partialInfo) {
    //first, we do the conversion - if char is not
    //multibyte, it will remain unibyte
    //char converted_char = convert_multibyte(c);
            //printf("  %c", new_char);
    for(int i =0; i<12; i++){
        char converted_char = buff[i];
        //printf("(%c)",converted_char);
        if(!(*partialInfo).in_word){
            if(is_alpha_underscore(converted_char)){
                (*partialInfo).in_word = 1;
                (*partialInfo).n_words++;
                (*partialInfo).n_chars++;
                (*partialInfo).n_consonants = (*partialInfo).n_consonants + !is_vowel(converted_char);
            }
            else if(is_apostrophe(converted_char) || is_space_separation_punctuation(converted_char)){
                return;
            }
        }
        else{
            if(is_alpha_underscore(converted_char)){
                (*partialInfo).n_chars++;
                (*partialInfo).n_consonants = (*partialInfo).n_consonants + !is_vowel(converted_char);
            }
            else if(is_apostrophe(converted_char)){
                return;
            }
            else if(is_space_separation_punctuation(converted_char)){
                (*partialInfo).in_word = 0;
                (*partialInfo).counting_array[(*partialInfo).n_chars-1][(*partialInfo).n_consonants]++;
                if((*partialInfo).n_chars > (*partialInfo).max_chars){
                    (*partialInfo).max_chars = (*partialInfo).n_chars;
                }
                (*partialInfo).n_chars = 0;
                (*partialInfo).n_consonants = 0;
            }
        }
    }
    

}


/**
 *  \brief Function worker.
 *
 *  Its role is to obtain data chunks, process them and store the results in shared region. 
 *
 */

static void *worker (void *par) {

    unsigned int id = *((unsigned int *) par);              /* worker id */

    char buff[12];      /* agr é um char mas tem de ser um buffer de carateres */

    PARTFILEINFO partialInfo;     /* struct to store partial info of current file being processed */

    while (getDataChunk(id, buff, &partialInfo) != 1) {        /* while data available */ 
        //wchar_t c = buff;
        processDataChunk(buff, &partialInfo);        /* process current data*/
        savePartialResults(id, &partialInfo);         /* save in shared region */ 
    }
    
    int status = EXIT_SUCCESS;
    pthread_exit(&status);

}



