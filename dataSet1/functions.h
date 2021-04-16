#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>

//funçao do edgar q nao entendo
//supostamente retorna o size de cada char para depois
//usar em conversor_multibyte
int check_char_multibyte(FILE *in_file, unsigned char **char_array) {
	unsigned char c;
	*char_array = NULL;

	if ((c = fgetc(in_file)) == EOF){
		return 0;
	}

	if ((c>>7)==0){
		*char_array = (unsigned char *)calloc(1, sizeof(c));
		(*char_array)[0] = c;
		return 1;
	}
	else if ((c>>5)==6){
		unsigned char c2 = fgetc(in_file);
		*char_array = (unsigned char *)calloc(2, sizeof(c));
		(*char_array)[0] = c;
		(*char_array)[1] = c2;
		return 2;
	}
	else if ((c>>4)==14){
		unsigned char c2 = fgetc(in_file);
		unsigned char c3 = fgetc(in_file);
		*char_array = (unsigned char *)calloc(3, sizeof(c));
		(*char_array)[0] = c;
		(*char_array)[1] = c2;
		(*char_array)[2] = c3;
		return 3;
	}
	else if ((c>>3)==30){
		unsigned char c2 = fgetc(in_file);
		unsigned char c3 = fgetc(in_file);
		unsigned char c4 = fgetc(in_file);
		*char_array = (unsigned char *)calloc(4, sizeof(c));
		(*char_array)[0] = c;
		(*char_array)[1] = c2;
		(*char_array)[2] = c3;
		(*char_array)[3] = c4;
		return 4;
	}
	else if (c==0xff){
		return 0;
	}
	else {
		printf("File char format error!");
		exit(1);
	}
}

//convert multi byte to single byte
unsigned char conversor_multibyte(unsigned char ** c, int char_len){
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

//antigo get_size: https://stackoverflow.com/questions/33737803/how-to-compare-multibyte-characters-in-c

