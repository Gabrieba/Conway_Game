#include <stdio.h>

#define ERRORVALUE    -1
#define ERRORSTRING   1
#define EXITVALUE     2
#define LOADVALUE     3

#define DIMH      50
#define DIMX      50

#define STYLE_BOLD    0x01
#define STYLE_OFF     0x00
#define COLOR_RED     31
#define COLOR_YELLOW  33


enum cell_state {ALIVE = 1, DEAD = 0};    // States of a cell

typedef char** mat;   // Matrix of cells

typedef struct {
              int height;
              int width;
} dimensions;                 // Dimensions of the matrix


// States of a cell

/* Function prototypes */
void errorMSG(char* msg);
void warningMSG(char* msg);
void helpCommand(int bit_load);
int stringStandardise(char* cmd, char* filename);
int loadGrid(char* filename, mat* pgrid, dimensions* dim);
int emptyMatrix(mat grid, dimensions dim);
void printMatrix(mat grid, dimensions dim);
int createMatrix(mat* pmat);
void destroyMatrix(mat* pmat);
int executecmd(char* cmd, char* filename, mat* mat1, dimensions* dim, int bit_load);
