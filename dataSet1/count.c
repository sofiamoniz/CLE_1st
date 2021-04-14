#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc, char *argv[])
{
    FILE *in_file;
    char word[50];
    int n_chars = 0 ;
    int n_consonants = 0;
    int n_words = 0;
    int max_len = 50 ; //max len of word is 50
    char largest_word [max_len];
    int in_word = 0;
    //int len = get_size("á");
   
    

    //printf("Enter file name:\n");
    //scanf("%s", in_name);
    for(int i=1; i<argc; i++){
        in_file = fopen(argv[i], "r");

        if (in_file == NULL)
            printf("Can't open %s for reading.\n", in_file);
        else
        {
            while (fscanf(in_file, "%s", word) != EOF)
            {
                //int len = 0;
                //printf("%s\n", word);
                //++wrd;
                for (int i = 0; word[i] != '\0';++i){
                    int len = get_size(word[i]);
                }

            }
        }
    }
    return 0;
}