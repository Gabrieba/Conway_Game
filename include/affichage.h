#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#define XWINDOW 640   // Width of the SDL cells window (in pixels)
#define HWINDOW 640   // Height of the SDL cells window (in pixels)
#define XINTERFACE 180

#define STRIPELARGE 1   // Width of a stripe (horizontal and vertical) (in pixels)

#define WAITTIME    600   // Waiting time (in ms) between two SDL updates


/* Function prototypes */
int initGrid(mat* mat1, dimensions dim);
int dispGrid(mat mat1, dimensions dim);
void pauseSDL(void);
int parametersCell(SDL_Rect* pposition, unsigned int* pixel, mat mat1, dimensions dim, int rang);
int playGame(mat mat1, dimensions dim);
