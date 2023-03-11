#include "structs.h"



slot_pointer * create_domains(slot_pointer * slots, char ***dict, int words, int *count){
    int i,j,k;
    for(i = 0; i < words; i++){
        
        slots[i]->ints = count[slots[i]->length - 1] / 32 + 1;

        //Initialize current domain
        slots[i]->current_domain = calloc((slots[i]->ints), sizeof(uint32_t));
        if(slots[i]->current_domain == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
        slots[i]->domain_size = count[slots[i]->length - 1];
        for(j = 0; j < count[slots[i]->length - 1]; j++){
            slots[i]->current_domain[j/32] |= 1u << (j % 32);
        }

        //Initialize reductions
        slots[i]->reductions = malloc(sizeof(struct stack2d));
        if(slots[i]->reductions == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
        slots[i]->reductions->elements = malloc(slots[i]->n_intercepts * sizeof(uint32_t *));
        if(slots[i]->reductions->elements == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
        for(k = 0; k < slots[i]->n_intercepts; k++){
            slots[i]->reductions->elements[k] = calloc(slots[i]->ints, sizeof(uint32_t));
            if(slots[i]->reductions->elements[k] == NULL){
                printf("Failed to allocate memory\n");
                exit(0);
            }
        }
        slots[i]->reductions->top = -1;
        slots[i]->reductions->size = slots[i]->n_intercepts;

        //Initialize future_fc
        slots[i]->future_fc = malloc(sizeof(struct stack));
        if(slots[i]->future_fc == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
        slots[i]->future_fc->elements = malloc((slots[i]->n_intercepts) * sizeof(int));
        if(slots[i]->future_fc->elements == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
        slots[i]->future_fc->top = -1;
        slots[i]->future_fc->size = slots[i]->n_intercepts;

        //Initialize past_fc
        slots[i]->past_fc = malloc(sizeof(struct stack));
        if(slots[i]->past_fc == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
        slots[i]->past_fc->elements = malloc(slots[i]->n_intercepts * sizeof(int));
        if(slots[i]->past_fc->elements == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
        slots[i]->past_fc->top = -1;
        slots[i]->past_fc->size = slots[i]->n_intercepts;    

    }

    return slots;
}




//Initializes the slots
void slot_init(slot_pointer * slots, int orientation, int length, int x, int y, int k) {
    slots[k] -> orientation = orientation;
    slots[k] -> length = length;
    slots[k] -> x = x;
    slots[k] -> y = y;
    slots[k] -> word = malloc((length + 1) * sizeof(char));
    if(slots[k]->word == NULL){
        printf("Failed to allocate memory\n");
        exit(0);
    }
    slots[k] -> instantiated = 0;
    slots[k] -> n_intercepts = 0;
}

//Checks if element1 <= element2 <= element1 + length. Returns 1 if it's true or 0 if it's false.
int in_range(int element1, int element2, int length) { 
    int constraint = (element1 <= element2) && (element2 <= element1 + length);
    return constraint;
}

//The first slot is the horizontal one and the second is the vertical one. If they intersect the function returns 1,otherwise it returns 0. 
int intersected(slot_pointer horizontal, slot_pointer vertical) {
    int flag1, flag2;
    flag1 = in_range(vertical -> x, horizontal -> x, vertical -> length - 1);
    flag2 = in_range(horizontal -> y, vertical -> y, horizontal -> length - 1);
    return flag1 && flag2;
}

//
slot_pointer * find_intercepts(slot_pointer * slots, int words) {
    int i, j, k = 0;

    for (i = 0; i <= words - 1; i++) {
        slots[i]->intercepts = malloc(slots[i]->n_intercepts * sizeof(struct square *));
        if(slots[i]->intercepts == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }

        for(j = 0; j < slots[i]->n_intercepts; j++){
            slots[i]->intercepts[j] = malloc(sizeof(struct square));
            if(slots[i]->intercepts[j] == NULL){
                printf("Failed to allocate memory\n");
                exit(0);
            }
        }

    }

    for (i = 0; i < words; i++, k = 0) {
        for (j = 0; j < words; j++) {
            if (slots[i] -> orientation == slots[j] -> orientation) {
                continue;
            } else {
                if (slots[i] -> orientation == HORIZONTAL) {
                    if (intersected(slots[i], slots[j])) {
                        slots[i]->intercepts[k]->slot = j;
                        slots[i]->intercepts[k]->position = slots[j] -> y - slots[i] -> y;
                        slots[i]->intercepts[k++]->slot_position = slots[i]->x - slots[j] -> x;
                    }
                } else {
                    if (intersected(slots[j], slots[i])) {
                        slots[i]->intercepts[k]-> position = slots[j] -> x - slots[i] -> x;
                        slots[i]->intercepts[k]->slot = j;
                        slots[i]->intercepts[k++]->slot_position = slots[i] -> y - slots[j] -> y;
                    }
                }
            }
        }
        
    }
    return slots;
}


//Consecutive blank boxes are considered as slots. In this funtion we "extract" the slots from the crossword grid, along with their properties.
slot_pointer * create_slots(char ** crossw, char ** crossw2, int size, int words) {
    int i, j, length, k = 0;
    slot_pointer * slots = malloc(words * sizeof(slot_pointer));
    if(slots == NULL){
        printf("Failed to allocate memory\n");
        exit(0);
    }

    for (i = 0; i <= words - 1; i++) {
        slots[i] = malloc(sizeof(struct slot));
        if(slots[i] == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
    }

    j = 0;
    for (i = 0; i < size; i++, j = 0) {
          
        while (j <= size - 1) {
            if (crossw[i][j] == 0) {
                j++;
                continue;
            } else {
                length = strlen(crossw[i] + j);

                if (length > 1) {
                    slot_init(slots, HORIZONTAL, length, i, j, k++);                  //We initialize each slot with some of its properties including length, orientation, starting position, etc.
                }
                j += length;
            }
        }
        
        
    }

    j = 0;
    for(i = 0; i < size; i++, j = 0){
        while (j <= size - 1) {

            if (crossw2[i][j] == 0) {

                j++;

            }else {

                length = strlen(crossw2[i] + j);
                if (length > 1) {
                    slot_init(slots, VERTICAL, length, j, i, k++);
                }
                j += length;

            }

        }

    }

    //Finding how many intercepts with other slots each slot has.
    for(i = 0; i < words; i++){
        for(j = 0;j < words; j++){
            if(slots[i]->orientation == slots[j]->orientation){
                continue;
            }else{
                if(slots[i]->orientation == HORIZONTAL){
                    if(intersected(slots[i],slots[j])){
                        slots[i]->n_intercepts++;
                    }
                }else{
                    if(intersected(slots[j],slots[i])){
                        slots[i]->n_intercepts++;
                    }
                }
            }
        }
    }
    slots = find_intercepts(slots, words);

    return slots;
}
