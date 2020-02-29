#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>

#include "../include/main.h"
#include "../include/affichage.h"
#include "../include/rules.h"


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


int coordonneeCalc(SDL_Rect* pposition, unsigned int* pixel, mat mat1, dimensions dim, int rang) {
  int i, j;
  i = rang / dim.width;
  j = (rang % dim.width);
  if (i < 0 || i > dim.height || j < 0 || j > dim.width) {
    errorMSG("Erreur dans le calcul des coordonnées");
    printf("i = %d et j = %d (WIDTH=%d et HEIGHT=%d)\n", i, j, dim.width, dim.height);
    printf("rang = %d\n", rang);
    return -1;
  }
  if (mat1[i][j] == '1')    // Cellule vivante
    *pixel = 0;     // Couleur = noir
  else if(mat1[i][j] == '0')    // Cellule morte
    *pixel = 255;       // Couleur = blanc
  else {
    errorMSG("Erreur dans la valeur des coefficients de la matrice");
    return ERRORVALUE;
  }
  pposition->x = j*(XWINDOW / dim.width);
  pposition->y = i*(HWINDOW / dim.height);
  return 0;
}



int dispGrid(mat mat1, dimensions dim) {
  SDL_Surface* ecran = NULL;
  int nb_cells = (dim.height)*(dim.width);
  SDL_Surface* cellule[DIMH*DIMX] = {NULL};
  SDL_Rect position;
  int i, xlarge, hlarge, code;
  unsigned int pixel;
  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
    errorMSG("Erreur à l'initialisation de la librairie SDL");
    printf("%s\n", SDL_GetError());
    return ERRORVALUE;
  }
  ecran = SDL_SetVideoMode(XWINDOW, HWINDOW, 32, SDL_SWSURFACE);
  SDL_WM_SetCaption("## Conway's game ##", NULL);
  if (ecran == NULL) {
    errorMSG("Erreur à l'ouverture de la fenêtre SDL");
    printf("%s\n", SDL_GetError());
    return ERRORVALUE;
  }
  xlarge = XWINDOW / (dim.width);   // Largeur d'une cellule
  hlarge = HWINDOW / (dim.height);    // Hauteur d'une cellule
  for (i = 0; i < nb_cells; i++) {
    cellule[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, xlarge, hlarge, 32, 0, 0, 0, 0);
  }

  for (i = 0; i < nb_cells; i++) {
    code = coordonneeCalc(&position, &pixel, mat1, dim, i);
    if (code < 0)
      return ERRORVALUE;
    SDL_FillRect(cellule[i], NULL, SDL_MapRGB(ecran->format, pixel, pixel, pixel));
    SDL_BlitSurface(cellule[i], NULL, ecran, &position);
  }
  SDL_Flip(ecran);    // Mise à jour de l'écran
  pauseSDL();     // Attente

  for (i = 0; i < DIMH*DIMX; i++) {
    SDL_FreeSurface(cellule[i]);
  }
  SDL_Quit();
  return 0;
}



int playGame(mat mat1, dimensions dim) {
  SDL_Surface* ecran = NULL;
  SDL_Event event;
  SDL_Surface* cellule[DIMH*DIMX] = {NULL};
  SDL_Rect position;
  int nb_cells = (dim.height)*(dim.width);    // Nombre total de cellules
  int continuer = 1; int prevtime = 0; int actualtime = 0; int age = 0;
  int i, xlarge, hlarge, code;
  unsigned int pixel;

  if (SDL_Init(SDL_INIT_VIDEO) == -1) {
    errorMSG("Erreur à l'initialisation de la librairie SDL");
    printf("%s\n", SDL_GetError());
    return ERRORVALUE;
  }
  ecran = SDL_SetVideoMode(XWINDOW, HWINDOW, 32, SDL_SWSURFACE);    // Configuration mode de fenetrage
  SDL_WM_SetCaption("## Conway's game ##", NULL);     // Titre
  if (ecran == NULL) {
    errorMSG("Erreur à l'ouverture de la fenêtre SDL");
    printf("%s\n", SDL_GetError());
    return ERRORVALUE;
  }
  xlarge = XWINDOW / (dim.width);   // Largeur d'une cellule
  hlarge = HWINDOW / (dim.height);    // Hauteur d'une cellule
  for (i = 0; i < nb_cells; i++) {
    cellule[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, xlarge, hlarge, 32, 0, 0, 0, 0);   // Création de toutes les cellules
  }

  while (age < GENERATION && continuer == 1) {    // Tant que la dernière génération n'est pas atteinte et que l'utilisateur n'a pas demandé à quitter
    SDL_PollEvent(&event);
    switch(event.type) {    // Demande à quitter
      case SDL_QUIT:
        continuer = 0;
        break;
    }
    actualtime = SDL_GetTicks();      // Temps actuel depuis le début de l'exécution du programme
    if (actualtime - prevtime > WAITTIME) {     // Si le temps d'attente est dépassé
      printf("Génération n°%d\n", age + 1);
      prevtime = actualtime;          // Ancien temps devient nouveau temps
      code = newMatrix(&mat1, dim);     // Calcul de la prochaine génération de cellules
      if (code < 0)
        return ERRORVALUE;
      age ++;       // Incrémente la génération
    }
    else {
      SDL_Delay(WAITTIME - (actualtime - prevtime));    // endort le processus pour libérer le CPU provisoirement
    }
    for (i = 0; i < nb_cells; i++) {
      code = coordonneeCalc(&position, &pixel, mat1, dim, i);   // Calcule les coordonnées graphique de la cellule, et la couleur qu'elle doit avoir
      if (code < 0)
        return ERRORVALUE;
      SDL_FillRect(cellule[i], NULL, SDL_MapRGB(ecran->format, pixel, pixel, pixel));   // Met à jour les paramètres de la cellule
      SDL_BlitSurface(cellule[i], NULL, ecran, &position);      // Ajoute la cellule à l'écran
    }
    SDL_Flip(ecran);    // Mise à jour de l'écran
  }

  pauseSDL();     // Génération finale atteinte, attente de fermeture de la fenêtre
  for (i = 0; i < DIMH*DIMX; i++) {
    SDL_FreeSurface(cellule[i]);      // Libère toutes les cellules
  }
  SDL_Quit();     // Quitte la SDL
  return 0;
}
