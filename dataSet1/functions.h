#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>


//read file and make conversion multibyte to singlebyte
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

//Check if ia space, separation or punctiation
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

//Check if is apostrophe
int is_apostrophe(unsigned char c){
    if(c==0x27){
        return 1;
    }
    else{
        return 0;
    }
}

//antigo get_size: https://stackoverflow.com/questions/33737803/how-to-compare-multibyte-characters-in-c
_Bool is_utf8(const char * string)
{
    if(!string)
        return 0;

    const unsigned char * bytes = (const unsigned char *)string;
    while(*bytes)
    {
        if( (// ASCII
             // use bytes[0] <= 0x7F to allow ASCII control characters
                bytes[0] == 0x09 ||
                bytes[0] == 0x0A ||
                bytes[0] == 0x0D ||
                (0x20 <= bytes[0] && bytes[0] <= 0x7E)
            )
        ) {
            bytes += 1;
            continue;
        }

        if( (// non-overlong 2-byte
                (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF)
            )
        ) {
            bytes += 2;
            continue;
        }

        if( (// excluding overlongs
                bytes[0] == 0xE0 &&
                (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// straight 3-byte
                ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
                    bytes[0] == 0xEE ||
                    bytes[0] == 0xEF) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) ||
            (// excluding surrogates
                bytes[0] == 0xED &&
                (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            )
        ) {
            bytes += 3;
            continue;
        }

        if( (// planes 1-3
                bytes[0] == 0xF0 &&
                (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// planes 4-15
                (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
                (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            ) ||
            (// plane 16
                bytes[0] == 0xF4 &&
                (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
                (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
                (0x80 <= bytes[3] && bytes[3] <= 0xBF)
            )
        ) {
            bytes += 4;
            continue;
        }

        return 0;
    }

    return 1;
}
