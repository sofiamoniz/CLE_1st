#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Check if is vowel | 1-> is vowel
int is_vowel(unsigned char c){
    if(c=='a' || c=='e' || c=='i' || c=='o' || c=='u' || c=='y'){
        return 1;
    }
    else if(c=='A' || c=='E' || c=='I' || c=='O' || c=='Y'){
        return 1;
    }
    else{
        return 0;
    }
}

//Check if is alpha | 1-> is alpha
int check_alpha(unsigned char c){
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        return 1;
    }
    else if((c>='0')&&(c<='9')){
        return 1;
    }
    //else if(c=='_'){ //porque é q isto está aqui
      //  return 1;
    //}
    else{
        return 0;
    }
}

int check_space_separation_apostrophe(unsigned char c){
    if(c==' ' || c =='  ' || c==0xa){
        return 1;
    }
    else if(c=='-' || c=='_' || c=='"' || c == '[' || c ==']' || c=='(' || c==')'){
        return 1;
    }
    else if(c=='.' || c == ',' || c==':' || c==';' || c == '?' || c =='!' || c == 0xE28093 || c == 0xE280A6 ){
        return 1;
    }
    else{
        return 0;
    }
}
//the apostrophe (' 0x27) and single quotation marks (‘ 0xE28098 - ’ 0xE28099)
//are considered here to merge two words into a single one.
//rip
int symbol_transformation(unsigned char **c, int length){

}

