#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#define XWINDOW 640   // Largeur de la fenêtre en pixels
#define HWINDOW 640   // Hauteur de la fenêtre en pixels

#define GENERATION  10
#define WAITTIME    900   // Temps d'attente (ms) entre deux fenêtre d'affichage

/* Prototypes */
int dispGrid(mat mat1, dimensions dim);
void pauseSDL(void);
int coordonneeCalc(SDL_Rect* pposition, unsigned int* pixel, mat mat1, dimensions dim, int rang);
int playGame(mat mat1, dimensions dim);
