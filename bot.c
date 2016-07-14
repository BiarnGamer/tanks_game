#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <math.h>
#include "engine.h"
#include "graphics.h"
#include "graphe.h"
#include "timer.h"
#include "bot.h"
#include "client.h"
#define MAP "data/map.bmp"
#include <time.h>

  

/**
 Le bot choisi une cible  :
 - joueur le plus proche
 - joueur au hasard
 - case au hasard

 (tirage aléatoire pour savoir quelle décision on prend)

 Puis il garde cette cible un certains nombre de tours avant d'en changer.
 Ce nombre de tour est compris entre NB_TOUR_AVANT_DECISION / 2 et NB_TOUR_AVANT_DECISION * 2

 Le bot prend ensuite le chemin le plus court pour atteindre sa cible, il fonce droit sur elle.

 Le bot tire autant qu'il peut.

 Note : on fait quoi quand cible atteinte ??

**/


void cible_proche(map_t * m, bot_t * bots, int num_bot) {
    // Recherche du tank le plus proche
    int id_min;
    int i;
    int x_bot;
    int y_bot;
    float dist_min;
    int x, y, x_min, y_min;
    float dist;

    x_bot = m->tanks[bots[num_bot].num_bot].coordonnees_dest_y;
    y_bot = m->tanks[bots[num_bot].num_bot].coordonnees_dest_x;
    x_min = m->tanks[0].coordonnees_dest_x;
    y_min = m->tanks[0].coordonnees_dest_y;
    dist_min = sqrt( pow(x_bot - x_min,2) + pow(y_bot - y_min,2) );
    id_min = 0;

    for(i=1; i<m->nombre_tank_actif; i++) {
        x=m->tanks[i].coordonnees_dest_x;
        y=m->tanks[i].coordonnees_dest_y;
        dist = sqrt( pow(x_bot - x,2) + pow(y_bot - y,2) );
        // On ne choisi pas une cible de notre équipe
        if(dist < dist_min && m->tanks[i].equipe != m->tanks[bots[num_bot].num_bot].equipe) {
            dist_min = dist;
            id_min = i;
        }
    }

    // Enregistre la cible
    bots[num_bot].cible_x = m->tanks[id_min].coordonnees_dest_y;
    bots[num_bot].cible_y = m->tanks[id_min].coordonnees_dest_x;
    bots[num_bot].id_cible = id_min;
}

void cible_aleat(map_t * m, bot_t * bots, int num_bot) {
    // Choix d'une cible
    int id_cible;
    do {
        id_cible = rand()%m->nombre_tank_actif;
    } while(id_cible == bots[num_bot].num_bot || m->tanks[id_cible].equipe == m->tanks[bots[num_bot].num_bot].equipe); // On ne choisi pas une cible de notre équipe

    // Enregistre la cible
    bots[num_bot].cible_x = m->tanks[id_cible].coordonnees_dest_y;
    bots[num_bot].cible_y = m->tanks[id_cible].coordonnees_dest_x;
    bots[num_bot].id_cible = id_cible;
}

void case_aleat(map_t * m, bot_t * bots, int num_bot) {
    // Choix d'une case franchissable
    int x, y;
    do {
        x = rand()%m->hauteur;
        y = rand()%m->largeur;
    } while(contraintes[m->plateau[x][y].type_case][NP]);

    // Enregistre la cible
    bots[num_bot].cible_x = x;
    bots[num_bot].cible_y = y;
    bots[num_bot].id_cible = -1;
}

int choix_cible(map_t * m, bot_t * bots, int num_bot) {

    // Si cible atteinte
    if(bots[num_bot].cible_x == m->tanks[m->num_tank_joueur].coordonnees_y && bots[num_bot].cible_y == m->tanks[m->num_tank_joueur].coordonnees_x) {
        bots[num_bot].nb_pas_avant_decision = 0;
    }

    // S'il n'y a pas de décision à prendre, on met à jour la position à atteindre et on quitte
    if(bots[num_bot].nb_pas_avant_decision) {
        // MàJ coordonnées
        if(bots[num_bot].id_cible != -1) {
            bots[num_bot].cible_x = m->tanks[bots[num_bot].id_cible].coordonnees_dest_y;
            bots[num_bot].cible_y = m->tanks[bots[num_bot].id_cible].coordonnees_dest_x;
        }

        bots[num_bot].nb_pas_avant_decision -= 1;
        return 0;
    }

    // Sinon, choix de la cible : la plus proche, au hasard ou juste d'une position aléatoire
    int type_decision = rand()%NB_TYPES_DECISION;
    switch(type_decision) {
    case PLUS_PROCHE:
        if(m->nombre_tank_actif > 1) {
            cible_proche(m, bots, num_bot);
        }
        else {
            case_aleat(m, bots, num_bot);
        }
        break;
    case TANK_ALEATOIRE:
        if(m->nombre_tank_actif > 1) {
            cible_aleat(m, bots, num_bot);
        }
        else {
            case_aleat(m, bots, num_bot);
        }

        break;
    case CASE_ALEATOIRE:
        case_aleat(m, bots, num_bot);
        break;
    }

    bots[num_bot].nb_pas_avant_decision = rand()%NB_TOUR_AVANT_DECISION + NB_TOUR_AVANT_DECISION/2;
//printf("Cible : %d (%d ; %d) [%d]\n", bots[num_bot].id_cible, bots[num_bot].cible_x, bots[num_bot].cible_y, bots[num_bot].nb_pas_avant_decision);
    return 0;
}

