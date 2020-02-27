#include <stdio.h>

#define ERRORVALUE -1
#define ERRORSTRING 1
#define EXITVALUE 2
#define LOADVALUE 3


#define STYLE_BOLD    0x01
#define STYLE_OFF     0x00
#define COLOR_RED     31
#define COLOR_YELLOW  33

#define WIDTH   40
#define HEIGHT  40

enum cell_state {ALIVE = 1, DEAD = 0};
typedef char** mat;

/* Prototypes */
void errorMSG(char* msg);
void warningMSG(char* msg);
int stringStandardise(char* cmd, char* filename);
int loadGrid(char* filename, mat* pgrid);
void printMatrix(mat grid);
int createMatrix(mat* pmat);
void destroyMatrix(mat* pmat);
int executecmd(char* cmd, mat* mat1, int bit_load);
