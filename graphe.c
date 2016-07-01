#include "engine.h"
#include "graphe.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/*
   Retourne la position d'un noeud dans le graphe.
   Retourne -1 s'il n'y appartient pas.
   Un noeud est identifié par ses coordonnées.
*/
int rechercheNoeud(graphe_t * g, int x, int y) {
    // Initialisation des variables
    int i;

    // Recherche du noeud dans le graphe
    for(i=0; i<g->nb_noeuds; i++) {
        // S'il a été trouvé, retourne la position du noeud dans le graphe
        if(g->noeuds[i].x == x && g->noeuds[i].y == y) {
            return i;
        }
    }

    // Si le noeud n'appartient pas au graphe, retourne -1
    return -1;
}

/*
   Génération d'un graphe à partir du plateau de jeu.
*/
void generation_graphe(map_t * m, graphe_t * g) {
    // Déclaration des variables
    int i, j; // Compteurs de boucle
    int x, y; // Coordonnées d'un noeud (position de la largteurcase sur le plateau
    noeud_t n; // Un noeud
    voisin_t voisins[4]; // Tableau de noeuds voisin : on l'alloue ici car 4 voisins max par noeud --> gain de temps
    voisin_t v; // Un noeud voisin
    int cout; // Cout du passage d'un noeud à l'autre

    // Initiilisation du graphe
    g->noeuds = malloc(sizeof(noeud_t) * m->largeur * m->hauteur); // nombre max de noeuds
    g->nb_noeuds = 0;

    // Liste des noeuds
    for(i=0; i<m->hauteur; i++) {
        for(j=0; j<m->largeur; j++) {
           // printf("%d[%d] ", m->plateau[i][j].type_case, contraintes[m->plateau[i][j].type_case][NP]);
             // Si case franchissable --> TEST À REVOIR PLUS TARD !!!!!!
             // Alors on ajoute le noeud au graphe
            if(!contraintes[m->plateau[i][j].type_case][NP]) {
                n.x = i;
                n.y = j;
                n.nb_voisins=0;
                n.id_graphe_partiel=-1;
                g->noeuds[g->nb_noeuds++] = n;
            }
        }
    }

    // Liste des voisins
    for(i=0; i<g->nb_noeuds; i++) {
        x = g->noeuds[i].x;
        y = g->noeuds[i].y;
        g->noeuds[i].nb_voisins = 0;
        cout = contraintes[m->plateau[x][y].type_case][V];

         // Si case à cté franchissable --> TEST À REVOIR PLUS TARD !!!!!!
         // Alors on ajoute l'arc au graphe
        if(x-1 >= 0 && !contraintes[m->plateau[x-1][y].type_case][NP]) {
            v.cout = cout;
            v.noeud = rechercheNoeud(g, x-1, y);
            voisins[g->noeuds[i].nb_voisins++] = v;
        }

        if(x+1 < m->hauteur && !contraintes[m->plateau[x+1][y].type_case][NP]) {
            v.cout = cout;
            v.noeud = rechercheNoeud(g, x+1, y);
            voisins[g->noeuds[i].nb_voisins++] = v;
        }

        if(y-1 >= 0 && !contraintes[m->plateau[x][y-1].type_case][NP]) {
            v.cout = cout;
            v.noeud = rechercheNoeud(g, x, y-1);
            voisins[g->noeuds[i].nb_voisins++] = v;
        }

        if(y+1 < m->largeur && !contraintes[m->plateau[x][y+1].type_case][NP]) {
            v.cout = cout;
            v.noeud = rechercheNoeud(g, x, y+1);
            voisins[g->noeuds[i].nb_voisins++] = v;
        }

        g->noeuds[i].liste_voisins = malloc(sizeof(voisin_t) * g->noeuds[i].nb_voisins);

        memcpy(g->noeuds[i].liste_voisins, voisins, sizeof(voisin_t) * g->noeuds[i].nb_voisins);

    }
    tableau_distances = malloc(sizeof(int) * g->nb_noeuds);
    tableau_predecesseurs = malloc(sizeof(int) * g->nb_noeuds);
    id_graphe_partiel=0;
}

