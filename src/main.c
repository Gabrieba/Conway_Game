#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* The Readline library */
#include <readline/readline.h>
#include <readline/history.h>

#include "../include/main.h"
#include "../include/affichage.h"
#include "../include/rules.h"
#include "../include/oscillat.h"



void errorMSG(char* msg) {
  fprintf(stdout, "%c[%d;%dm", 0x1B, STYLE_BOLD, COLOR_RED);   // Set to bold red letters
  printf("ERROR : %s\n", msg);
  fprintf(stdout, "%c[%dm", 0x1B, 0);   // Reset
}


void warningMSG(char* msg) {
  fprintf(stdout, "%c[%d;%dm", 0x1B, STYLE_BOLD, COLOR_YELLOW);   // Set to bold yellow letters
  printf("WARNING : %s\n", msg);
  fprintf(stdout, "%c[%dm", 0x1B, 0);   // Reset
}


void infoMSG(char* msg) {
  fprintf(stdout, "%c[%d;%dm", 0x1B, STYLE_BOLD, COLOR_BLUE);   // Set to bold blue letters
  printf("INFO : %s\n", msg);
  fprintf(stdout, "%c[%dm", 0x1B, 0);   // Reset
}



// Disp a helpful guide for the user in the shell
void helpCommand(int bit_load) {
  puts("");
  puts("In the command shell :");
  puts("load <file.txt>    to load the initial grid in the shell (don't indicate the folder '/grid')");
  puts("disp               to print a grid with a graphic display");
  puts("run                to apply the rules on a cells matrix over 1 generation (without graphic display)");
  puts("play               to play the grid (previously loaded in the shell) over multiple generations (with graphic display)\n");
  puts("exit               to quit the shell");
  puts("");
  if (bit_load)
    infoMSG("Actually, an initial grid has successfully been loaded in the shell");
  else
    warningMSG("Actually, no initial grid has been loaded in the shell; please use the command 'load ...'");
  puts("");
}


// Charge une grille initiale dans l'interpréteur depuis un fichier
// Le contenu est chargé dans 'pgrid', ses dimensions dans 'dim'
int loadGrid(char* filename, mat* pgrid, dimensions* dim) {
  FILE* f1; int i, j, width, height;
  char chaine[256];
  char file[15] = "grid/";    // Nom du dossier
  enum cell_state state;
  strcat(file, filename);
  f1 = fopen(file, "r");
  if (f1 == NULL) {
    errorMSG("Ouverture du fichier grille initiale impossible");
    return ERRORVALUE;
  }
  fgets(chaine, 128, f1);   // Lecture ligne commentaire
  fscanf(f1, "%d %d", &height, &width);   // Lecture des dimensions
  if (height > DIMH || width > DIMX) {
    warningMSG("Matrice trop grande; veuillez changer 'DIMH' et/ou 'DIMX' dans main.h");
    return ERRORSTRING;
  }
  dim->height = height;
  dim->width = width;
  chaine[0] = '\0';
  fgets(chaine, 2, f1);   // Lecture du retour chariot pour atteindre le début de la grille

  char line[dim->width];
  for (i = 0; i < dim->height; i++) {
    if(fgets(line, (dim->width)+2, f1) == NULL) {
      errorMSG("Trop peu de lignes de cellules dans la grille");
      return ERRORVALUE;
    }
    for (j = 0; j < dim->width; j++) {
      state = line[j];
      if (state == '\n' || state == ' ') {
        errorMSG("Trop peu de cellules inscrites dans une ligne de la grille.");
        return ERRORVALUE;
      }
      if (state != 48 && state != 49) {     // ASCII characters of '0' and '1'
        errorMSG("Etat inconnu lu dans la grille (0/1)");
        printf("state = %c\n", state);
        return ERRORVALUE;
      }
      (*pgrid)[i][j] = (char) state;
    }
  }
  if(fgets(line, (dim->width)+2, f1) != NULL) {
    errorMSG("Trop de lignes de cellules dans la grille");
    return ERRORVALUE;
  }
  fclose(f1);
  return 0;
}



