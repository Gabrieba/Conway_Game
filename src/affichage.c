#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "../include/main.h"
#include "../include/affichage.h"


void pauseSDL(void) {
  int etat = 1;
  SDL_Event event;
  while (etat) {
    SDL_WaitEvent(&event);
    switch(event.type) {
      case SDL_QUIT :
        etat = 0;
    }
  }
}


int coordonneeCalc(SDL_Rect* pposition, unsigned int* pixel, mat mat1, int rang) {
  int i, j;
  i = rang / WIDTH;
  j = (rang % WIDTH);
  if (i < 0 || i > HEIGHT || j < 0 || j > WIDTH) {
    errorMSG("Erreur dans le calcul des coordonnées");
    printf("i = %d et j = %d (WIDTH=%d et HEIGHT=%d)\n", i, j, WIDTH, HEIGHT);
    printf("rang = %d\n", rang);
    return -1;
  }
  if (mat1[i][j] == '1')
    *pixel = 255;     // Couleur = blanc
  else if(mat1[i][j] == '0')
    *pixel = 0;       // Couleur = noir
  else {
    errorMSG("Erreur dans la valeur des coefficients de la matrice");
    return -1;
  }
  pposition->x = j*(XWINDOW / WIDTH);
  pposition->y = i*(HWINDOW / HEIGHT);
  return 0;
}



int dispGrid(mat mat1) {
  SDL_Surface* ecran = NULL;
  int nb_cells = HEIGHT*WIDTH;
  SDL_Surface* cellule[HEIGHT*WIDTH] = {NULL};
  SDL_Rect position;
  int i, xlarge, hlarge, code;
  unsigned int pixel;
  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
    errorMSG("Erreur à l'initialisation de la librairie SDL");
    printf("%s\n", SDL_GetError());
    return -1;
  }
  ecran = SDL_SetVideoMode(XWINDOW, HWINDOW, 32, SDL_SWSURFACE);
  SDL_WM_SetCaption("## Conway's game ##", NULL);
  if (ecran == NULL) {
    errorMSG("Erreur à l'ouverture de la fenêtre SDL");
    printf("%s\n", SDL_GetError());
    return -1;
  }
  xlarge = XWINDOW / WIDTH;   // Largeur d'une cellule
  hlarge = HWINDOW / HEIGHT;    // Hauteur d'une cellule
  for (i = 0; i < nb_cells; i++) {
    cellule[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, xlarge, hlarge, 32, 0, 0, 0, 0);
  }

  for (i = 0; i < nb_cells; i++) {
    code = coordonneeCalc(&position, &pixel, mat1, i);
    if (code < 0)
      return -1;
    SDL_FillRect(cellule[i], NULL, SDL_MapRGB(ecran->format, pixel, pixel, pixel));
    SDL_BlitSurface(cellule[i], NULL, ecran, &position);
  }
  SDL_Flip(ecran);    // Mise à jour de l'écran
  pauseSDL();     // Attente

  for (i = 0; i < nb_cells; i++) {
    SDL_FreeSurface(cellule[i]);
  }
  SDL_Quit();
  return 0;
}
