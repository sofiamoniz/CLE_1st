/**
 *  \file main.c
 *
 *  \brief Problem: Assignment 1 - Multithreading (counting problem)
 *
 *  Synchronization based on monitors (Lampson / Redell)  - Main program
 *
 *  \author Alina Yanchuk e Ana Sofia Moniz Fernandes
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

#include "sharedRegion.h"
#include "main_functions.h"

/* variables to define the size of the buffer */
int MAX_SIZE_WORD = 50;
int MAX_BYTES_TO_READ = 12;

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
 *  Processing of a data chunk. The approach given by the professor was followed:
 *      -If in_word is false:
 *          -if the char is alphanumeric or underscore, in_word is set to true, and we increment the total words, total
 *              num of chars and the number of consonants
 *          -if the char is apostrophe or space/separation/punctiation, in_word remains set to false
 *      -If in_word is true:
 *          -if the char is alpha or underscore, we increment chars and consonants;
 *          -if char is apostrophe, we return;
 *          -if char is space/separation/punctiation, we set in_word to false and update the word couting
 *
 */

void processDataChunk(char *buf, PARTFILEINFO *partialInfo) {
    char converted_char;
    int buf_size = size_of_array(buf);
    for(int i=0; i<buf_size;i++){
        converted_char = buf[i];
    }
           
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


/**
 *  \brief Function worker.
 *
 *  Its role is to obtain data chunks, process them and store the results in shared region. 
 *
 */

static void *worker (void *par) {

    unsigned int id = *((unsigned int *) par);              /* worker id */

    /* buffer has size of MAX_BYTES_TO_READ bytes + MAX_SIZE_WORD -> this way,
    we prevent the case where the last word that was readen is not complete. It will be a set of complete words. */
    char buf[MAX_BYTES_TO_READ+MAX_SIZE_WORD];

    PARTFILEINFO partialInfo;     /* struct to store partial info of current file being processed */

    while (getDataChunk(id, buf, &partialInfo) != 1) {        /* while data available */ 
        processDataChunk(buf, &partialInfo);        /* process current data*/
        savePartialResults(id, &partialInfo);         /* save in shared region */ 
    }
    
    int status = EXIT_SUCCESS;
    pthread_exit(&status);

}