// Return 1 if the matrix is full of zero; 0 otherwise
int emptyMatrix(mat grid, dimensions dim) {
  int i, j;
  for (i = 0; i < dim.height; i++) {
    for (j = 0; j < dim.width; j++) {
      if (grid[i][j] == 49)
        return 0;
    }
  }
  return 1;
}



// Return 1 if the two matrix are equal, 0 otherwise
int compareMatrix(mat mat1, mat mat2, dimensions dim) {
  int i, j;
  for (i = 0; i < dim.height; i++) {
    for (j = 0; j < dim.width; j++) {
      if (mat1[i][j] != mat2[i][j])
        return 0;
    }
  }
  return 1;
}



// Print the matrix in the shell with '0' and '1'
void printMatrix(mat grid, dimensions dim) {
  int i, j;
  for (i = 0; i < dim.height; i++) {
    for (j = 0; j < dim.width; j++) {
      printf("%c ", grid[i][j]);
    }
    puts("");
  }
  puts("");
}



// Allocate dynamic memory to the cells matrix
// Return ERRORVALUE if an error has occured
// Return 0 otherwise
int createMatrix(mat* pmat) {
    int i;
    *pmat = malloc(DIMH * sizeof *(*pmat));     // Allocate an array of cells lines
    if (*pmat == NULL) {
        errorMSG("Allocation mémoire impossible pour la matrice");
        return ERRORVALUE;
    }
    for (i = 0; i < DIMH; i++) {
      (*pmat)[i] = calloc(DIMX, sizeof *(*pmat)[i]);    // Allocate a cells line
      if ((*pmat)[i] == NULL) {
        errorMSG("Allocation mémoire impossible pour la matrice");
        return ERRORVALUE;
      }
    }
    return 0;
}



// Free the allocated matrix
void destroyMatrix(mat* pmat) {
  int i;
  for (i = 0; i < DIMH; i++)
    free((*pmat)[i]);
  free(*pmat);
}



// Redirige l'exécution du programme vers les fonctions appropriées, suivant la commande rentrée
// bit_load vaut 1 si l'utilisateur a déjà chargé sa grille initiale dans l'interpréteur, 0 sinon
int executecmd(char* cmd, char* filename, mat* mat1, dimensions* dim, int bit_load) {
  int code;
  char command[128] = "ls grid | grep ";
  if (strcmp(cmd, "") == 0) {   // Commande vide
    return 0;
  }
  if (strcmp(cmd, "exit") == 0) {
    return EXITVALUE;
  }
  else if (strcmp(cmd, "load") == 0) {
    code = createMatrix(mat1);
    if (code < 0) {
      return ERRORVALUE;
    }
    strcat(command, filename);
    if (system(command) != 0) {   // Teste si la fichier 'filename' existe dans grid/
      warningMSG("Fichier grille initiale introuvable");
      return 0;
    }
    code = loadGrid(filename, mat1, dim);
    if (code != 0) {
      destroyMatrix(mat1);
      return code;
    }
    printMatrix(*mat1, *dim);
    return LOADVALUE;
  }
  else if (strcmp(cmd, "disp") == 0) {
    if (bit_load != 1) {
      warningMSG("Veuillez charger une grille initiale avant de l'afficher");
      return 0;
    }
    code = dispGrid(*mat1, *dim);
    if (code < 0) {
      destroyMatrix(mat1);
      return ERRORVALUE;
    }
    return 0;
  }
  else if (strcmp(cmd, "run") == 0) {
    if (bit_load != 1) {
      warningMSG("Veuillez charger une grille initiale avant de l'exécuter");
      return ERRORSTRING;
    }
    code = newMatrix(mat1, *dim);
    if (code < 0) {
      destroyMatrix(mat1);
      return ERRORVALUE;
    }
    printMatrix(*mat1, *dim);
    return 0;
  }
  else if (strcmp(cmd, "play") == 0) {
    if (bit_load != 1) {
      warningMSG("Veuillez charger une grille initiale avant de l'exécuter");
      return ERRORSTRING;
    }
    code = playGame(*mat1, *dim);     // Play with the grid 'mat1' and a graphic display
    system("rm tmp.rle");         // Remove temporary RLE file which contains all the RLE configurations of the previous game
    if (code < 0) {
      destroyMatrix(mat1);
      return ERRORVALUE;
    }
    return 0;
  }
  else if (strcmp(cmd, "convert") == 0) {
    strcat(command, filename);
    if (system(command) != 0) {           // Check wether the file is in the folder '/grid' or not
      warningMSG("Fichier grille initiale introuvable");
      return 0;
    }
    code = textToRLE(filename);     // Convert the file .txt to RLE file
    if (code < 0)
      return ERRORVALUE;
  }
  else if (strcmp(cmd, "help") == 0) {
    helpCommand(bit_load);
    return 0;
  }
  else {
    warningMSG("Commande inconnue");
    return ERRORSTRING;
  }
}



