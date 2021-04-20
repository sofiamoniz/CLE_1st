#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>


//make conversion multibyte to singlebyte
//if it's already singlebyte, it breaks
char convert_multibyte(wchar_t c){
    switch (c) {
        //left or right single
        //quotion marks to apostrophe
        case L'`': c=0x27; break;
        case L'’': c=0x27; break;
        case L'‘': c=0x27; break;
        
        //á–à–â–ã to a :
        case L'à': c=L'a'; break;
        case L'á': c=L'a'; break;
        case L'â': c=L'a'; break;
        case L'ã': c=L'a'; break;
        //Á–À–Â–Ã to A:
        case L'Á': c=L'A'; break;
        case L'À': c=L'A'; break;
        case L'Â': c=L'A'; break;
        case L'Ã': c=L'A'; break;
        //é–è–ê to e :
        case L'è': c=L'e';break;
        case L'é': c=L'e';break;
        case L'ê': c=L'e';break;
        //É–È–Ê to E:
        case L'É': c=L'E';break;
        case L'È': c=L'E';break;
        case L'Ê': c=L'E';break;
        //í–ì to i
        case L'í': c=L'i';break;
        case L'ì': c=L'i';break;
        //Í–Ì to I
        case L'Í': c=L'I';break;
        case L'Ì': c=L'I';break;
        //ó–ò–ô–õ to o
        case L'ó': c=L'o'; break;
        case L'ò': c=L'o'; break;
        case L'ô': c=L'o'; break;
        case L'õ': c=L'o'; break;
        //Ó–Ò–Ô–Õ to O
        case L'Ó': c=L'O'; break;
        case L'Ò': c=L'O'; break;
        case L'Ô': c=L'O'; break;
        case L'Õ': c=L'O'; break;
        //ú–ù to u
        case L'ú': c=L'u';break;
        case L'ù': c=L'u';break;
        case L'ü': c=L'u';break;
        //Ú–Ù to U
        case L'Ú': c=L'U';break;
        case L'Ù': c=L'U';break;
        //Ç–ç to C
        case L'Ç': c=L'C';break;
        case L'ç': c=L'C';break;
        //left and right double quotation marks to double quotation mark
        case L'«': c=L'"';break;
        case L'»': c=L'"';break;

        //dash to hyphen
        case L'‒': c=L'-';break;
        case L'–': c=L'-';break;
        case L'—': c=L'-';break;

        //ellipsis to full point
        case L'…': c=L'.';break;

        //double quotion to "
        case L'”': c=L'"'; break;
        case L'“': c=L'"'; break;

        default:    break;
    }
    return c;
}



//Check if is vowel | 1-> is vowel
int is_vowel(unsigned char c){
    if(c=='a' || c=='e' || c=='i' || c=='o' || c=='u' || c=='y'){
        return 1;
    }
    else if(c=='A' || c=='E' || c=='I' || c=='O' || c=='U' || c=='Y'){
        return 1;
    }
    else{
        return 0;
    }
}

//Check if is alpha or underscore | 1-> is alpha or underscore
int is_alpha_underscore(unsigned char c){
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        return 1;
    }
    else if((c>='0')&&(c<='9')){
        return 1;
    }
    else if(c=='_'){
        return 1;
    }
    else{
        return 0;
    }
}

//Check if ia space, separation or punctiation. 1-> it is
int is_space_separation_punctuation(unsigned char c){
    if(c==' ' || c =='  ' || c==0xa){ //space
        return 1;
    }
    else if ((c=='-') || (c=='"') || (c=='[')||(c==']')||(c=='(')||(c==')')){ //separation
        return 1;
    }
    else if(c=='.' || c == ',' || c==':' || c==';' || c == '?' || c =='!' || c == 0xE28093 || c == 0xE280A6 ){ //punctuation
        return 1;
    }
    else{
        return 0;
    }
}

//Check if is apostrophe. 1-> it is
int is_apostrophe(unsigned char c){
    if(c==0x27){
        return 1;
    }
    else{
        return 0;
    }
}


void savePartialResults(char char_array[11], int size_to_read){
    //tem q se guardar global o array counting_array, n_words e max_chars, para
    //depois usar nas contas finais.
    //TODO: tirar os prints e fazer só as contas, para depois passar à função de print
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
    for(int i = 0; i<11; i++){
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
    printf("Total words: %d\n", n_words);

    printf("   ");
    for(int j = 0; j<max_chars; j++){
        printf("%5d ", j+1);
    }
    printf("\n");

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
    return;

}