void deplacement_bot(map_t *m, bot_t * bots, graphe_t * g, int num_bot) {
    int x,y; // Coordonnées de la nouvelle case
    // On tente de se déplacer vers notre cible
    if(m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement == 0) {
        m->code_deplacement_demande = 1;
        // Calcul du meilleur chemin et choix de la nouvelle case
        prochaineCase(g, m, m->tanks[m->num_tank_joueur].coordonnees_y, m->tanks[m->num_tank_joueur].coordonnees_x, bots[num_bot].cible_x, bots[num_bot].cible_y, &x, &y);

        // Calcul de la direction du déplacement
        // Gauche
        if(m->tanks[m->num_tank_joueur].coordonnees_x - y == 1) {
            m->code_deplacement_demande = SDLK_LEFT;
        }
        // Droite
        else if(m->tanks[m->num_tank_joueur].coordonnees_x - y == -1) {
            m->code_deplacement_demande = SDLK_RIGHT;
        }
        // Haut
        else if(m->tanks[m->num_tank_joueur].coordonnees_y - x == 1) {
            m->code_deplacement_demande = SDLK_UP;
        }
        // Bas
        else if(m->tanks[m->num_tank_joueur].coordonnees_y - x == -1) {
            m->code_deplacement_demande = SDLK_DOWN;
        }
        // Rien
        else {
            m->code_deplacement_demande = 0;
        }
    }
    else {
        m->code_deplacement_demande = 0;
    }
}

void update_bot(map_t *m, int sock, bot_t * bots, graphe_t * g, int num_bot) {

    char * donnees_obus;
    int demande_tir; // Indique si un tir a été effectué (1) ou non (0)
    int nombre_nouveaux_obus;
    obus_t obus;
    obus_t * tab_obus_tmp;
    bonus_t bonus;


    /* Gestion du déplacement du joueur */
    // Si déplacement possible

    // Choix d'une cible
    choix_cible(m, bots, num_bot);

    // Demande de déplacement
    deplacement_bot(m, bots, g, num_bot);

    // Validation du déplacement
    gestion_deplacements(m, sock);


    // On envoie notre strucure tank au serveur
    // Envoie
    tank_t tank = m->tanks[m->num_tank_joueur];
    sendData(sock,&tank, sizeof(tank_t));

    // Attente de la réponse : le serveur nous envoie le tableau de tanks
    tank_t * tanks = (tank_t *) malloc(sizeof(tank_t)*m->nombre_tank_actif);
    receiveData(sock,tanks, sizeof(tank_t)*m->nombre_tank_actif);
    memcpy(m->tanks, tanks,  sizeof(tank_t)*m->nombre_tank_actif);
    free(tanks);

    /* Gestion des obus */
    // Gestion du tir d'un obus
    // Si un obus a été créé, on l'envoie au serveur
    m->tir_demande=1;
    demande_tir = gestion_tir(m, &obus);
    if(demande_tir) {
        donnees_obus = (char *) malloc(sizeof(int) + sizeof(obus_t));
        memcpy(donnees_obus, &demande_tir, sizeof(int));
        memcpy(donnees_obus + sizeof(int), &obus, sizeof(obus_t));

        sendData(sock,donnees_obus, sizeof(int) + sizeof(obus_t));

        free(donnees_obus);
    }
    // Sinon, on indique au serveur qu'aucun obus n'a été créé
    else {
        sendData(sock,&demande_tir, sizeof(int));
    }

    // Réception des nouveaux obus
    // Reçoit le nombre de nouvel obus
    receiveData(sock,&nombre_nouveaux_obus, sizeof(int));

    // Reçoit les nouveaux obus
    // Si tableau plein
    if(m->nombre_obus + nombre_nouveaux_obus > m->taille_tableau_obus) {
        m->taille_tableau_obus += nombre_nouveaux_obus + 5;
        tab_obus_tmp = (obus_t *) malloc( m->taille_tableau_obus *  sizeof(obus_t));
        memcpy(tab_obus_tmp, m->tab_obus, m->nombre_obus *  sizeof(obus_t));
        free(m->tab_obus);
        m->tab_obus = tab_obus_tmp;
    }

    // Copie les nouveaux obus à la fin du tableau d'obus
    receiveData(sock,m->tab_obus + m->nombre_obus , sizeof(obus_t)*nombre_nouveaux_obus);
    m->nombre_obus += nombre_nouveaux_obus;


    // Gestion obus
    gestion_deplacements_obus(m);

	/* Gestion des bonus */
    // Ramassage bonus
    gestion_bonus(m);

    //Génération d'un bonus
    generation_bonus(m, sock);

    // Réception bonus
    if(m->delais_avant_reception_bonus <= 0) {

        receiveData(sock,&bonus, sizeof(bonus_t));

        if(m->nb_bonus < NOMBRE_MAX_BONUS) {
            m->tab_bonus[m->nb_bonus++]=bonus;
        }

        m->delais_avant_reception_bonus = (DELAIS_ENTRE_BONUS*1000/TICK_INTERVAL) - 1;

    }
    else  {
        m->delais_avant_reception_bonus--;
    }

}


