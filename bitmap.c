#include "libraries.h"
#define a(n) n, n+1, n+1, n+2
#define b(n) a(n), a(n+1), a(n+1), a(n+2)
#define c(n) b(n), b(n+1), b(n+1), b(n+2)
#define d(n) c(n), c(n+1), c(n+1), c(n+2)
#define e(n) d(n), d(n+1), d(n+1), d(n+2)
#define f(n) e(n), e(n+1), e(n+1), e(n+2)
#define g(n) f(n), f(n+1), f(n+1), f(n+2)

//Creates a lookup table where the value for each index is the number of set bits it has.
//Its indexes can take values in the space [0,2^16-1]
char *create_lookup_table(void){
    char *ltable = malloc(((1 << 16)) * sizeof(char));
    if(ltable == NULL){
        printf("Failed to allocate memory\n");
        exit(0);
    }
    char temp_ltable[] = {g(0), g(1), g(1), g(2)};
    memcpy(ltable,temp_ltable,(1 << 16) * sizeof(char));
    return ltable;
}

#undef a
#undef b
#undef c
#undef d
#undef e
#undef f
#undef g


uint32_t ****create_bitmap(char ***dict,int *count,int max_length){
    int i,j,k;
    uint32_t ****bitmap;
    
    bitmap = malloc(max_length * sizeof(uint32_t *** ));        //First index is about the length of the words.
    if(bitmap == NULL){
        printf("Failed to allocate memory\n");
        exit(0);
    }

    for (i = 0; i < max_length; i++) {                         
        bitmap[i] = malloc((i + 1) * sizeof(uint32_t ** ));     //Second index is about the position inside the word
        if(bitmap[i] == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
    }

    for (i = 0; i < max_length; i++) {
        for(j = 0; j < (i + 1); j++){
            bitmap[i][j] = malloc(26 * sizeof(uint32_t *));     //Third index is about the letter in the position given by the second index
            if(bitmap[i][j] == NULL){
                printf("Failed to allocate memory\n");
                exit(0);    
            }
        }
    }

    for(i = 0; i < max_length; i++){
        for(j = 0; j < (i + 1); j++){
            for(k = 0; k < 26; k++){
                bitmap[i][j][k] = calloc(count[i]/32 + 1, sizeof(uint32_t));    //Due to the fact that we can't store bit-arrays of unlimited size in a variable we store them as 32 bit integers. Fourth index is about which integer the word is saved at.
                if(bitmap[i][j][k] == NULL){
                    printf("Failed to allocate memory\n");
                    exit(0);
                }
            }
        }
    }
    
    for(i = 0; i < max_length; i++){
        for(j = 0; j < count[i]; j++){
            for(k = 0; k <= i; k++){
                bitmap[i][k][dict[i][j][k] - 'a'][j / 32] |= 1 << (j % 32);
            }
        }
    }
    return bitmap;
}
