#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#define XWINDOW 640   // Width of the SDL window (in pixels)
#define HWINDOW 640   // Height of the SDL window (in pixels)

#define WAITTIME    600   // Waiting time (in ms) between two SDL updates


/* Function prototypes */
int dispGrid(mat mat1, dimensions dim);
void pauseSDL(void);
int coordonneeCalc(SDL_Rect* pposition, unsigned int* pixel, mat mat1, dimensions dim, int rang);
int playGame(mat mat1, dimensions dim);
