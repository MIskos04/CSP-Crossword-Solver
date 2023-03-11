#include "functions.h"
#include "libraries.h"

void free_memory(slot_pointer *slots, int words, char ***dict, int *count, int max_length, uint32_t **** bitmap, char *ltable, char **crossw, char **crossw2, int size){
    int i, j, m;
    for(i = 0; i < words; i++){
        free(slots[i]->current_domain);
        free(slots[i]->word);
        free(slots[i]->past_fc->elements);
        free(slots[i]->past_fc);
        free(slots[i]->future_fc->elements);
        free(slots[i]->future_fc);
        for(j = 0; j < slots[i]->n_intercepts; j++){
            free(slots[i]->intercepts[j]);
        }
        free(slots[i]->intercepts);
        for(j = 0; j < slots[i]->reductions->size; j++){
            free(slots[i]->reductions->elements[j]);
        }
        free(slots[i]->reductions->elements);
        free(slots[i]->reductions);
        free(slots[i]);
    }
    free(slots);

    for(i = 0; i < max_length; i++){
        for(j = 0; j < i + 1; j++){
            for(m = 0; m < 26; m++){
                free(bitmap[i][j][m]);
            }
            free(bitmap[i][j]);
        }
        free(bitmap[i]);
    }
    free(bitmap);
    free(ltable);

    for(i = 0; i < max_length; i++){
        for(j = 0; j < count[i]; j++){
            free(dict[i][j]);
        }
        free(dict[i]);
    }
    free(dict);
    free(count);


    for(i = 0; i <= size; i++){
        free(crossw[i]);
        free(crossw2[i]);
    }
    free(crossw);
    free(crossw2);

}



//Main Function of the program
int main(int argc, char * argv[]) {
    int i,words, max_length, size, draw, check, solved;
    char **crossw, **crossw2, dict_file[81];

    //Initializes variables check, draw and dict_file to default values
    check = 0;
    draw = 0;
    strcpy(dict_file,"Words.txt");

    //Assigns variables check,draw and dict_file values bases on the arguments in the command line
    read_arguments(argc, argv, dict_file, &check, &draw);

    //Creates crossword template based on input
    crossw = create_crossword(argv[1], &crossw2, &size, &words, &max_length);

    //Representing each word in the crossword as an entity
    slot_pointer * slots = create_slots(crossw, crossw2, size, words);

    //Stores the number of words for each length
    int *count = calloc(max_length, sizeof(int ));
    
    //Creates a 3 dimensional array with the words from the dictionary
    char ***dict = create_dictionary(dict_file, max_length, count);

    //Creates the 4-dimensional array bitmap[length][poisition][letter][ints]. It contains a bit array. If the i-th word of the dict[length][i] has the "letter" in the specific position, then the "i-th" bit in the bit array is 1.
    //Due to the limitation that we can't store too many bits in a variable, we store the bit array as layers of 32 bits each.
    uint32_t ****bitmap = create_bitmap(dict, count, max_length);

    //A look-up table containing the number of set bits for every 16-bit number. 
    char *ltable = create_lookup_table();

    //Creates the current domain for each slot, alongside with attributes that are closely related with it.
    slots = create_domains(slots, dict, words, count);


    if(!check){
        solved = solver(slots, dict, words, count, bitmap, ltable);
        
    }else{
        solved = check_solution(slots, dict, count, words);
    }

    if(solved){

        if(draw){                                        //If draw is enables it draws the solution
            draw_crossword(slots, crossw, words, size);
        }else if(!check){
            for(i = 0; i < words; i++){
                printf("%s\n",slots[i]->word);
            }
        }

    }else if(!check){
        fprintf(stderr,"No solution found\n");
    }

    free_memory(slots, words, dict, count, max_length, bitmap, ltable, crossw, crossw2, size);
}

