
----- USE -----

The initial grids are placed in the folder '/grid' in '.txt' format
On the first line, a comment to specify the nature of the file
On the second line, the grid dimensions in number of cells

Cells are represented by a matrix of binary states: 1 <=> alive and 0 <=> dead

This projet tries to adopt as much as possible a modular achitecture


Commands avalaible in the shell :

load <file.txt>         to load the initial grid in the interpreter
                        This '.txt' file must be contained in the folder '/grid'

disp                    to display the grid through a SDL graphic window (static)

run                     to apply the rule on a generation (without graphic display)

play                    to play the grid previously loaded according to the common rule (with SDL graphic display)
                        It stops when the grid is empty or when it has reached a maximum number of generations

create                  to create an initial grid through a graphic interface
                        At the end, the grid is automatically memorize in the shell

convert <file.txt>      to convert a '.txt' file containing an initial grid to a '.rle' file
                        This '.txt' file must be contained in the folder '/grid'

exit                    to quit the shell

help                    Provide a helpful guide




----- NEXT UPDATES -----

The resolution (and dimensions) of the graphics window will be automatically adapted to the dimensions of the matrix
The initial grids could be in RLE format, more compact format
Addition of new gaming rules
Initial grids created through the graphic interface will create a '.txt' file automatically
Dimensions of the grid can be chosen when using the graphic interface
A validation button will be added to improve the graphic interface




----- GREAT AMBITIONS -----

Detection of 'Jardin d'Eden de Conway'
Implementation of logic gates ...