// Recover the command through 'cmd' and the filename (if it exists) through 'filename'
// Return ERRORSTRING if an error occured because of the user
// Return 0 otherwise
int stringStandardise(char* cmd, char* filename) {
  const char* separators = " -";
  char* str;                // Local variable used by strtok
  strcpy(str, cmd);
  char* strtoken = strtok(str, separators);   // Get the user command
  if (strtoken == NULL)       // Empty command
    return ERRORSTRING;
  strcpy(cmd, strtoken);         // Store the user command
  strtoken = strtok(NULL, separators);   // Get the filename
  if (strcmp(cmd, "load") != 0 && strcmp(cmd, "convert") != 0) {      //If the user command isn't 'load' or 'convert'
    if (strtoken != NULL) {                     // Too much arguments
      warningMSG("Trop d'arguments spécifiés");
      return ERRORSTRING;
    }
    return 0;
  }
  else {        // for commands 'load' and 'convert'
    if (strtoken == NULL) {
      warningMSG("Il manque le nom de la grille à traiter");
      return ERRORSTRING;
    }
    strcpy(filename, strtoken);         // Store the filename
    strtoken = strtok(NULL, separators);      // Check if something is written after the filename
    if (strtoken != NULL) {               // Too much arguments
      warningMSG("Trop d'arguments spécifiés");
      return ERRORSTRING;
    }
  }
  return 0;
}



// Main programm + errors handler
// Return EXIT_FAILURE if an other function returns ERRORVALUE
// Return EXIT_SUCCESS if the user asks to quit the shell
int main(int argc, char* argv[]) {
  int code; int bit_load = 0;
  mat mat1;
  dimensions dim;
  puts("\n \t \t WELCOME TO CONWAY'S GAME");
  char* cmd = calloc(128, sizeof(*cmd));          // Dynamic memory for user command
  if (cmd == NULL) {
    errorMSG("Erreur lors de l'allocation mémoire de commande.");
    exit(EXIT_FAILURE);
  }
  char* filename = calloc(128, sizeof(*filename));      // Dynamic memory for the filename
  if (cmd == NULL) {
    errorMSG("Erreur lors de l'allocation mémoire de nom de fichier.");
    exit(EXIT_FAILURE);
  }

  while(1) {
    cmd = readline("Shell : > ");             // Get the user command
    code = stringStandardise(cmd, filename);
    if (code == 0)                                     // If no error has occured
      code = executecmd(cmd, filename, &mat1, &dim, bit_load);        // Execute the user command
    cmd[0] = '\0';            // Erase the previous command
    filename[0] = '\0';       // Erase the previous filename
    switch(code) {      // Errors handler
      case 0 :          // No error
        break;
      case ERRORSTRING :    // Typo in a user command
        break;
      case EXITVALUE :      // The user asks to quit the shell
        free(cmd);
        free(filename);
        exit(EXIT_SUCCESS);
      case LOADVALUE :    // The user has loaded his initial grid
        bit_load = 1;
        break;
      case ERRORVALUE:      // An error has occured in an other function
        puts("Erreur détectée");
        free(cmd);
        free(filename);
        exit(EXIT_FAILURE);
      default :                 // Error occured in the errors handler
        errorMSG("Code de retour inconnu");
        free(cmd);
        free(filename);
        exit(EXIT_FAILURE);
    }
  }
  puts("Should never be here");
}
