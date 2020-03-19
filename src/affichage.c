#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "../include/main.h"
#include "../include/affichage.h"
#include "../include/rules.h"
#include "../include/oscillat.h"



// Pause the graphic SDL display
// Wait the user request to quit the SDL window
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



// Compute the position of cell number 'rang' and store it in 'pposition'
// Compute the color of the cell according to its state and store it in 'pixel'
// Return ERRORVALUE if an error has occured
// Return 0 otherwise
int coordonneeCalc(SDL_Rect* pposition, unsigned int* pixel, mat mat1, dimensions dim, int rang) {
  int i, j;
  i = rang / dim.width;
  j = (rang % dim.width);
  if (i < 0 || i > dim.height || j < 0 || j > dim.width) {      // if the previous calculations are absurd ; for DEBUG
    errorMSG("Erreur dans le calcul des coordonnées");
    printf("i = %d et j = %d (WIDTH=%d et HEIGHT=%d)\n", i, j, dim.width, dim.height);
    printf("rang = %d\n", rang);
    return ERRORVALUE;
  }
  if (mat1[i][j] == '1')         // Alive cell
    *pixel = 0;                 // Color = black
  else if(mat1[i][j] == '0')    // Dead cell
    *pixel = 255;               // Color = white
  else {
    errorMSG("Erreur dans la valeur des coefficients de la matrice");
    return ERRORVALUE;
  }
  pposition->x = j*(XWINDOW / dim.width);     // Horizontal position of cell (i, j)
  pposition->y = i*(HWINDOW / dim.height);    // Vertical position of cell (i, j)
  return 0;
}




