#include <stdio.h>

#define STYLE_BOLD    0x01
#define STYLE_OFF     0x00
#define COLOR_RED     31
#define COLOR_YELLOW  33

#define WIDTH   21
#define HEIGHT  21

enum cell_state {ALIVE = 1, DEAD = 0};
typedef char** mat;

/* Prototypes */
void errorMSG(char* msg);
void warningMSG(char* msg);
int stringStandardise(char* cmd);
