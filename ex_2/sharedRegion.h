/**
 *  \brief Store a value in the data transfer region.
 *
 *  Operations carried out by main thread and workers.
 *
 *  
 */

#ifndef SHAREDREGION_H
#define SHAREDREGION_H

void storeFileNames (int nFileNames, char *fileNames[]);

int processConvPoint(int threadId, int *fileId, int *n, double **x,double **y, int *point);

void savePartialResult (int threadId, int fileId, int point, double val);

void checkProcessingResults();

#endif