#ifndef SHAREDREGION_H
#define SHAREDREGION_H

extern char *fileNamesRegion[3];

void storeFileNames (int nFileNames, char *fileNames[]);

int processConvPoint(int threadId, int *fileId, int *n, double *x,double *y, int *point);

void savePartialResult (int threadId, int fileId, int point, double val);

#endif