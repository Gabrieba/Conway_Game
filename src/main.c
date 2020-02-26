#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* la librairie readline */
#include <readline/readline.h>
#include <readline/history.h>

#include "../include/main.h"




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
  char file[15] = "grid/";
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



void printGrid(mat grid) {
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
}



void destroyMatrix(mat* pmat) {
  int i;
  for (i = 0; i < HEIGHT; i++)
    free((*pmat)[i]);
  free(*pmat);
}


int stringStandardise(char* cmd) {
  int i, length;
  int j = 0;
  char* new_cmd = calloc(256, sizeof(*new_cmd));
  if (new_cmd == NULL) {
    errorMSG("Erreur lors de l'allocation mémoire de commande.");
    return(-1);
  }
  length = strlen(cmd);
  for (i=0; i<length; i++) {
    if (cmd[i] != ' ' && cmd[i] != '\t') {
      new_cmd[j] = cmd[i];
      j++;
    }
  }
  new_cmd[j] = '\0';
  strcpy(cmd, new_cmd);
  free(new_cmd);
  return 0;
}



int main(int argc, char* argv[]) {
  int code;
  mat mat1;
  puts("\n \t \t WELCOME TO CONWAY'S GAME");
  char* cmd = calloc(256, sizeof(*cmd));
  if (cmd == NULL) {
    perror("Erreur lors de l'allocation mémoire de commande.");
    exit(EXIT_FAILURE);
  }
  while(1) {
    cmd = readline("Shell : > ");
    code = stringStandardise(cmd);
    if (code < 0)
      break;
    if (strcmp(cmd, "exit") == 0) {
      free(cmd);
      exit(EXIT_SUCCESS);
    }
    if (strcmp(cmd, "load") == 0) {
      code = createMatrix(&mat1);
      if (code < 0) {
        destroyMatrix(&mat1);
        break;
      }
      code = loadGrid("grid.txt", &mat1);
      if (code < 0) {
        destroyMatrix(&mat1);
        break;
      }
      printGrid(mat1);
      destroyMatrix(&mat1);
    }
    cmd[0] = '\0';
  }
  puts("Erreur détectée");
  free(cmd);
  exit(EXIT_FAILURE);
}
