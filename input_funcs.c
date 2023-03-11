#include "libraries.h"

char ** create_crossword(char * crossword_file, char *** crossw2, int  * size, int * words, int * max_length) {
    int i, j, x, y, length;
    char **crossw;
    FILE *ptr;

    *max_length = -1;       //Stores the maximum length of word that can fit in the crossword
    ptr = fopen(crossword_file, "r");
    if (ptr == NULL) {
        printf("Couldn't access crossword file\n");
        exit(0);
    }
    
    fscanf(ptr, "%d ", size);
    //One extra block for each row and column to store /0
    crossw = malloc((* size + 1) * sizeof(char * ));
    *crossw2 = malloc((*size + 1) * sizeof(char * ));       //A crossword whose lines are crossw's columns

    if (crossw == NULL || *crossw2 == NULL) {

        printf("Failed to allocate memory\n");
        exit(0);

    } else {

        for (i = 0; i <=  *size; i++) {

            crossw[i] = malloc((* size + 1) * sizeof(char));
            (*crossw2)[i] = malloc((* size + 1) * sizeof(char));
            if (crossw[i] == NULL || (*crossw2)[i] == NULL) {
                printf("Failed to allocate memory\n");
                exit(0);
            }

        }

    }

    for (i = 0; i <= * size; i++) {
        for (j = 0; j <= * size; j++) {
            crossw[i][j] = '1';
        }
    }

    for (i = 0; i <= * size; i++) {

        crossw[*size][i] = '\0';
        crossw[i][*size] = '\0';

    }

    while (!feof(ptr)) {

        fscanf(ptr, "%d %d ", & x, & y);
        crossw[x - 1][y - 1] = '\0';

    }

    for (i = 0; i <= * size; i++) {
        for (j = 0; j <= * size; j++) {

            //Crossw2 is a crossword that has crossw's columns as lines
            (* crossw2)[j][i] = crossw[i][j];

        }
    }

    //Counting how many words it contains
    * words = 0;
    j = 0;
    for (i = 0; i < *size; i++,j = 0) {
        while (j < *size) {
            if (crossw[i][j] != 0) {

                length = strlen(crossw[i] + j);
                if(length > *max_length){                    
                    *max_length = length;
                }

                if (length > 1) {
                    (*words) ++;
                }else{
                    if(strlen((*crossw2)[j] + i) == 1){         //Words of length 1 can't exist in the crossword. Therefore if the program finds one it terminates.
                        if(i == 0){
                            printf("Words must have more than 1 letter\n");
                            exit(0);
                        }else if((*crossw2)[j][i - 1] == 0){
                            printf("Words must have more than 1 letter\n");
                            exit(0);
                        }
                    }
                }

                j += length;

            }else{
                j++;
            }
        }

        j = 0;
        while (j <= * size - 1) {
            if (( * crossw2)[i][j] != 0) {

                length = strlen(( * crossw2)[i] + j);

                if(length > *max_length){                    
                    *max_length = length;
                }

                if (length > 1) {
                    ( * words) ++;
                }
                j += length;

            } else {

                j++;

            }
        }
    }

    if(fclose(ptr)){
        printf("Failed to close crossword file\n");
        exit(0);
    }

    return crossw;

}

char *** create_dictionary(char * file, int max_length, int * count) {
    char temp[81], ***dict;
    int length, i, j;
    FILE * ptr;
    ptr = fopen(file, "r");
    if (ptr == NULL) {
        printf("Couldn't access dictionary file\n");
        exit(0);
    }

    while (!feof(ptr)) {
        fscanf(ptr, "%80s ", temp);
        length = strlen(temp);

        if (length <= max_length && length > 0) {            
            count[length-1] ++;
        }
    }

    dict = malloc(max_length * sizeof(char ** ));
    if(dict == NULL){
        printf("Failed tp allocate memory\n");
        exit(0);
    }

    for (i = 0; i <= max_length - 1; i++) {
        dict[i] = malloc(count[i] * sizeof(char * ));
        if(dict[i] == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
    }

    for (i = 0; i <= max_length - 1; i++) {
        for(j = 0; j < count[i]; j++){

            dict[i][j] = malloc((i + 2) * sizeof(char));
            if(dict[i][j] == NULL){
                printf("Failed to allocate memory\n");
                exit(0);
            }

        }
        
    }

    for (i = 0; i <= max_length - 1; i++) {
        count[i] = 0;
    }
   
    rewind(ptr);
    if (ptr == NULL) {
        printf("Failed to rewind file");
        exit(0);
    }
     
    while (!feof(ptr)) {

        fscanf(ptr, "%80s ", temp);
        length = strlen(temp);
        if (length <= max_length && length > 0) {
            strcpy(dict[length-1][count[length-1]++], temp);        //Stores the word in the dictionary
        }

    }

    if(fclose(ptr)){
        printf("Failed to close dictionary file\n");
        exit(0);
    }

    return dict;
}