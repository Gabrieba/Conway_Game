#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

#include "../include/main.h"
#include "../include/affichage.h"



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


int loadGrid(char* filename, mat* pgrid) {
  FILE* f1; int i, j;
  char chaine[256];
  char line[WIDTH];
  char file[15] = "grid/";    // Nom du dossier
  enum cell_state state;
  strcat(file, filename);
  f1 = fopen(file, "r");
  if (f1 == NULL) {
    errorMSG("Fichier inexistant !");
    return -1;
  }
  fgets(chaine, 256, f1);
  for (i = 0; i < HEIGHT; i++) {
    if(fgets(line, WIDTH+2, f1) == NULL) {
      errorMSG("Trop peu de lignes de cellules dans la grille");
      return -1;
    }
    for (j = 0; j < WIDTH; j++) {
      state = line[j];
      if (state == '\n' || state == ' ') {
        errorMSG("Trop peu de cellules inscrites dans une ligne de la grille");
        return -1;
      }
      if (state != 48 && state != 49) {     // ASCII characters of '0' and '1'
        errorMSG("Etat inconnu lu dans la grille (0/1)");
        printf("state = %c\n", state);
        return -1;
      }
      (*pgrid)[i][j] = (char) state;
    }
  }
  if(fgets(line, WIDTH+2, f1) != NULL) {
    errorMSG("Trop de lignes de cellules dans la grille");
    return -1;
  }
  fclose(f1);
  return 0;
}



void printMatrix(mat grid) {
  int i, j;
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++) {
      printf("%c ", grid[i][j]);
    }
    puts("");
  }
  puts("");
}



int createMatrix(mat* pmat) {
    int i;
    *pmat = malloc(HEIGHT * sizeof *(*pmat));
    if (*pmat == NULL) {
        printf("Allocation impossible");
        return -1;
    }
    for (i = 0; i < HEIGHT; i++) {
      (*pmat)[i] = calloc(WIDTH, sizeof *(*pmat)[i]);
      if ((*pmat)[i] == NULL) {
        printf("Allocation impossible");
        return -1;
      }
    }
    return 0;
}



void destroyMatrix(mat* pmat) {
  int i;
  for (i = 0; i < HEIGHT; i++)
    free((*pmat)[i]);
  free(*pmat);
}



// bit_load vaut 1 si l'utilisateur a déjà chargé sa grille initiale dans l'interpréteur
int executecmd(char* cmd, mat* mat1, int bit_load) {
  int code;
  if (strcmp(cmd, "") == 0) {   // Commande vide
    return 0;
  }
  if (strcmp(cmd, "exit") == 0) {
    return EXITVALUE;
  }
  else if (strcmp(cmd, "load") == 0) {
    code = createMatrix(mat1);
    if (code < 0) {
      destroyMatrix(mat1);
      return ERRORVALUE;
    }
    code = loadGrid("grid.txt", mat1);
    if (code < 0) {
      destroyMatrix(mat1);
      return ERRORVALUE;
    }
    printMatrix(*mat1);
    return LOADVALUE;
  }
  else if (strcmp(cmd, "disp") == 0) {
    if (bit_load != 1) {
      warningMSG("Veuillez charger une grille initiale avant de l'afficher");
      return 0;
    }
    code = dispGrid(*mat1);
    if (code < 0) {
      destroyMatrix(mat1);
      return ERRORVALUE;
    }
    return 0;
  }
  else if (strcmp(cmd, "run") == 0) {
    warningMSG("Pas encore implémenté !");
    return 0;
  }
  else {
    warningMSG("Commande inconnue");
    return 0;
  }
}



int stringStandardise(char* cmd, char* filename) {
  const char* separators = " ,.-;";
  char* str;
  strcpy(str, cmd);
  char* strtoken = strtok(str, separators);   // Récupération commande
  if (strtoken == NULL)       // Commande vide
    return ERRORSTRING;
  strcpy(cmd, strtoken);
  strtoken = strtok(NULL, separators);   // Récupération nom du fichier
  if (strcmp(cmd, "load") != 0) {
    if (strtoken != NULL) {
      warningMSG("Trop d'arguments spécifiés");
      return ERRORSTRING;
    }
    return 0;
  }
  else {        // Commande load
    if (strtoken == NULL) {
      warningMSG("Il manque le nom de la grille à charger");
      return ERRORSTRING;
    }
    strcpy(filename, strtoken);
    strtoken = strtok(NULL, separators);   // On regarde en 3eme position
    if (strtoken != NULL) {
      warningMSG("Trop d'arguments spécifiés");
      return ERRORSTRING;
    }
  }
  return 0;
}



int main(int argc, char* argv[]) {
  int code; int bit_load = 0;
  mat mat1;
  puts("\n \t \t WELCOME TO CONWAY'S GAME");
  char* cmd = calloc(128, sizeof(*cmd));
  if (cmd == NULL) {
    errorMSG("Erreur lors de l'allocation mémoire de commande.");
    exit(EXIT_FAILURE);
  }
  char* filename = calloc(128, sizeof(*filename));
  if (cmd == NULL) {
    errorMSG("Erreur lors de l'allocation mémoire de nom de fichier.");
    exit(EXIT_FAILURE);
  }
  while(1) {
    cmd = readline("Shell : > ");
    code = stringStandardise(cmd, filename);
    if (code == 0)
      code = executecmd(cmd, &mat1, bit_load);
    cmd[0] = '\0';
    filename[0] = '\0';
    switch(code) {      // Gestion des erreurs
      case 0 :
        break;
      case ERRORSTRING :    // Faute de frappe commandes
        break;
      case EXITVALUE :
        free(cmd);
        free(filename);
        exit(EXIT_SUCCESS);
      case LOADVALUE :    // L'utilisateur a chargé sa grille initiale
        bit_load = 1;
        break;
      case ERRORVALUE:
        puts("Erreur détectée");
        free(cmd);
        free(filename);
        exit(EXIT_FAILURE);
      default :
        errorMSG("Code de retour inconnu");
        free(cmd);
        free(filename);
        exit(EXIT_FAILURE);
    }
  }
  puts("Should never be here");
}
