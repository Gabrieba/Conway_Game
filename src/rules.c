#include <stdio.h>
#include <stdlib.h>

#include "../include/main.h"


// Compute the number of neighbours alive around the cell
// Note : the grid is cyclic
int calculNeighbours(mat mat1, dimensions dim, int i, int j) {
  int xposition, hposition;
  int res = 0;

  if (i-1 < 0)
    hposition = (dim.height)-1;
  else
    hposition = i-1;
  if (mat1[hposition][j] == 49) res++;     // DESSUS DIRECT

  if (j-1 < 0)
    xposition = (dim.width)-1;
  else
    xposition = j-1;
  if (mat1[hposition][xposition] == 49) res++;   // DESSUS GAUCHE

  if (j+1 > (dim.width)-1)
    xposition = 0;
  else
    xposition = j+1;
  if (mat1[hposition][xposition] == 49) res++;   // DESSUS DROITE

  if (mat1[i][xposition] == 49) res++;   // DROITE DIRECT

  if (j-1 < 0)
    xposition = (dim.width)-1;
  else
    xposition = j-1;
  if (mat1[i][xposition] == 49) res++;   // GAUCHE DIRECT

  if (i+1 > (dim.height)-1)
    hposition = 0;
  else
    hposition = i+1;
  if (mat1[hposition][j] == 49) res++;   // DESSOUS DIRECT

  if (j-1 < 0)
    xposition = (dim.width)-1;
  else
    xposition = j-1;
  if (mat1[hposition][xposition] == 49) res++;   // DESSOUS GAUCHE

  if (j+1 > (dim.width)-1)
    xposition = 0;
  else
    xposition = j+1;
  if (mat1[hposition][xposition] == 49) res++;   // DESSOUS DROITE

  return res;
}



// Copy the matrix mat1 in the matrix pmat
void copyMatrix(mat* pmat, mat* mat1, dimensions dim) {
  int i, j;
  for (i = 0; i < dim.height; i++) {
    for (j = 0; j < dim.width; j++) {
      (*pmat)[i][j] = (*mat1)[i][j];
    }
  }
}



// Calculate the new matrix by applying the rules
// Return -1 if an error has occured
// Return 1 if a stable configuration has been detected
// Return 0 otherwise
int newMatrix(mat* pmat, dimensions dim) {
  int i, j, res, code;
  mat mat1;
  code = createMatrix(&mat1);
  if (code < 0) {
    return ERRORVALUE;
  }

  for (i = 0; i < dim.height; i++) {
    for (j = 0; j < dim.width; j++) {
      res = calculNeighbours(*pmat, dim, i, j);
      if (res < 0 || res > 8) {
        errorMSG("Nombre de voisins absurde");
        return ERRORVALUE;
      }
      if ((*pmat)[i][j] == 49) {   // If the cell is initially alive
        if (res > 3 || res < 2) mat1[i][j] = 48;    // Mort par etouffement ou isolement
        else mat1[i][j] = 49;
      }
      else {    // If the cell is initially dead
        if (res == 3) {
          mat1[i][j] = 49;      // Birth
        }
        else mat1[i][j] = 48;
      }

    }
  }
  if (compareMatrix(*pmat, mat1, dim)) {     // Stable configuration has been detected
    infoMSG("Stable configuration has been detected");
    return 1;
  }
  copyMatrix(pmat, &mat1, dim);   // Update the new matrix in the current matrix
  destroyMatrix(&mat1);       // Destroy the temporary matrix
  return 0;
}
