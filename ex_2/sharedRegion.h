#ifndef SHAREDREGION_H
#define SHAREDREGION_H

extern char *fileNamesRegion[3];

typedef struct {
   int  fileId;
   int signalSize;
   double *x;
   double* y;
   double* xy;
} Signal;

extern Signal * signals; 

extern int fileCurrentlyProcessed;

void storeFileNames (int nFileNames, char *fileNames[]);

int processConvPoint(int threadId, int *fileId, int *n, double *x,double *y, int *point);

void savePartialResult (int threadId, int fileId, int point, double val);

#endif