// Disp the grid 'mat1' with a graphic display
// Return ERRORVALUE if an error has occured, 0 otherwise
int dispGrid(mat mat1, dimensions dim) {
  SDL_Surface* ecran = NULL;
  int nb_cells = (dim.height)*(dim.width);        // Total number of cells
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
  xlarge = XWINDOW / (dim.width);         // Width of a cell (horizontal dimension)
  hlarge = HWINDOW / (dim.height);        // Height of a cell (vertical dimension)
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
  SDL_Color colortxt = {255, 0, 0};         // Text color for number of generations (red)
  SDL_Surface* textSurface = NULL;
  int nb_cells = (dim.height)*(dim.width);    // Total number of cells
  int continuer = 1; int prevtime = 0; int actualtime = 0; int age = 0;       // 'continuer' used to detect the user request to quit the SDL window
  int i, xlarge, hlarge, code;
  int bitoscill = 0;                // Used to indicate that an oscillation has already been detected
  unsigned int pixel;
  char buffer[1024];

  if (SDL_Init(SDL_INIT_VIDEO) == -1) {       // Open the SDL library
    errorMSG("Erreur à l'initialisation de la librairie SDL");
    printf("%s\n", SDL_GetError());
    return ERRORVALUE;
  }
  ecran = SDL_SetVideoMode(XWINDOW, HWINDOW, 32, SDL_SWSURFACE);    // window mode configuration
  SDL_WM_SetCaption("## Conway's game ##", NULL);                  // Title of the SDL window
  if (ecran == NULL) {
    errorMSG("Erreur à l'ouverture de la fenêtre SDL");
    printf("%s\n", SDL_GetError());
    SDL_Quit();                      // Quit the SDL library
    return ERRORVALUE;
  }
  xlarge = XWINDOW / (dim.width);      // Width of a cell (horizontal dimension)
  hlarge = HWINDOW / (dim.height);    // Height of a cell (vertical dimension)
  for (i = 0; i < nb_cells; i++) {
    cellule[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, xlarge, hlarge, 32, 0, 0, 0, 0);   // Cell SDL surfaces are created
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
  if (TTF_Init() == -1) {             // Open the TTF library
    errorMSG("Erreur à l'initialisation de la librairie TTF");
    printf("%s\n", SDL_GetError());
    SDL_Quit();                   // Quit the SDL library
    return ERRORVALUE;
  }
  police = TTF_OpenFont("Police/clootie.otf", 30);      // Get the police file
  code = oscilMatrix(mat1, dim);
  if (code < 0) {
    SDL_Quit();               // Quit the SDL library
    TTF_CloseFont(police);    // Close the police file
    TTF_Quit();               // Quit the TTF library
    return ERRORVALUE;
  }
  sprintf(buffer, "%d", age);
  textSurface = TTF_RenderText_Solid(police, buffer, colortxt);

  while (continuer == 1) {    // While the user don't ask for leaving
    SDL_PollEvent(&event);
    switch(event.type) {    // Ask for leaving the SDL while the grid is running
      case SDL_QUIT:
        continuer = 0;
        break;
    }

    actualtime = SDL_GetTicks();                  // Actual time since the beginning of the execution of the programm
    if (actualtime - prevtime > WAITTIME) {     // If the waiting time limit has been reached
      age++;                                       // Incremente the generation number
      sprintf(buffer, "%d", age);
      textSurface = TTF_RenderText_Solid(police, buffer, colortxt);

      prevtime = actualtime;          // Previous time becomes the actual time reference

      for (i = 0; i < nb_cells; i++) {
        code = coordonneeCalc(&position, &pixel, mat1, dim, i);   // Compute cordinates and color for cell i
        if (code < 0) {
          SDL_Quit();                // Quit the SDL library
          TTF_CloseFont(police);    // Close the police file
          TTF_Quit();               // Quit the TTF library
          return ERRORVALUE;
        }
        SDL_FillRect(cellule[i], NULL, SDL_MapRGB(ecran->format, pixel, pixel, pixel));   // Update the new parameters for cell i
        SDL_BlitSurface(cellule[i], NULL, ecran, &position);                             // Add the cell to the SDL window
      }
      SDL_Flip(ecran);    // Update the SDL window for cells

      position.x = 5;    // Position of the generation number
      position.y = 2;
      SDL_BlitSurface(textSurface, NULL, ecran, &position);     // Add the number of generations to the SDL window
      SDL_Flip(ecran);        // Update the SDL window for the generation number

      if (emptyMatrix(mat1, dim))   // If the cells matrix is fully empty
        break;

      code = newMatrix(&mat1, dim);     // Compute the next cells generation
      if (code < 0) {
        for (i = 0; i < DIMH*DIMX; i++) {
          SDL_FreeSurface(cellule[i]);      // Free all the cells
        }
        SDL_FreeSurface(ecran);   // Free the global SDL window
        SDL_Quit();                // Quit the SDL library
        TTF_CloseFont(police);    // Close the police file
        TTF_Quit();                // Quit the TTF library
        return ERRORVALUE;
      }
      if (code == 1)          // Stable configuration has been detected (over 2 successive generations)
        break;
      if (bitoscill == 0) {     // Oscillations detection is triggered only if no oscillation has been detected before
        code = oscilMatrix(mat1, dim);      // To detect matrix oscillations
        if (code < 0) {
          for (i = 0; i < DIMH*DIMX; i++) {
            SDL_FreeSurface(cellule[i]);      // Free all the cells
          }
          SDL_FreeSurface(ecran);   // Free the global SDL window
          SDL_Quit();               // Quit the SDL library
          TTF_CloseFont(police);    // Close the police file
          TTF_Quit();               // Quit the TTF library
          return ERRORVALUE;
        }
        if (code > 0)  {        // Matrix oscillation detected : 'code' contains the period
          bitoscill = 1;
          infoMSG("Matrix oscillations has been detected !");
          printf("Period = %d\n", code);
        }
      }
      if (age > 40)      // for Debug
        break;
    }

    else
      SDL_Delay(WAITTIME - (actualtime - prevtime));    // Puts the process to sleep

  }

  pauseSDL();                           // Fully empty grid reached, waiting for closing the SDL window
  for (i = 0; i < DIMH*DIMX; i++) {
    SDL_FreeSurface(cellule[i]);      // Free all the cells
  }
  SDL_FreeSurface(ecran);   // Free the global SDL window

  TTF_CloseFont(police);    // Close the police file
  SDL_Quit();             // Quit the SDL library
  TTF_Quit();              // Quit the TTF library
  return 0;
}
