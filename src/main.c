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
  puts("create             to create an initial grid with a pleasant graphic interface");
  puts("disp               to print a grid with a graphic display");
  puts("run                to apply the rules on a cells matrix over 1 generation (without graphic display)");
  puts("play               to play the grid (previously loaded in the shell) over multiple generations (with graphic display)\n");
  puts("exit               to quit the shell");
  puts("");
  if (bit_load)
    infoMSG("Actually, an initial grid has successfully been loaded in the shell");
  else
    warningMSG("Actually, no initial grid has been loaded in the shell; please use the command 'create' or 'load ...'");
  puts("");
}


// Load an initial grid in the shell from a '.txt' file
// The matrix is loaded in 'pgrid', its dimensions are loaded in 'dim'
// Return ERRORVALUE or ERRORSTRING if an error has occured, 0 otherwise
int loadGrid(char* filename, mat* pgrid, dimensions* dim) {
  FILE* f1; int i, j, width, height;
  char chaine[256];
  char file[15] = "grid/";    // Folder name
  enum cell_state state;
  strcat(file, filename);
  f1 = fopen(file, "r");
  if (f1 == NULL) {
    errorMSG("Initial grid file opening is impossible");
    return ERRORVALUE;
  }
  fgets(chaine, 128, f1);             // Read the comment in the first line
  fscanf(f1, "%d %d", &height, &width);   // Read the dimensions and store it 'height' and 'width'
  if (height > DIMH || width > DIMX) {
    warningMSG("Matrix too big; please change 'DIMH' and/or 'DIMX' in file 'main.h'");
    return ERRORSTRING;
  }
  dim->height = height;   // Store the matrix height
  dim->width = width;     // Store the matrix width
  fgets(chaine, 2, f1);   // Read the carriage return at the end of the second line to reach the beginning of the grid

  char line[dim->width];
  for (i = 0; i < dim->height; i++) {
    if(fgets(line, (dim->width)+2, f1) == NULL) {     // Not enough cells lines in the matrix
      errorMSG("Not enough cell lines in the grid (loadGrid)");
      return ERRORVALUE;
    }
    for (j = 0; j < dim->width; j++) {
      state = line[j];
      if (state == '\n' || state == ' ') {      // Not enough cells in a matrix line
        errorMSG("Not enough cells written in a line of the grid (loadGrid)");
        return ERRORVALUE;
      }
      if (state != 48 && state != 49) {     // ASCII characters of '0' and '1'
        errorMSG("Unknown cell state read in your grid ; waited : 0 / 1 (loadGrid)");
        printf("state = %c ?\n", state);
        return ERRORVALUE;
      }
      (*pgrid)[i][j] = (char) state;
    }
  }
  if(fgets(line, (dim->width)+2, f1) != NULL) {     // Too much cells lines in the matrix
    errorMSG("Too much lines in the grid (loadGrid)");
    return ERRORVALUE;
  }
  fclose(f1);
  return 0;
}




