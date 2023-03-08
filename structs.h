#include "libraries.h"
#define HORIZONTAL 0
#define VERTICAL 1
typedef struct slot * slot_pointer;
typedef struct square * square_pointer;
typedef struct stack * stack_pointer;
typedef struct stack2d *stack2d_pointer;

struct slot{
    int orientation;                      //Takes the value 0 if it is horizontal or 1 if it is vertical.
    int x;                                //x of starting position.
    int y;                                //y of starting position.
    int length;                           //Length of word that can fill the slot.
    int instantiated;                     //Takes the value 1 if the slot is instantiated with a word or 0 if it's not.
    char *word;                           //Stores the value assigned to the slot.
    int domain_size;                      //The domain size of the slot.
    int ints;                             //The domain of a slot is stored as 32-bit integers. Ints is the number of 32-bit integers used.
    uint32_t *current_domain;             //The available words of this slot.
    stack2d_pointer reductions;           //The words that were excluded because of the instantiations of past slots.
    stack_pointer past_fc;                //A stack containing the slots that forward checked against this slot.
    stack_pointer future_fc;              //A stack containing the slots that this slot forward checks against.
    int n_intercepts;                     //The number of intercepts with the rest of the slots.
    square_pointer *intercepts;           //Contains informations about the intercepts.
};

struct square{
    int position;                         //The position it has in the the slot.
    int slot;                             //The other slot that intercepts with the slot in the specific square.
    int slot_position;                    //The positionn it has on the other slot.
};

struct stack{
    int top;
    int size;
    int *elements;
};

struct stack2d{
    int top;
    int size;
    uint32_t **elements;
};