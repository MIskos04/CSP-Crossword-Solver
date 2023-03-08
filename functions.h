#include "structs.h"
//Input Functions
char ***create_dictionary(char *, int, int *);
char ** create_crossword(char * , char *** , int *, int * , int *);

//Entities function
slot_pointer *create_slots(char **,char **,int ,int );
slot_pointer * create_domains(slot_pointer * , char ***, int , int *);
int solver(slot_pointer *, char ***, int , int *, uint32_t ****, char * );

//Bitmap Functions
uint32_t **** create_bitmap(char ***, int *, int);
char * create_lookup_table(void);

//Arguments Functions
void read_arguments(int , char **, char *, int *, int *);
int check_solution(slot_pointer *, char ***, int *, int );
void draw_crossword(slot_pointer *, char **, int , int );