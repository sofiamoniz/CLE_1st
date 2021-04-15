#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
//correr 
//gcc -o count count.c  
//./count text0.txt text1.txt text2.txt text3.txt text4.txt

int main(int argc, char *argv[])
{
   
   char word[50];
    

    //printf("Enter file name:\n");
    //scanf("%s", in_name);
    for(int i=1; i<argc; i++){
        char *file = argv[i];
        FILE *f = fopen(file, "r");
        int c;
        if (f == NULL)
            printf("Can't open %s for reading.\n", argv[i]);
        else{
            while((c = fgetc(f)) != EOF) {
            printf("\n(%c)", c);
            int char_bytes = read_char_utf8(c);
            printf(" \char_bytes: %d", char_bytes);
        }
        }
        
        fclose (f);
    }

    /*
    for(int i=1; i<argc; i++){
        char *file = argv[i];
        FILE *f = fopen(file, "r");
        unsigned char *c_arr;
        int status_length = read_char_utf8(f,&c_arr);
        printf(" status_length: %d", status_length);

        
        
        
        
    }
    */
    //unsigned char c = '–';
    //char *pChar = malloc(sizeof(char));

    /* check pChar is not null */

    //*pChar = c;
    int len = get_size("á");
    printf(" what: %d", len);


    return 0;
}