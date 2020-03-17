
----- UTILISATION -----
Les grilles initiales sont placées dans grid/ au format .txt
Un commentaire sur la première ligne de ce fichier
Les dimensions de la grille en nombre de cellules sur la deuxième ligne
Les cellules sont représentées par une matrice d'états binaires : 1 <=> vivant et 0 <=> mort

Dans l'interpréteur :
load <file.txt>       pour charger la grille initiale dans l'interpréteur
                      Ce fichier .txt doit être contenu dans le dossier grid/

disp                  pour afficher la grille sous format graphique (statique)

run                   pour appliquer la règle sur une génération (sans affichage graphique)

play                  pour jouer la grille préalablement chargée selon la règle commune (avec affichage graphique)
                      Ne s'arrête que lorsque la grille est vide

convert <file.txt>    pour convertir un fichier .txt contenant une grille initale en un fichier .rle
                      Ce fichier .txt doit être contenu dans le dossier grid/

exit                  pour quitter l'interpréteur de commande

help                  pour obtenir de l'aide



----- PROCHAINES MISES A JOUR -----
La résolution (et les dimensions) de la fenêtre graphique s'adapteront aux dimensions de la matrice
Les grilles initiales pourront être au format RLE, format plus compact
Ajout de règles supplémentaires
Implantation de structures usuelles (canons, planeurs, ...)
Détecteur de structures oscillantes


----- GRANDES AMBITIONS -----
Détection d'un jardin d'Eden de Conway
Implantation de portes logiques...
