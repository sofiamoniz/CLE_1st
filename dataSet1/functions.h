#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

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

//Check if ia space, separation or punctiation
int is_space_separation_punctuation(unsigned char c){
    if(c==' ' || c =='  ' || c==0xa){ //space
        return 1;
    }
    else if(c=='-' || c=='"' || c == '[' || c ==']' || c=='(' || c==')'){ //separation
        return 1;
    }
    else if(c=='.' || c == ',' || c==':' || c==';' || c == '?' || c =='!' || c == 0xE28093 || c == 0xE280A6 ){ //punctuation
        return 1;
    }
    else{
        return 0;
    }
}

//Check if is apostrophe
int is_apostrophe(unsigned char c){
    if(c==0x27){
        return 1;
    }
    else{
        return 0;
    }
}

//tirei daqui https://stackoverflow.com/questions/33737803/how-to-compare-multibyte-characters-in-c
//get size of bytes read of utf-8 char
int get_size(char * c){
    int i=0, char_len;

    setlocale(LC_CTYPE, "en_US.utf8");

    while ((char_len = mblen(&c[i], MB_CUR_MAX)) > 0)
    {
        /* &text[i] contains multibyte character of length char_len */
        if(memcmp(&c[i], "ö", char_len) == 0)
        {
            printf("ö \n");
        }

        i += char_len;
    }

    return i;
}
//convert multi byte to single byte
unsigned char conversor_multibyte(char ** c, int char_len){
    if(char_len == 1){
        unsigned char c_first = (*c)[0]; //first and only part of char, in this case
        //c_first == '´' este dá erro
        if (c_first == '`'){ //if the char is equal to left or right single
                                            //quotion marks, it should be converted to apostrophe
            return 0x27;
        }
    }
    else if(char_len == 2){
        unsigned char c_first = (*c)[0];
        unsigned char c_second = (*c)[1];
        if(c_first == 0xc3){
            //Á–À–Â–Ã–á–à–â–ã to A :
            if(c_second == 0x80 || c_second == 0x81 || c_second == 0x82 || c_second == 0x83){
                return 'A';
            }
            else if(c_second == 0xa0 || c_second == 0xa1 || c_second == 0xa2 || c_second == 0xa3){
                return 'a';
            }
            //É–È–Ê–é–è–ê to E :
            else if(c_second == 0x88 || c_second == 0x89 || c_second == 0x8a){
                return 'E';
            }
            else if(c_second == 0xa8 || c_second == 0xa9 || c_second == 0xaa){
                return 'e';
            }
            //Í–Ì–í–ì to I
            else if(c_second == 0x8c || c_second == 0x8d){
                return 'I';
            }
            else if(c_second == 0xac || c_second == 0xad ){
                return 'i';
            }
            //Ó–Ò–Ô–Õ–ó–ò–ô–õ
            else if(c_second == 0x92 || c_second == 0x93 || c_second == 0x94 || c_second == 0x95){
                return 'O';
            }
            else if(c_second == 0xb2 || c_second == 0xb3 || c_second == 0xb4 || c_second == 0xb5){
                return 'o';
            }
            //Ú–Ù–ú–ù to U
            else if(c_second == 0x99 || c_second == 0x9a){
                return 'U';
            }
            else if(c_second == 0xb9 || c_second == 0xba ){
                return 'u';
            }
            //Ç–ç to C
            else if(c_second == 0x87 || c_second == 0xa7){
                return 'C';
            }
        }
        //left and right double quotation marks to double quotation mark
        else if(c_first == 0xc2){
            if(c_second == 0xab || c_second == 0xbb){
                return '"';
            }
        }
    }
    else if(char_len == 3){
        unsigned char c_first = (*c)[0];
        unsigned char c_second = (*c)[1];
        unsigned char c_third = (*c)[2];
        //dash to hyphen
        if((c_first== 0xe2 && c_second == 0x80 ) && (c_third == 0x92 || c_third == 0x93)){
            return '-';
        }
        //ellipsis to full point
        else if(c_first == 0xe2 && c_second == 0x80 && c_third == 0xa6){
            return '.';
        }
    }
    else{
        printf("Error reading char.\n");
        exit(1);
    }
}