#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions_chunks.h"
int **counting_array;
int n_words;
int n_chars;
int n_consonants;
int in_word;
int max_chars;

//O PROBLEMA DEVE SER NA LINHA 147

//correr 
//gcc -o count count_chunks.c  
//./count text0.txt text1.txt text2.txt text3.txt text4.txt
void savePartialResults(char *char_array, int size_to_read){
    //printf("%d size_to_Read ", size_to_read);
    /*
    printf("tamanho %d ", size);
    for(int i = 0; i<=size; i++){
        printf("|%c|", char_array[i]);
    }
    */
    for(int i = 0; i<size_to_read; i++){ 
        char c = char_array[i];
        if(!in_word){
            if(is_alpha_underscore(c)){
                in_word = 1;
                n_words++;
                n_chars++;
                n_consonants = n_consonants + !is_vowel(c);
            }
            else if(is_apostrophe(c) || is_space_separation_punctuation(c)){
                continue;
            }
        }
        else{
            if(is_alpha_underscore(c)){
                n_chars++;
                n_consonants = n_consonants + !is_vowel(c);
            }
            else if(is_apostrophe(c)){
                continue;
            }
            else if(is_space_separation_punctuation(c)){
                in_word = 0;
                counting_array[n_chars-1][n_consonants]++;
                if(n_chars > max_chars){
                    max_chars = n_chars;
                }
                n_chars = 0;
                n_consonants = 0;
            }
        }
    }
}
void printProcessingResults(){
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

int main(int argc, char *argv[])
{
    for(int i=1; i<argc; i++){
        int size_to_read = 0; //esvaziar de cada vez q é encontrado um novo ficheiro
        char char_array[15]; //15 é um tamanho qualquer, podemos escolher. escolhi este para ser pequeno
                            //e testar com o text0.txt. Dei um espaço a mais só pelo sim pelo não
        
        //char converted_char;
        setlocale(LC_CTYPE, "");
        FILE *f = fopen(argv[i], "r");
        //array for countings - max size of word is 50
        //o counting_array e outras vars globais de calculos vao esvaziar cada vez que for lido um novo ficheiro
        counting_array = (int **)calloc(50, sizeof(int *));
        for (int j = 0; j<50; j++){
            counting_array[j] = (int *)calloc(j+2, sizeof(int));
        }
        n_words = 0;
        n_chars = 0;
        n_consonants = 0;
        in_word = 0;
        max_chars = 0;

        if (!f){
            printf("Can't open %s for reading.\n", argv[i]);
            return 1;}
        for (wchar_t c; (c = fgetwc(f)) != WEOF;){
            //first, we do the conversion - if char is not
            //multibyte, it will remain unibyte
            //printf("|%d|", size_to_read);
            if(size_to_read<=12){ //12 ou um nº qualquer desde q seja menor do q o valor com q o array foi inicializado
                //acho q o problema está no size_to_read<ALGO
                char converted_char = convert_multibyte(c);
                char_array[size_to_read] = converted_char;
                size_to_read++;
                //printf("%d", size_to_read);
            } //com este if e else não está a guardar o último chunk, caso seja <=12 - ou seja, caso no último passo entre no if
            
            else{
                //quando chega a 12, deve passar o array a uma função para
                //guardar os partial results, colocar size_to_read a 0 e esvaziar o array
                //para poder seguir para o próximo chunk
                //printf("else %d\n", size_to_read);
                savePartialResults(char_array, size_to_read);
                memset(char_array, 0, sizeof char_array); //esvaziar array
                size_to_read = 0;
            }
        }
        fclose(f); 
        /*
        for(int i = 0; i<size_to_read; i++){
            printf("|%c|", char_array[i]);
            //printf("%d", size_to_read);
        } 
        */
        if(size_to_read != 0){ //se ao fim de ler todo o ficheiro ainda tiver um size_to_read, significa q ainda falta um chunk
                                //ou seja, não entrou no else portanto não guardou os resultados
            //printf("fora %d\n", size_to_read);
            savePartialResults(char_array, size_to_read);
            memset(char_array, 0, sizeof char_array); //esvaziar array
            size_to_read = 0;
        }
        
        //até aqui funciona bem o savePartialResults(), sem as vars globais e só com prints. Agora vou tentar juntar os chunks para dar print final
        printProcessingResults();

        
    }//fim de for a iterar pelos ficheiros da linha de comandos

    return 0;
}


    






