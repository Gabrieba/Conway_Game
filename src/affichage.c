#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "../include/main.h"
#include "../include/affichage.h"
#include "../include/rules.h"
#include "../include/oscillat.h"


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
  SDL_Flip(ecran);    // Update the SDL window
  pauseSDL();     // Wait for a response from the user

  for (i = 0; i < DIMH*DIMX; i++) {
    SDL_FreeSurface(cellule[i]);
  }
  SDL_Quit();
  return 0;
}



int playGame(mat mat1, dimensions dim) {
  SDL_Surface* ecran = NULL;
  TTF_Font* police = NULL;
  SDL_Event event;
  SDL_Surface* cellule[DIMH*DIMX] = {NULL};
  SDL_Rect position;
  SDL_Color colortxt = {255, 0, 0};         // Text color to print the number of generations
  SDL_Surface* textSurface = NULL;
  int nb_cells = (dim.height)*(dim.width);    // Total number of cells
  int continuer = 1; int prevtime = 0; int actualtime = 0; int age = 1;
  int i, xlarge, hlarge, code;
  unsigned int pixel;
  char buffer[1024];

  if (SDL_Init(SDL_INIT_VIDEO) == -1) {       // Open the SDL library
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
  if (emptyMatrix(mat1, dim) == 1) {   // If matrix fully empty at the beginning
    for (i = 0; i < nb_cells; i++) {
      SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));   // All cells are white
      position.x = 0;
      position.y = 0;
      SDL_BlitSurface(ecran, NULL, ecran, &position);      // Add the cell to the SDL window
    }
    SDL_Flip(ecran);    // Update the new SDL window
  }
  if (TTF_Init() == -1) {       // Open the TTF library
    errorMSG("Erreur à l'initialisation de la librairie TTF");
    printf("%s\n", SDL_GetError());
    return ERRORVALUE;
  }
  police = TTF_OpenFont("Police/clootie.otf", 30);      // Get the police file
  code = oscilMatrix(mat1, dim);
  if (code < 0)
    return ERRORVALUE;
  sprintf(buffer, "%d", age);
  textSurface = TTF_RenderText_Solid(police, buffer, colortxt);

  while (emptyMatrix(mat1, dim) != 1 && continuer == 1) {    // While the grid is not fully empty and the user didn't ask for leaving
    SDL_PollEvent(&event);
    switch(event.type) {    // Ask for leaving the SDL while the grid is running
      case SDL_QUIT:
        continuer = 0;
        break;
    }
    for (i = 0; i < nb_cells; i++) {
      code = coordonneeCalc(&position, &pixel, mat1, dim, i);   // Calcule les coordonnées graphique de la cellule, et la couleur qu'elle doit avoir
      if (code < 0)
        return ERRORVALUE;
      SDL_FillRect(cellule[i], NULL, SDL_MapRGB(ecran->format, pixel, pixel, pixel));   // Met à jour les paramètres de la cellule
      SDL_BlitSurface(cellule[i], NULL, ecran, &position);      // Add the cell to the SDL window
    }
    SDL_Flip(ecran);    // Update the SDL window for cells

    position.x = 5;    // Position of the generation number
    position.y = 2;
    SDL_BlitSurface(textSurface, NULL, ecran, &position);     // Add the number of generations to the SDL window
    SDL_Flip(ecran);        // Update the SDL window for the generation number
    actualtime = SDL_GetTicks();      // Temps actuel depuis le début de l'exécution du programme

    if (actualtime - prevtime > WAITTIME) {     // Si le temps d'attente est dépassé
      sprintf(buffer, "%d", age);
      textSurface = TTF_RenderText_Solid(police, buffer, colortxt);

      prevtime = actualtime;          // Previous time becomes the actual time reference
      code = newMatrix(&mat1, dim);     // Compute the next cells generation
      if (code < 0)
        return ERRORVALUE;
      if (code == 1)          // Stable configuration has been detected
        break;
      age++;                            // Incremente the generation number
      code = oscilMatrix(mat1, dim);      // To detect matrix oscillations
      if (code < 0)
        return ERRORVALUE;
      if (code == 1)          // Matrix oscillation detected : count is incremented
        printf("Matrix oscillation has been detected at age %d !\n", age-1);
      if (age > 160) break;
    }

    else
      SDL_Delay(WAITTIME - (actualtime - prevtime));    // endort le processus pour libérer le CPU provisoirement
  }

  pauseSDL();     // Fully empty grid reached, waiting for closing the SDL window
  for (i = 0; i < DIMH*DIMX; i++) {
    SDL_FreeSurface(cellule[i]);      // Free all the cells
  }
  SDL_FreeSurface(ecran);   // Free the global SDL window

  TTF_CloseFont(police);    // Close the police file

  SDL_Quit();     // Quit the SDL library
  TTF_Quit();     // Quit the TTF library
  return 0;
}
