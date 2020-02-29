#include <stdio.h>
#include <stdlib.h>

#include "../include/main.h"


int calculNeighbours(mat mat1, int i, int j) {
  int xposition, hposition;
  int res = 0;

  if (i-1 < 0)
    hposition = HEIGHT-1;
  else
    hposition = i-1;
  if (mat1[hposition][j] == 49) res++;     // DESSUS DIRECT

  if (j-1 < 0)
    xposition = WIDTH-1;
  else
    xposition = j-1;
  if (mat1[hposition][xposition] == 49) res++;   // DESSUS GAUCHE

  if (j+1 > WIDTH-1)
    xposition = 0;
  else
    xposition = j+1;
  if (mat1[hposition][xposition] == 49) res++;   // DESSUS DROITE

  if (mat1[i][xposition] == 49) res++;   // DROITE DIRECT

  if (j-1 < 0)
    xposition = WIDTH-1;
  else
    xposition = j-1;
  if (mat1[i][xposition] == 49) res++;   // GAUCHE DIRECT

  if (i+1 > WIDTH-1)
    hposition = 0;
  else
    hposition = i+1;
  if (mat1[hposition][j] == 49) res++;   // DESSOUS DIRECT

  if (j-1 < 0)
    xposition = WIDTH-1;
  else
    xposition = j-1;
  if (mat1[hposition][xposition] == 49) res++;   // DESSOUS GAUCHE

  if (j+1 > WIDTH-1)
    xposition = 0;
  else
    xposition = j+1;
  if (mat1[hposition][xposition] == 49) res++;   // DESSOUS DROITE

  return res;
}



void copyMatrix(mat* pmat, mat* mat1) {
  int i, j;
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++) {
      (*pmat)[i][j] = (*mat1)[i][j];
    }
  }
}


int newMatrix(mat* pmat) {
  int i, j, res, code;
  mat mat1;
  code = createMatrix(&mat1);
  if (code < 0) {
    return ERRORVALUE;
  }

  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++) {
      res = calculNeighbours(*pmat, i, j);
      if (res < 0 || res > 8) {
        errorMSG("Nombre de voisins absurde");
        return -1;
      }
      if ((*pmat)[i][j] == 49) {   // Si la cellule est initialement vivante
        if (res > 3 || res < 2) mat1[i][j] = 48;    // Mort par etouffement ou isolement
        else mat1[i][j] = 49;
      }
      else {    // Si la cellule est initialement morte
        if (res == 3) {
          mat1[i][j] = 49;      // Naissance
        }
        else mat1[i][j] = 48;
      }

    }
  }
  copyMatrix(pmat, &mat1);
  destroyMatrix(&mat1);
  return 0;
}
