#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions_chunks.h"
//correr 
//gcc -o count count.c  
//./count text0.txt text1.txt text2.txt text3.txt text4.txt

int main(int argc, char *argv[])
{
    for(int i=1; i<argc; i++){
        int size_to_read = 0;
        char char_array[11]; //11 é um tamanho qualquer, podemos escolher
        
        //char converted_char;
        setlocale(LC_CTYPE, "");
        FILE *f = fopen(argv[i], "r");
        if (!f){
            printf("Can't open %s for reading.\n", argv[i]);
            return 1;}
        for (wchar_t c; (c = fgetwc(f)) != WEOF;){
            //first, we do the conversion - if char is not
            //multibyte, it will remain unibyte
            //printf("|%d|", size_to_read);
            if(size_to_read<=11){ //11 é um tamanho qualquer, podemos escolher. Pus este pequeno para testar
                char converted_char = convert_multibyte(c);
                //printf("|%c|", converted_char);
                char_array[size_to_read] = converted_char;
                size_to_read++;
                //printf("%d", size_to_read);
            } //com este if e else não está a guardar o último chunk, caso seja <11
            
            else{
                //quando chega a 11, deve passar o array a uma função ou assim para
                //guardar os partial results, colocar size_to_read a 0 e esvaziar o array.
                //Depois segue para o próximo chunk
                savePartialResults(char_array, size_to_read);
                memset(char_array, 0, sizeof char_array); //esvaziar array
                size_to_read = 0;
            }
        }
        //printf("|%d|", size_to_read);
        for(int i = 0; i<size_to_read; i++){
            //printf("|%c|", char_array[i]);
            //wprintf(L"Cannot identify %lc\n", char_array[i]);
            //printf("%d", size_to_read);
        }
        if(size_to_read != 0){ //se ao fim de ler todo o ficheiro ainda tiver um size_to_read, significa q ainda falta um chunk
            savePartialResults(char_array, size_to_read);
            memset(char_array, 0, sizeof char_array); //esvaziar array
            size_to_read = 0;
        }
        //for(int i = 0; i<size_to_read; i++){
            //printf("|%c|", char_array[i]);
            //wprintf(L"Cannot identify %lc\n", char_array[i]);
            //printf("%d", size_to_read);
        //}
        
        
    }

    return 0;
}





    






