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
        //array for countings - max size of word is 50
        int **counting_array = (int **)calloc(50, sizeof(int *));
		for (int j = 0; j<50; j++){
			counting_array[j] = (int *)calloc(j+2, sizeof(int));
		}
        //char converted_char;
        setlocale(LC_CTYPE, "");
        FILE *f = fopen(argv[i], "r");
        if (!f){
            printf("Can't open %s for reading.\n", argv[i]);
            return 1;}
        for (wchar_t c; (c = fgetwc(f)) != WEOF;){
            printf("%d\n", c);
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
                    wprintf(L"Cannot identify %lc\n", c);
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
                    counting_array[n_chars-1][n_consonants]++;
                    if(n_chars > max_chars){
                        max_chars = n_chars;
                    }
                    n_chars = 0;
                    n_consonants = 0;
                }
                else{
                    wprintf(L"Cannot identify %lc\n", c);
                }
            }
        }
        fclose(f);  
        printf("Total words: %d\n", n_words);

		printf("   ");
		for(int j = 0; j<max_chars; j++){
			printf("%5d ", j+1);
		}
		printf("\n");

        //Print  number words each word length
		printf("   ");
		int *soma = (int *)calloc(max_chars, sizeof(int));
		int tmp = 0;
		for(int j = 0; j<max_chars; j++){
			int ind_sum = 0;
			for(int k = 0; k<j+2; k++){
				ind_sum = ind_sum + counting_array[j][k];
			}
			tmp = tmp + ind_sum;
			soma[j] = ind_sum;
			printf("%5d ", soma[j]);
		}
		printf("\n");


		//FINAL PRINT
		printf("   ");
		for(int j = 0; j<max_chars; j++){
			double s = (double)soma[j];
			double st = (double)tmp;
			double r = (double)(s/st*100);
			printf("%5.2f ", r);
		}
		printf("\n");

	
		for(int j = 0; j<max_chars+1; j++){ 
			printf("%2d ", j);
			for(int k = 0; k<max_chars; k++){ 
				if(k<j-1){
					printf("      ");
				}
				else if(soma[k]==0){ 
					double r = 0;
					printf("%5.1f ", r);
				}
				else{
					double cell = (double)counting_array[k][j];
					double s = (double)soma[k];
					double r = (double)(cell/s*100);
					printf("%5.1f ", r);
				}
			}
			printf("\n");
		}
		printf("\n");
        
        
    }

    return 0;

}


    