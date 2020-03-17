#include <stdio.h>

#define MAXCONF 150


// Function prototypes
int textToRLE(char* fichier);
void matrixToRLE(char* conf, mat mat1, dimensions dim);
int oscilMatrix(mat mat1, dimensions dim);
