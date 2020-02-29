
----- UTILISATION -----
Les grilles initiales sont placées dans grid/ au format .txt
Un commentaire sur la première ligne de ce fichier
Les cellules sont représentées par une matrice d'états binaires : 1 <=> vivant et 0 <=> mort

Dans l'interpréteur :
load <file.txt>    pour charger la grille initiale dans l'interpréteur

disp               pour afficher la grille sous format graphique

run                pour appliquer la règle sur une génération (sans affichage graphique)

play               pour jouer la grille préalablement chargée selon la règle commune sur N générations (avec affichage graphique)
                   La macro N est spécifiée dans 'affichage.h'

help               pour obtenir de l'aide


----- PROCHAINES MISES A JOUR -----
La résolution (et les dimensions) de la fenêtre graphique s'adapteront aux dimensions de la matrice
Les grilles initiales pourront être au format RLE, format plus compact
Ajout de règles supplémentaires
Arrêt de l'exécution lorsque toutes les cellules sont mortes, ou qu'une configuration stable est obtenue
Implantation de structures usuelles (canons, planeurs, ...)


----- GRANDES AMBITIONS -----
Détection d'un jardin d'Eden de Conway
Implantation de portes logiques...
