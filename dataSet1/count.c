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
        int n_words = 0;
        int n_chars = 0;
        int n_consonants = 0;
        int in_word = 0;
        int max_size = 50;
        int max_chars = 0;
        int n_unrecognized_char = 0;
        //char converted_char;
        setlocale(LC_CTYPE, "");
        FILE *f = fopen(argv[i], "r");
        if (!f)
            return 1;
        for (wchar_t c; (c = fgetwc(f)) != WEOF;){
            //first, we do the conversion - if char is not
            //multibyte, it will remain unibyte
            char converted_char = convert_multibyte(c);
            //printf("  %c", new_char);
           
            if(!in_word){
                if(is_alpha_underscore(converted_char)){
                    in_word = 1;
                    n_words++;
                    n_chars++;
                    n_consonants = n_consonants + !is_vowel(converted_char);
                }
                else if(is_apostrophe(converted_char) || is_space_separation_punctuation(converted_char)){
                    continue;
                }
                else{
                    n_unrecognized_char++;
                    wprintf(L"Cannot identify %lc\n", c);
                    //printf("Cannot identify %c\n)", converted_char);
                }
            }
            else{
                if(is_alpha_underscore(converted_char)){
                    n_chars++;
                    n_consonants = n_consonants + !is_vowel(converted_char);
                }
                else if(is_apostrophe(converted_char)){
                    continue;
                }
                else if(is_space_separation_punctuation(converted_char)){
                    in_word = 0;
                    if(n_chars > max_chars){
                        max_chars = n_chars;
                    }
                    n_chars = 0;
                    n_consonants = 0;
                }
                else{
                    n_unrecognized_char++;
                    wprintf(L"Cannot identify %lc\n", c);
                    //printf("Cannot identify %c\n", converted_char);
                }
            }
        }
        //printf("acabou");
        fclose(f);  
        printf("Total number of words: %d\n", n_words);
        if(n_unrecognized_char){
			printf("%d unrecognized characters found.\n", n_unrecognized_char);
		}
        
        
    }

    return 0;

}


    