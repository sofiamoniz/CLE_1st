/**
 *  \brief Store filenames and get and stored partial infos.
 *
 *  Operations carried out by main thread and workers.
 *
 */

#ifndef SHAREDREGION_H
#define SHAREDREGION_H

typedef struct {
   int  fileId;    
   int n_words;
   int n_chars;
   int n_consonants;
   int in_word;
   int max_size;
   int max_chars;
   int **counting_array;
   bool done;
} PARTFILEINFO;

void storeFileNames (int nFileNames, char *fileNames[]);

int getDataChunk(int threadId,wchar_t **buf, PARTFILEINFO *partialInfo);

void savePartialResults (int threadId, PARTFILEINFO *partialInfo);

void printProcessingResults();

#endif