#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
//correr 
//gcc -o count count.c  
//./count text0.txt text1.txt text2.txt text3.txt text4.txt

int main(int argc, char *argv[])
{
   
    for(int i=1; i<argc; i++){
        char *file = argv[i];
        FILE *in_file = fopen(file, "r");
        int c;
        int n_words = 0;
        int n_chars = 0;
        int n_consonants = 0;
        int in_word = 0;
        int max_size = 50;
        int max_chars = 0;
       
        while(1){
            unsigned char *char_array;
            //Read char
            int char_size = check_char_multibyte(in_file,&char_array);
            //printf("\n(%d)", char_size);
            if(char_size){ //if char is multibyte, convert to singlebyte
                unsigned char new_c = conversor_multibyte(&char_array, char_size);
                printf("\n(%c)", new_c);
                if(in_word){
                    if(is_space_separation_punctuation(new_c)){
                        in_word = 0;
                        n_words ++;
                        //qualquer coisa para largestWord
                    }
                    else if(is_alpha_underscore(new_c)){
                        n_chars ++;

                    }
                
                }


            }   
            else{
                if(in_word){
                    if (n_chars > max_chars){
                        max_chars = n_chars;
                    }
                }
                break;
            }
            printf("\nN_words (%d)", max_chars);
            fclose(in_file);
        }
        
        
    }

    return 0;

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
    //int len = get_size('á');
    //printf(" what: %d", len);


    