#include "structs.h"

//Pushes an integer in a stack of integers
void push(int element, stack_pointer stack){
    if(stack->top != stack->size - 1){
        stack->elements[++stack->top] = element;
    }
}

//Pops an integer from a stack of integers
int pop(stack_pointer stack){
    if(stack->top >= 0){
        return stack->elements[stack->top--];
    }else{
        return -1;
    }
}

//Pops an array from a stack of arrays
uint32_t * pop2d(stack2d_pointer stack){
    if(stack->top >= 0){
        return stack->elements[stack->top--];
    }else{
        return NULL;
    }
}

//Pushes an array in a stack of arrays
void push2d(uint32_t *element, int ints, stack2d_pointer stack2d){
    int i;
    if(stack2d->top != stack2d->size - 1){
        stack2d->top++;
        for(i = 0; i < ints; i++){
            stack2d->elements[stack2d->top][i] = element[i];
        }
    }
}

//A function that calulates and returns the size of slots[i]'s current domain.
int calculate_domain_size(slot_pointer *slots, int i, char * ltable){
    int k,sum = 0;
    for(k = 0; k < slots[i]->ints; k++){
        sum = sum + ltable[slots[i]->current_domain[k] >> 16] + ltable[slots[i]->current_domain[k] & 0x0000FFFF];
    }
    return sum;
}

//A function that removes from slots[j]'s current domain all values that are not consistent with slots[i]'s current instantiation.
//It returns 1 if there are remaining values in slots[j]'s current domain or 0 if there are not.
int check_forward(slot_pointer *slots, char ***dict, uint32_t ****bitmap, int *count, char *ltable, int *consistent, int i, int j){
    int k,m;
    int index = slots[i]->intercepts[j]->slot;
    uint32_t reduction[slots[index]->ints];
    uint32_t temp;
    for(m = 0; m < slots[index]->ints; m++){
        temp = bitmap[slots[index]->length - 1][slots[i]->intercepts[j]->slot_position][slots[i]->word[slots[i]->intercepts[j]->position] - 'a'][m];
        reduction[m] = slots[index]->current_domain[m] & (~temp);
        slots[index]->current_domain[m] &= temp;
    }
    slots[index]->domain_size = calculate_domain_size(slots, index, ltable);
    
    push2d(reduction, slots[index]->ints, slots[index]->reductions);
    push(index, slots[i]->future_fc);
    push(i, slots[index]->past_fc);
    return slots[index]->domain_size != 0;
}

//A function that undoes the reductions performed by the forwards checks of slots[i]'s current instantiation.
void undo_reductions(slot_pointer *slots, int i, char *ltable){
    int k,j;
    uint32_t *reduction;
    j = pop(slots[i]->future_fc);
    while(j != -1){
        reduction = pop2d(slots[j]->reductions);
        for(k = 0; k < slots[j]->ints; k++){
            slots[j]->current_domain[k] |= reduction[k];
        }
        slots[j]->domain_size = calculate_domain_size(slots, j, ltable);
        pop(slots[j]->past_fc);
        j = pop(slots[i]->future_fc);
    }
}

//This function is called when un-labeling slots[i]. It restores all the values that were removed from slots[i]'s current domain,
//because they resulted in the anhiliation of slots[j]'s current domain, where slots[j] is a slot that was not istantiated
//when slots[i] checked forward.
void update_current_domain(slot_pointer *slots, int i, char *ltable,int *count){
    int k,j;
    for(j = 0; j < count[slots[i]->length - 1]; j++){
        slots[i]->current_domain[j/32] |= 1u << (j % 32);
    }

    for(j = 0; j <= slots[i]->reductions->top; j++){
        for(k = 0; k < slots[i]->ints; k++){
            slots[i]->current_domain[k] ^= slots[i]->reductions->elements[j][k];
        }
    }
    slots[i]->domain_size = calculate_domain_size(slots, i, ltable);
    
}

//A function that attempts to instantiate slot[i]
int fc_label(slot_pointer * slots, char ***dict, uint32_t ****bitmap, int *count, char *ltable, int *consistent, int i, char **conf_set){
    int m,k,j, f, index, conflict, conflict_p;
    *consistent = 0;
    //Searching for words int the domain
    for(k = 0; k < slots[i]->ints && !(*consistent); k++){
        while(slots[i]->current_domain[k] && !(*consistent)){
            
            m = 0;
            while(!(slots[i]->current_domain[k] & (1u << m++))); //Find the rightmost bit
            m--;
            strcpy(slots[i]->word, dict[slots[i]->length - 1][m + 32*k]);
            
            *consistent = 1;
            //Check consistency
            for(j = 0; j < slots[i]->n_intercepts && *consistent; j++){
                index = slots[i]->intercepts[j]->slot;
                
                if(!slots[index]->instantiated){
                    *consistent = check_forward(slots, dict, bitmap, count, ltable, consistent, i, j);
                }
                
            }
            if(!(*consistent)){
                conflict_p = slots[i]->intercepts[j-1]->position;
                conflict  = slots[i]->word[conflict_p];
                slots[i]->current_domain[k] &= ~(1u << m);
                for(f = 0; f < slots[i]->ints; f++){
                    slots[i]->current_domain[f] &= ~bitmap[slots[i]->length - 1][conflict_p][conflict - 'a'][f];
                }
                slots[i]->domain_size--;
                undo_reductions(slots, i, ltable);
                for(f = 0; f <= slots[j - 1]->past_fc->top; f++){
                    conf_set[i][slots[j - 1]->past_fc->elements[f]] = 1;
                }
            }
        }
    }
    if(*consistent){
        slots[i]->instantiated = 1;
        return i+1;
    }else{
        slots[i]->instantiated = 0;
        return i;
    }
}

