#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#define XWINDOW 640   // Largeur de la fenêtre en pixels
#define HWINDOW 640   // Hauteur de la fenêtre en pixels



/* Prototypes */
int dispGrid(mat mat1);
void pauseSDL(void);
int coordonneeCalc(SDL_Rect* pposition, unsigned int* pixel, mat mat1, int rang);
