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
  char file[256] = "grid/";
  enum cell_state state;
  strcat(file, filename);
  f1 = fopen(file, "r");
  if (f1 == NULL) {
    errorMSG("Fichier inexistant !");
    return -1;
  }
  fgets(chaine, 256, f1);
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++) {
      state = fgetc(f1);
      if (state != 48 && state != 49) {     // ASCII characters of '0' and '1'
        errorMSG("Erreur dans l'écriture du fichier grille");
        return -1;
      }
      (*pgrid)[i][j] = state;
    }
  }
  fclose(f1);
  return 0;
}



void printGrid(mat grid) {
  int i, j;
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++) {
      printf("%d", grid[i][j]);
    }
    puts("");
  }
  puts("");
}


void createGrid(mat* pgrid) {
  int i;
  mat grid = calloc(HEIGHT, sizeof(*grid));
  if (grid == NULL) {
    errorMSG("Erreur lors de l'allocation mémoire pour la grille");
    return;
  }
  for (i = 0; i < WIDTH; i++) {
    grid[i] = calloc(WIDTH, sizeof(mat));
    if (grid[i] == NULL) {
      errorMSG("Erreur lors de l'allocation mémoire pour la grille");
      return;
    }
  }
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
  mat* pgrid;
  puts("\n \t \t WELCOME TO CONWAY'S GAME");
  char* cmd = calloc(256, sizeof(*cmd));
  if (cmd == NULL) {
    perror("Erreur lors de l'allocation mémoire de commande.");
    exit(EXIT_FAILURE);
  }
  while(1) {
    cmd = readline("Shell : > ");
    code = stringStandardise(cmd);
    if (code < 0) {
      errorMSG("Erreur string standardise");
      free(cmd);
      exit(EXIT_FAILURE);
    }
    if (strcmp(cmd, "exit") == 0) {
      free(cmd);
      exit(EXIT_SUCCESS);
    }
    if (strcmp(cmd, "load") == 0) {
      createGrid(pgrid);
      code = loadGrid("grid.txt", pgrid);
      printGrid(*pgrid);
    }
    cmd[0] = '\0';
  }
  puts("Should never be here");
}