int main(int argc, char *argv[]) {

    if (argc<2) {
        printf("L'adresse du serveur doit être indiqué.\nSyntaxe: %s <IP serveur> \n",argv[0]);
        exit(1);
    }

    // Initialisations
    srand(time(NULL)); // initialisation de rand
    map_t *m; // Carte du jeu
    int finished=0; // Programme se termine quand appuie sur la croix ou appuie sur échap
    init_tableau_contraintes(); // Initialisation tableau des contraintes
    init_tableau_char_carac();// Initialisation du tableau de caractéritiques des tanks
    int nombre_bots = 1;//atoi(argv[2]);

    // Chargement de la carte
    m=loadMap(MAP); // Charge la carte

    // Connexion au serveur, pour récupérer numéro du joueur et nombre de joueur
    int id, nb, b;
    int i;
    int sock;
    bot_t bots[nombre_bots];


    for(i=0; i<nombre_bots; i++) {
        bots[i].nb_pas_avant_decision=0;
        sock=openConnection(argv[1], &id ,&nb, &b);
        bots[i].sock = sock;
        bots[i].num_bot = id;

        if(sock > 0) {
            printf("Connexion réussi.\nNombre de joueurs : %d\nNuméro : %d\n", nb, id);
        }
        else {
            perror("Échec de connexion:");
            exit(1);
        }

       // printf("Bot[%d] - Num : %d - Sock : %d\n", i, bots[i].num_bot, bots[i].sock );
    }
    m->num_tank_joueur=id;
    m->nombre_tank_actif=nb; // Nombre de tanks actifs
    m->brouillard=b; // Brouillard

    /** APRÈS AVOIR RÉCUPÉRÉ LES INFOS SUR LE SEVREUR **/
    init_jeu(m, sock); // Initialisation des tanks
    timerInit(); // Initialise le timer pour temporiser le jeu : on va avancer pas par pas



// Génération du graphe
graphe_t g;

generation_graphe(m, &g);


/*
int a, b;
printf("digraph G {\nedge [color=brown, arrowsize=1];\nnode [color=lightyellow2, style=filled];\n");
for(a=0; a<g.nb_noeuds; a++) {
    for(b=0; b<g.noeuds[a].nb_voisins; b++) {
        printf("\"(%d ; %d)\" -> \"(%d ; %d)\" [label=\"%d\"];\n", g.noeuds[a].x, g.noeuds[a].y, g.noeuds[g.noeuds[a].liste_voisins[b].noeud].x, g.noeuds[g.noeuds[a].liste_voisins[b].noeud].y, g.noeuds[a].liste_voisins[b].cout);
    }
}
printf("}\n");*/

    // Boucle principale
    while (!finished) { // Boucle principale du jeu : boucle infinie
        for(i=0; i<nombre_bots; i++) {
            m->num_tank_joueur = bots[i].num_bot;
            finished=getEvent(m); // Get event capture les évenements comme les touches clavier
            update_bot(m, bots[i].sock, bots, &g, i); // Gestion des déplacements : fonction du moteur du jeu, pas graphique
        }

        // fprintf(stderr,".\n");
        timerWait(); // On attend une certaine durée jusqu'au prochain pas de temps
    }

    close_graphe();

    return 0;
}
