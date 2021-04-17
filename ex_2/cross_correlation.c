#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(int agrc, char *argv[]) {

    for(int i=1; i<agrc; i++) {
    
        FILE *fp;

        fp = fopen(argv[i], "rb");

        if (fp == NULL) { 
            printf("Cannot open file \n"); 
            exit(0); 
        } 

        int nElements;
        fread(&nElements, sizeof(int), 1, fp);

        double elementsX[nElements];
        fread(elementsX, sizeof (double), nElements, fp);

        double elementsY[nElements];
        fread(elementsY, sizeof (double), nElements, fp);

        double elementsXY[nElements];
        double xy;

        for (int j = 0; j<nElements; j++) {
            xy = 0.0;
            for (int k = 0; k<nElements; k++) {
                xy = xy + (elementsX[k] * elementsY[(j+k) % nElements]); 
            }
            elementsXY[j] = xy;
        }

        fclose(fp);

        fp = fopen(argv[i], "ab");

        if (fp == NULL) { 
            printf("Cannot open file \n"); 
            exit(0); 
        } 

        fwrite(elementsXY, sizeof(elementsXY),1, fp);
        
    }
        return 0; 
}
