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


int stringStandardise(char* cmd) {
  int i, length;
  int j = 0;
  char* new_cmd = calloc(256, sizeof(*new_cmd));
  if (new_cmd == NULL) {
    perror("Erreur lors de l'allocation mémoire de commande.");
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
  puts("\n \t \t WELCOME TO CONWAY'S GAME");
  errorMSG("test ezf");
  warningMSG("test 2");
  char* cmd = calloc(256, sizeof(*cmd));
  if (cmd == NULL) {
    perror("Erreur lors de l'allocation mémoire de commande.");
    exit(EXIT_FAILURE);
  }
  while(1) {
    cmd = readline("Shell : > ");
    code = stringStandardise(cmd);
    if (code < 0) {
      perror("Erreur string standardise");
      exit(EXIT_FAILURE);
    }
    if (strcmp(cmd, "exit") == 0) {
      free(cmd);
      exit(EXIT_SUCCESS);
    }
    cmd[0] = '\0';
  }
  puts("Should never be here");
}
