#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "functions.h"
//https://stackoverflow.com/questions/13238388/remove-letter-accents-from-a-given-text

int main(int argc, char **argv){
    setlocale(LC_CTYPE, "");
    FILE *f = fopen(argv[1], "r");
    if (!f)
        return 1;

    for (wchar_t c; (c = fgetwc(f)) != WEOF;){
        
        //wprintf(L"%lc", c);
        char new_char = convert_multibyte('1');
        printf("  %d", is_alpha_underscore(new_char));
        printf('-' == '—');
        

    }
    
    fclose(f);
    return 0;
}
/*
int main(int argc, char **argv){
    setlocale(LC_CTYPE, "");
    FILE *f = fopen(argv[1], "r");
    if (!f)
        return 1;

    for (wchar_t c; (c = fgetwc(f)) != WEOF;){
        switch (c) {
            unsigned char new_char;
            //left or right single
            //quotion marks to apostrophe
            case L'`': c=0x27; break;
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
            case L'–': c=L'-';break;
            case L'—': c=L'-';break;
            //ellipsis to full point
            case L'…': c=L'.';break;


            default:    break;
        }
        wprintf(L"%lc", c);
        

    }
    
    fclose(f);
    return 0;
}
*/