// Create a new file 'filename', and store in it the cells matrix
// Return ERRORVALUE if an error has occured, 0 otherwise
int matrixToText(char* filename, mat mat1, dimensions dim) {
  FILE* fp;
  int i, j;
  char filepath[128] = "grid/";
  strcat(filepath, filename);     // Add the folder before the filename
  fp = fopen(filepath, "a");      // Create the new file
  if (fp == NULL) {
    errorMSG("Error while creating the file in 'matrixToText'");
    return ERRORVALUE;
  }
  fprintf(fp, "# GRID CONWAY'S GAME\n");      // Write the comment on the first line
  fprintf(fp, "%d %d\n", dim.height, dim.width);      // Write the dimensions of the matrix
  for (i = 0; i < dim.height; i++) {
    for (j = 0; j < dim.width; j++) {
      fprintf(fp, "%c", mat1[i][j]);      // Write the cell states in the .txt file
    }
    fprintf(fp, "\n");      // Carriage return at the end of the line
  }
  fclose(fp);
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



// Allocate dynamic memory to the cells matrix, and fill the matrix with zeros
// Return ERRORVALUE if an error has occured
// Return 0 otherwise
int createMatrix(mat* pmat) {
    int i, j;
    *pmat = malloc(DIMH * sizeof *(*pmat));     // Allocate an array of cells lines
    if (*pmat == NULL) {
        errorMSG("Memory allocation for matrix impossible (createMatrix)");
        return ERRORVALUE;
    }
    for (i = 0; i < DIMH; i++) {
      (*pmat)[i] = calloc(DIMX, sizeof *(*pmat)[i]);    // Allocate a cells line
      if ((*pmat)[i] == NULL) {
        errorMSG("Memory allocation for matrix impossible (createMatrix)");
        return ERRORVALUE;
      }
    }
    for (i = 0; i < DIMH; i++) {
      for (j = 0; j < DIMX; j++)
        (*pmat)[i][j] = 48;
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



// Redirects the execution of the program to the appropriate functions, following the user command
// 'bit_load' is equal to 1 if the user has already loaded or created his initial grid in the shell, 0 otherwise
int executecmd(char* cmd, char* filename, mat* mat1, dimensions* dim, int* bit_load) {
  int code;
  char command[128] = "ls grid | grep ";      // UNIX command to find the user initial grid file
  if (strcmp(cmd, "") == 0) {             // Empty command
    return 0;
  }
  if (strcmp(cmd, "exit") == 0) {
    if (*bit_load == 1)
      destroyMatrix(mat1);
    return EXITVALUE;
  }
  else if (strcmp(cmd, "load") == 0) {
    if (*bit_load == 1)
      destroyMatrix(mat1);            // Free the previous matrix
    code = createMatrix(mat1);            // Create the matrix
    if (code < 0)
      return ERRORVALUE;
    strcat(command, filename);
    if (system(command) != 0) {         // Check if the file 'filename' exists in the folder '/grid'
      warningMSG("Initial grid file not found");
      return 0;
    }
    code = loadGrid(filename, mat1, dim);     // load the user grid into the matrix
    if (code < 0)
      return code;                // Return the error code
    *bit_load = 1;              // Grid loaded in the shell
    printMatrix(*mat1, *dim);     // Disp the cells matrix in the shell
    return 0;
  }
  else if (strcmp(cmd, "disp") == 0) {
    if (*bit_load != 1) {                 // If the user didn't load his initial grid before
      warningMSG("Please load an initial grid in the shell before printing it");
      return 0;
    }
    code = dispGrid(*mat1, *dim);   // Disp the cells matrix in a SDL graphic window
    if (code < 0)
      return ERRORVALUE;
    return 0;
  }
  else if (strcmp(cmd, "run") == 0) {
    if (*bit_load != 1) {        // If the user didn't load or create his initial grid before
      warningMSG("Please load an initial grid in the shell before running it");
      return ERRORSTRING;
    }
    code = newMatrix(mat1, *dim);     // Get the matrix of the next cells generation and store it in 'mat1'
    if (code < 0)
      return ERRORVALUE;
    printMatrix(*mat1, *dim);      // Disp the cells matrix in the shell
    return 0;
  }
  else if (strcmp(cmd, "play") == 0) {
    if (*bit_load != 1) {           // No grid previously saved or created
      warningMSG("Please load an initial grid in the shell before running it");
      return ERRORSTRING;
    }
    code = playGame(*mat1, *dim);     // Play with the grid 'mat1' with a graphic display
    system("rm tmp.rle");         // Remove temporary RLE file which contains all the RLE configurations of the previous game
    if (code < 0)
      return ERRORVALUE;
    return 0;
  }
  else if (strcmp(cmd, "convert") == 0) {
    strcat(command, filename);
    if (system(command) != 0) {           // Check wether the file is in the folder '/grid' or not
      warningMSG("Initial grid file not found");
      return ERRORSTRING;
    }
    code = textToRLE(filename);     // Convert the file .txt to RLE file
    if (code < 0)
      return ERRORVALUE;
    return 0;
  }
  else if (strcmp(cmd, "create") == 0) {
    dim->width = 40;
    dim->height = 40;
    if (filename[0] != '\0') {      // User specify a filename
      strcat(command, filename);
      if (system(command) == 0) {           // Verify that the filename specified by the user doesn't already exist
        warningMSG("Please specify another name for your file, because it already exists");
        return ERRORSTRING;
      }
    }
    if (*bit_load == 1)
      destroyMatrix(mat1);       // Free the previous matrix, to erase the previous initial grid
    code = createMatrix(mat1);            // Create the new matrix
    if (code < 0)
      return ERRORVALUE;
    code = initGrid(mat1, *dim);     // Enable user to create a new initial grid through a SDL graphic interface
    if (code < 0)
      return ERRORVALUE;
    *bit_load = 1;          // The new grid is created and saved in the shell
    if (filename[0] != '\0') {          // If the user didn't specify a filename
      code = matrixToText(filename, *mat1, *dim);     // Store the grid in a new .txt file
      if (code < 0)
        return ERRORVALUE;
      infoMSG("Your initial grid has been successfully saved");
    }
    return 0;
  }
  else if (strcmp(cmd, "help") == 0) {
    helpCommand(*bit_load);          // Provide a helpful guide to the user
    return 0;
  }
  else {
    warningMSG("Unknown command");
    return ERRORSTRING;
  }
}



// Recover the command through 'cmd' and the filename (if it exists) through 'filename'
// Return ERRORSTRING if an error occured because of the user
// Return 0 otherwise
int stringStandardise(char* cmd, char* filename) {
  const char* separators = " ";
  char* str;                // Local variable used by strtok
  strcpy(str, cmd);
  char* strtoken = strtok(str, separators);   // Get the user command
  if (strtoken == NULL)       // Empty command
    return ERRORSTRING;
  strcpy(cmd, strtoken);         // Store the user command
  if (strcmp(cmd, "load") == 0 || strcmp(cmd, "convert") == 0 ) {     // Command 'load' and 'convert'
    strtoken = strtok(NULL, separators);   // Get the filename
    if (strtoken == NULL) {                     // No filename specified
      warningMSG("Please, specified a filename");
      return ERRORSTRING;
    }
    strcpy(filename, strtoken);         // Store the filename
    strtoken = strtok(NULL, separators);   // Get the filename
    if (strtoken != NULL) {                     // too much arguments
      warningMSG("Too much arguments");
      return ERRORSTRING;
    }
  }
  else if (strcmp(cmd, "create") == 0) {      // Command 'create'
    strtoken = strtok(NULL, separators);      // Get the filename if it has been specified
    if (strtoken != NULL)
      strcpy(filename, strtoken);         // Store the filename if it has been specified
  }
  else {      // Command 'exit' 'play' 'run' 'disp' 'help'
    strtoken = strtok(NULL, separators);   // Get the filename
    if (strtoken != NULL) {                     // too much arguments
      warningMSG("Too much arguments");
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
    errorMSG("Error while allocating memory for user command (main)");
    exit(EXIT_FAILURE);
  }
  char* filename = calloc(128, sizeof(*filename));      // Dynamic memory for the filename
  if (cmd == NULL) {
    errorMSG("Error while allocating memory for filename (main)");
    exit(EXIT_FAILURE);
  }

  while(1) {
    cmd = readline("Shell : > ");             // Get the user command
    code = stringStandardise(cmd, filename);
    if (code == 0)                                     // If no error has occured
      code = executecmd(cmd, filename, &mat1, &dim, &bit_load);        // Execute the user command
    cmd[0] = '\0';            // Erase the previous command
    filename[0] = '\0';       // Erase the previous filename between each command
    switch(code) {      // Errors handler
      case 0 :          // No error
        break;
      case ERRORSTRING :    // Typo in a user command
        break;
      case EXITVALUE :      // The user asks to quit the shell ; matrix mat1 already free
        free(cmd);              // Free the user command
        free(filename);         // Free the filename
        exit(EXIT_SUCCESS);
      case ERRORVALUE:      // An error has occured in an other function
        puts("Error detected");
        if (bit_load == 1)
          destroyMatrix(&mat1);   // Free the cells matrix
        free(cmd);                  // Free the user command
        free(filename);               // Free the filename
        exit(EXIT_FAILURE);
      default :                 // Error occured in the errors handler
        errorMSG("Unknown return code (error handler)");
        if (bit_load == 1)
          destroyMatrix(&mat1);     // Free the cells matrix
        free(cmd);                  // Free the user command
        free(filename);             // Free the filename
        exit(EXIT_FAILURE);
    }
  }
  puts("Should never be here");
}