/*
   Retourne le cout pour aller d'un noeud à un autre.
   Cout de l'arc entre src et dst s'il exite.
   + infini sinon.
*/
int cout(graphe_t * g, int src, int dst) {
    int i;
    for(i=0; i<g->noeuds[src].nb_voisins; i++) {
        if(g->noeuds[src].liste_voisins[i].noeud == dst) {
            return g->noeuds[src].liste_voisins[i].cout;
        }
    }
    return INT_MAX; // INT_MAX == cout infini
}

/*
   Retourne la position du noeud non traité le plus proche du noeud source.
   On parcours le tableau des distances à la recherche du noeud à la distance
   minimale.
*/
int noeud_plus_proche(graphe_t * g) {
    // Déclaration des variables
    int i, min, rang_min;
    rang_min = 0;
    min = INT_MAX;

    for(i=0; i<g->nb_noeuds; i++) {
        // Si noeud pas traité et plus proche
        if(g->noeuds[i].id_graphe_partiel != id_graphe_partiel && tableau_distances[i] < min) {
            min = tableau_distances[i];
            rang_min = i;
        }
    }
    return rang_min;
}


/*
   Indique les coordonnées de la prochaine case où aller pour se rapprocher de la cibe.
   Recherche du meilleur chemin
*/
void prochaineCase(graphe_t * g, map_t * m, int x_src, int y_src, int x_dst, int y_dst, int * x, int * y) {
    int i;
    int num_src = rechercheNoeud(g, x_src, y_src);
    int num_dst = rechercheNoeud(g, x_dst, y_dst);
//printf("SRC : %d (%d ; %d) -- DST : %d (%d ; %d)\n", num_src, x_src, y_src, num_dst, x_dst, y_dst);
    int noeud;
    int num_noeud_plus_proche;
    int nb_noeuds_restants = g->nb_noeuds-1;
    int cout_noeud;
    int nouveau_cout;

    // Initialisation
    // Tableau des distances et des prédécesseurs
    for(i=0; i<g->nb_noeuds; i++) {
        tableau_distances[i] = cout(g, num_src, i);
        tableau_predecesseurs[i] = num_src;
    }

    // Noeud source traité
    g->noeuds[num_src].id_graphe_partiel=id_graphe_partiel;

    // Traite le reste
    while(nb_noeuds_restants > 0) {
        num_noeud_plus_proche = noeud_plus_proche(g);
        // Si distance mini est infinie -> stop
        if(tableau_distances[num_noeud_plus_proche] == INT_MAX) {
            break;
        }

        // On considère le sommet trouvé comme étant traité
        g->noeuds[num_noeud_plus_proche].id_graphe_partiel = id_graphe_partiel;
        // On parcours ses voisins pour voir s'il est le meilleur prédécesseurs
         for(i=0; i<g->noeuds[num_noeud_plus_proche].nb_voisins; i++) {
            nouveau_cout = g->noeuds[num_noeud_plus_proche].liste_voisins[i].cout + tableau_distances[num_noeud_plus_proche] ;
            cout_noeud = tableau_distances[g->noeuds[num_noeud_plus_proche].liste_voisins[i].noeud];

            if(nouveau_cout < cout_noeud) {
                tableau_distances[g->noeuds[num_noeud_plus_proche].liste_voisins[i].noeud] = nouveau_cout;
                tableau_predecesseurs[g->noeuds[num_noeud_plus_proche].liste_voisins[i].noeud] = num_noeud_plus_proche;
            }
            else {
            }
         }

         nb_noeuds_restants--;
    }
    // À présent, dans tableau_predecesseurs, on a une solution optimale pour aller de la source vers la destination
    if(tableau_distances[num_dst] == INT_MAX) {
        *x = x_src;
        *y = y_src;
    }
    else {
        noeud = num_dst;
        while(tableau_predecesseurs[noeud] != num_src) {
          //  printf("(%d ; %d) -> (%d ; %d)\n", g->noeuds[tableau_predecesseurs[noeud]].x, g->noeuds[tableau_predecesseurs[noeud]].y, g->noeuds[noeud].x, g->noeuds[noeud].y);
            noeud = tableau_predecesseurs[noeud];

        }
        *x = g->noeuds[noeud].x;
        *y = g->noeuds[noeud].y;
    }

    id_graphe_partiel = (id_graphe_partiel + 1) % INT_MAX;
}

/*
   Libération de la mémoire lorsque le graphe n'est plus utile
*/
void close_graphe() {
    free(tableau_distances);
    free(tableau_predecesseurs);
}
