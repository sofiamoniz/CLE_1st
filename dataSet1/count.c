#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc, char *argv[])
{
    char in_name[80];
    FILE *in_file;
    char word[50];
    int wrd ;
    int consonant = 0;
    int len = get_size("~");
    

    //printf("Enter file name:\n");
    //scanf("%s", in_name);
    for(int i=1; i<argc; i++){
        in_file = fopen(argv[i], "r");

        if (in_file == NULL)
            printf("Can't open %s for reading.\n", in_name);
        else
        {
            while (fscanf(in_file, "%s", word) != EOF)
            {
                //printf("%s\n", word);
                //++wrd;
                for (int i = 0; word[i] != '\0';++i){
                    //printf("(%c)", word[i]);
                    printf("The size is: %d", len);
                    
                    
                    
                }

            }
        }
    }
    return 0;
}