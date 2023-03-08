#include "structs.h"
void read_arguments(int argc, char *argv[], char *dict_file, int *check, int *draw){
    int i;
    if(argc < 2){
        fprintf(stderr,"usage : ./crossw 'crossword_file' -dict 'dictionary_file' -check -draw\n");
        exit(0);
    }
    if(argv[1][0] == '-'){
        fprintf(stderr,"usage : ./crossw 'crossword_file' -dict 'dictionary_file' -check -draw\n");
        exit(0);
    }
    i = 2;
    while(i < argc){
        if(!strcmp(argv[i],"-dict")){
            if(++i < argc){
                if(argv[i][0] != '-'){
                    strncpy(dict_file,argv[i++],80);
                }else{
                    fprintf(stderr,"usage : ./crossw 'crossword_file' -dict 'dictionary_file' -check -draw\n");
                    exit(0);
                }
            }else{
                fprintf(stderr,"usage : ./crossw 'crossword_file' -dict 'dictionary_file' -check -draw\n");
                exit(0);
            }
        }else if(!strcmp(argv[i], "-check")){
            *check = 1;
            i++;
        }else if(!strcmp(argv[i], "-draw")){
            *draw = 1;
            i++;
        }else{
            fprintf(stderr,"usage : ./crossw 'crossword_file' -dict 'dictionary_file' -check -draw\n");
            exit(0);
        }
    }
}


int check_solution(slot_pointer *slots, char ***dict, int *count, int words){
    char temp[81];
    int flag, index, i, j;
    i = 0;
    while(scanf("%80s",temp) != -1){
        flag = 1;
        if(i < words){

            if(slots[i]->length == strlen(temp)){

                for(j = 0; j < count[strlen(temp) - 1]; j++){

                    if(!strcmp(dict[slots[i]->length - 1][j], temp)){
                        flag = 0;
                        break;
                    }

                }
                if(flag){
                    printf("Word %c%s%c not in dictionary\n",'"',temp,'"');
                    return 0;
                }
                strcpy(slots[i]->word,temp);
                if(slots[i]->orientation == VERTICAL){
                    for(j = 0; j < slots[i]->n_intercepts; j++){

                        index = slots[i]->intercepts[j]->slot;
                        if(slots[i]->word[slots[i]->intercepts[j]->position] != slots[index]->word[slots[i]->intercepts[j]->slot_position]){
                            printf("Word %c%s%c cannot be placed\n", '"', slots[i]->word, '"');
                            return 0;
                        }

                    }
                }
                i++;
            }else{

                printf("Word %c%s%c cannot be placed\n", '"', temp, '"');
                return 0;
                
            }
        }else{
            printf("More words than needed\n");
            return 0;
        }
    }
    if(i != words){
        printf("Not enough words\n");
        return 0;
    }
    return 1;
}

void draw_crossword(slot_pointer *slots, char **crossw, int words, int size){
    int m, i, j;
    for(m = 0;m < words; m ++){
        if(slots[m]->orientation == HORIZONTAL){
            strcpy(crossw[slots[m]->x] + slots[m]->y, slots[m]->word);
        }else{
            for(j = 0; j < slots[m]->length; j++){
                crossw[slots[m]->x + j][slots[m]->y] = slots[m]->word[j];
            }

        }
    }

    for(i=0;i<size;i++){
        for(j=0;j<size;j++){
            if(crossw[i][j] == 0){
                printf("###");
            }else{
                printf("%2c ",crossw[i][j]);
            }
        }
        printf("\n");
    }
}