//A functions that uninstantiates slot[i] and undoes any forward check performed by slot[i]
int fc_unlabel(slot_pointer * slots, int *count, char *ltable, int *consistent, int i, stack_pointer filled, char **conf_set, int words){
    int h ,k, m, temp,flag = 0;

    for(k = filled->top; k >= 0; k--){
        temp = filled->elements[k];
        if(conf_set[i][temp]){
            flag = 1;
            break;
        }else{
            for(m = 0; m <= slots[i]->past_fc->top; m++){
                if(temp == slots[i]->past_fc->elements[m]){
                    flag =  1;
                    break;
                }
            }
            if(flag){
                break;
            }

        }
    }

    if(flag){
        h = temp;
    }else{
        filled->top = -1;
        *consistent = 0;
        return 0;
    }

    for(k = 0; k < words; k++){
        conf_set[h][k] |= conf_set[i][k];
    }
    for(k = 0; k <= slots[i]->past_fc->top; k++){
        conf_set[h][slots[i]->past_fc->elements[k]] = 1;
    }
    conf_set[h][h] = 0;
    
    slots[i]->instantiated = 0;
    update_current_domain(slots, i, ltable, count);
    memset(conf_set[i], 0, words);

    while((temp = pop(filled)) != h){
        memset(conf_set[temp], 0 , words);
        slots[temp]->instantiated = 0;
        undo_reductions(slots, temp, ltable);
        update_current_domain(slots, temp, ltable, count);
    }


    undo_reductions(slots, h, ltable);
    for(k = 0; k < slots[h]->ints;k++){
        if(slots[h]->current_domain[k]){
            m = 0;
            while(!(slots[h]->current_domain[k] & (1 << m++)));
            slots[h]->current_domain[k] &= ~(1u << (m - 1)); 
            break;
        }
    }
    slots[h]->domain_size--;
    *consistent = slots[h]->domain_size != 0;
    return h;
}

//Chooses which slot to instantiate next.
int choose_value(slot_pointer *slots, int words, stack_pointer filled){
    int i,j,flag,min_slot, max = -1, max_slot;
    unsigned int min= -1;
    if(filled->top == -1){
        for(i = 0; i < words; i++){
            if(slots[i]->n_intercepts > max){
                max = slots[i]->n_intercepts;
                max_slot = i;
            }
        }
        return max_slot;
    }
    for(i = 0;i < words; i++){
        flag = 0;
        for(j = 0; j<= filled->top; j++){
            if(i == filled->elements[j]){
                flag = 1;
                break;
            }
        }
        if(flag){
            continue;
        }else{
            if(slots[i]->domain_size < min){
                min = slots[i]->domain_size;
                min_slot = i;
            }
        }
    }
    return min_slot;
}

int solver(slot_pointer *slots, char ***dict, int words, int *count, uint32_t ****bitmap, char * ltable){
    int i, j, consistent = 1, status = -1;
    stack_pointer filled; 
    char **conf_set;

    filled = malloc(sizeof(struct stack));
    if(filled == NULL){
        printf("Failed to allocate memory\n");
        exit(0);
    }
    filled ->top = -1;
    filled->size = words;
    filled->elements = malloc(words*sizeof(int));
    if(filled->elements == NULL){
        printf("Failed to allocate memory\n");
        exit(0);
    }


    conf_set = malloc(words * sizeof(char *));
    if(conf_set == NULL){
        printf("Failed to allocate memory\n");
        exit(0);
    }
    for(i = 0; i < words; i++){
        conf_set[i] = calloc(words, sizeof(char));
        if(conf_set[i] == NULL){
            printf("Failed to allocate memory\n");
            exit(0);
        }
    }

    i = choose_value(slots, words,filled);
    while(status == -1){
        if(consistent){
            fc_label(slots, dict, bitmap, count, ltable, &consistent, i, conf_set);
            if(consistent){
                push(i, filled);
                i = choose_value(slots, words, filled);
            }
            
        }else{
            i = fc_unlabel(slots, count, ltable, &consistent, i, filled, conf_set, words);
        }

        if(filled->top == words-1){
            status = 1;
        }else if((filled->top == -1) && !consistent){
            
            status = 0;
        }
    }

    for(i = 0; i < words; i++){
        free(conf_set[i]);
    }
    free(conf_set);
    free(filled->elements);
    free(filled);
    return status;
}