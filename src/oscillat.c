#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/main.h"
#include "../include/oscillat.h"



// Write a cells matrix from a text file into a RLE file
int textToRLE(char* fichier) {
  FILE* fp;
  char chaine[256];
  char file[15] = "grid/";    // Nom du dossier
  char cell; char cellref;
  strcat(file, fichier);
  int height, width, i, j; int coef = 1;

  fp = fopen(file, "r");
  if (fp == NULL) {
    errorMSG("Erreur lors de l'ouverture du fichier contenant la grille initiale");
    return ERRORVALUE;
  }
  file[strlen(file)-3] = 'r';     // To change extension .txt to .rle
  file[strlen(file)-2] = 'l';
  file[strlen(file)-1] = 'e';
  puts(file);
  FILE* f1 = fopen(file, "w");
  if (f1 == NULL) {
    errorMSG("Erreur lors de la création du fichier RLE");
    return ERRORVALUE;
  }

  fgets(chaine, 128, fp);
  fscanf(fp, "%d %d", &height, &width);   // Reading dimensions
  fgets(chaine, 2, fp);                 // Reading the carriage return to reach the beginning of the grid

  char line[width];
  cellref = fgetc(fp);     // Current cell reference
  for (j = 1; j < width; j++) {
    cell = fgetc(fp);
    if (cell == cellref)
     coef ++;           // Coefficient is incremented
    else {
      if (cellref == '1') fprintf(f1, "%dA", coef);      // Write the number of cells alive into the RLE file
      else fprintf(f1, "%dD", coef);      // Write the number of cells dead into the RLE file
      cellref = cell;         // Update the new current cell
      coef = 1;                // Coefficient is reset to 1
    }
  }
  fgetc(fp);    // Reading the carriage return between each line
  for (i = 1; i < height; i++) {
    for (j = 0; j < width; j++) {
      cell = fgetc(fp);
      if (cell == cellref)
       coef ++;           // Coefficient is incremented
      else {
        if (cellref == '1') fprintf(f1, "%dA", coef);      // Write the number of cells alive into the RLE file
        else fprintf(f1, "%dD", coef);      // Write the number of cells dead into the RLE file
        cellref = cell;         // Update the new current cell
        coef = 1;                // Coefficient is reset to 1
      }
    }
    fgetc(fp);    // Reading the carriage return between each line
  }
  if (cellref == '1') fprintf(f1, "%dA", coef);      // Write the number of cells alive into the RLE file
  else fprintf(f1, "%dD", coef);      // Write the number of cells dead into the RLE file
  fclose(fp);
  fclose(f1);
  return 0;
}



// Return the RLE configuration of a matrix cells through 'char* conf'
void matrixToRLE(char* conf, mat mat1, dimensions dim) {
  int i, j;
  int coef = 0;
  char cell;
  char buffer[8];

  cell = mat1[0][0];
  for (i = 0; i < dim.height; i++) {
    for (j = 0; j < dim.width; j++) {
      if (mat1[i][j] == cell)
        coef++;
      else {
        sprintf(buffer, "%d", coef);
        strcat(conf, buffer);
        if (cell == '1')       // Write the number of alive cells into the RLE configuration
          strcat(conf, "A");
        else               // Write the number of dead cells into the RLE configuration
          strcat(conf, "D");
        cell = mat1[i][j];         // Update the new current cell reference
        coef = 1;                // Coefficient is reset to 1
      }
    }
  }
  sprintf(buffer, "%d", coef);    // Add the very last coefficient
  strcat(conf, buffer);
  if (cell == '1') strcat(conf, "A");      // Write the number of alive cells into the RLE configuration
  else strcat(conf, "D");               // Write the number of dead cells into the RLE configuration
}





// Return 1 if the grid configuration has already been saved in the RLE file
// Return ERRORVALUE if an error has occured
// Otherwise save the new grid configuration in the RLE file at the end and return 0
int oscilMatrix(mat mat1, dimensions dim) {
  int nbconf = 0;
  char line[256];
  char* conf = calloc( dim.width, sizeof(*conf));
  if (conf == NULL) {
    errorMSG("Erreur lors d'une allocation dynamique pour la détection d'oscillations");
    return ERRORVALUE;
  }
  FILE* f1 = fopen("tmp.rle", "a+");
  if (f1 == NULL) {
    errorMSG("Erreur lors de l'ouverture du fichier intermédiaire");
    return ERRORVALUE;
  }
  matrixToRLE(conf, mat1, dim);        // Compute the RLE configuration of the matrix and store it in "conf"
  while (fgets(line, 1600, f1) != NULL) {      // While a matrix configuration can be read in the RLE file
    nbconf ++;
    line[strlen(line)-1] = '\0';      // In order to delete the carriage return at the end of 'line'
    if (strcmp(line, conf) == 0) {    // The same RLE configuration has already been saved previously
      return 1;
    }
  }
  if (nbconf < MAXCONF)       // Number of RLE configurations is limited to MAXCONF, large enough to detect any matrix oscillation I think
    fprintf(f1, "%s\n", conf);
  else {
    rewind(f1);         // Return to the beginning of the file, to delete the oldest RLE configuration
    fprintf(f1, "%s\n", conf);
  }
  free(conf);
  fclose(f1);
  return 0;
}
