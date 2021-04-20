#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//correr 
//gcc count.c   
//gcc -o count count.c  
//./count text0.txt text1.txt text2.txt text3.txt text4.txt
int check_vowel(char a)
{
  char no[] = "ÁÀÂÃáàâãÉÈÊéèêÍÌíìÓÒÔÕóòôõÚÙúùÇçyY";
  if (a == 'A' || a == 'E' || a == 'I' || a == 'O' || a == 'U')
    return 1;
  else if (a == 'a' || a == 'e' || a == 'i' || a == 'o' || a == 'u')
    return 1;
  else    // You may omit this else as the control comes here if the character is not a vowel.
    for (int i = 0; strlen(no);++i){
        printf("entrei");
        if (a == no[i]){
            printf("Reconhece");
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[])
{
    char in_name[80];
    FILE *in_file;
    char word[50];
    int wrd ;
    int consonant = 0;
    

    //printf("Enter file name:\n");
    //scanf("%s", in_name);
    for(int i=1; i<argc; i++){
        in_file = fopen(argv[i], "r");

        if (in_file == NULL)
            printf("Can't open %s for reading.\n", in_name);
        else
        {
            while (fscanf(in_file, "%s", word) != EOF)
            {
                //printf("%s\n", word);
                ++wrd;
                for (int i = 0; word[i] != '\0';++i){
                    //printf("(%c)", word[i]);
                    if ((word[i] >= 'a' && word[i] <= 'z') || (word[i] >= 'A' && word[i] <= 'Z')) {
                        //if (check_vowel(word[i]) == 0){
                            //printf("(%c)", word[i]);
                            //consonant++;
                        //}
                        
                    }
                    else{
                        char no[] = "ÁÀÂÃáàâãÉÈÊéèêÍÌíìÓÒÔÕóòôõÚÙúùÇçyY";
                        char noA[] = "ÁÀÂÃáàâã";
                        for (int j = 0; strlen(no);++j){
                            printf("entrei");
                            if (word[i] == noA[j]){
                                printf("Reconheceeeee");
                                
                                return 0;
                            }
                            else{
                                printf("errado");
                            }
                        }
                    }
                    

                    
                        /*if ((word[i] >= 'a' && word[i] <= 'z') || (word[i] >= 'A' && word[i] <= 'Z')) {
                            for(int j=0; j<strlen(no);j++){
                                if(word[i] != no[j]){
                                    ++consonant;
                                }
                            }
                            
                        }
                        */
                    
                    
                }

            }
            printf("\n The number of words in the  file %s is : %d\n",argv[i],wrd-1); 
            printf("\n The number of consonants in the  file %s is : %d\n",argv[i],consonant); 
            fclose(in_file);
        }
    }
    return 0;
}
