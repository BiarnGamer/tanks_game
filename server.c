/***********************************************************/
/***********************************************************/
/**                                                       **/
/**    Ce fichier gère le serveur de jeu :                **/
/**    - connexion des clients                            **/
/**    - initialisation des clients                       **/
/**    - validation des coordonnées de chaque tank        **/
/**    - récupération du tank de chaque joueur            **/
/**    - envoie du tableau de tank à tous les joueurs     **/
/**    - récupération des obus tiés par chaque joueur     **/
/**    - envoie du tableau des nouveaux obus tirés à      **/
/**      tous les joueurs                                 **/
/**                                                       **/
/***********************************************************/
/***********************************************************/

/********************************/
/**   Inclusion des en-tetes   **/
/********************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine.h"
#include "server.h"
#include "timer.h"
#define ERROR(X) { perror(X); exit(1); }
#define PORT 2048

/**********************************/
/**   Définition des fonctions   **/
/**********************************/
/*
   Initialisation du serveur. Le nombre total de joueur doit etre
   indiqué.  Cette fonction ne retourne que lorsque tous les clients
   sont connectés et que le serveur leur a renvoyé leur numero et le
   nombre de joueurs.

   Données en entrée :
   - nplayers: le nombre de joueurs
   - tanks : le tableau de tanks
   - brouillard : activation du brouillard de gueure (1) ou non (0)
 */
void initServer(int nplayers, tank_t *  tanks, int brouillard) {
    int sock;
    int i;
    int t[3];
    struct sockaddr_in server;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) ERROR("socket");
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) == -1) ERROR("bind");
    if (listen(sock, nplayers) == -1) ERROR("listen");
    clients = (int *)malloc(sizeof(int)*nplayers);

    for (i=0; i<nplayers; i++) {
        if ((clients[i] = accept(sock,0, 0)) == -1) ERROR("accept");
        printf("Player %d connected.\n",i);
    }
    close(sock);
    printf("All connected.\n");

    // Tout le monde est connecté. Maintenant, envoie à chaque client le nombre de joueurs et son numero
    for (i=0; i<nplayers; i++) {
        t[0]=nplayers; // Le nombre de joueurs
        t[1]=i; // Le numero du client
        t[2]=brouillard; // brouillard

        // Envoi le tout (3 entiers)
        write(clients[i],&t,sizeof(int)*3);
    }
}

/*
  Initialisation du jeu : valide la position de chaque tank,
  puis récupère chaque tank avant d'envoyer la liste complète
  à tous les joueurs.
  Comme on récupère les tanks un à un dans l'ordre de connexion des joueurs,
  les premiers arrivés sont prioritaires en cas de conflit sur la position
  (si deux joueurs veulent aller sur la meme case).

  Données en entrée :
  - nplayers: le nombre de joueurs
  - tanks : le tableau de tanks
*/
void initPartie(int nplayers, tank_t *  tanks) {
    // Déclaration des variables
    int i; // compteur de boucle
    int positionValide; // Position valide (1) ou non (0)
    tank_t tank; // Un tank


    // Validation du positionnement des tanks
    for(i=0; i<nplayers; i++) {
        positionValide = 0;
        while(!positionValide) {
            positionValide = validationPosition(i, nplayers, tanks);
            write(clients[i],&positionValide,sizeof(int));
        }

        // Récupération du tank du joueur
        read(clients[i],&tank,sizeof(tank_t)) ;
        tanks[i]=tank;
    }

    // Envoi le tableau de tanks à tous les joueurs
    for(i=0; i<nplayers; i++) {
        write(clients[i],tanks,sizeof(tank_t)*nplayers);
    }
}


/*
  Validation de la position d'un tank :
  - récupère la position demandée
  - vérifie si elle est déjà occupée ou non et retourne le résultat

  Données en entrée :
  - num_joueur : numéro du tank à traiter
  - nplayers: le nombre de joueurs
  - tanks : le tableau de tanks
*/
int validationPosition(int num_joueur, int nplayers, tank_t * tanks) {
    // Déclaration des variables
    int x, y; // Coordonnées demandées
    int i; // Compteur de boucle

    // Réception des coordonnées demandées
    read(clients[num_joueur],&x,sizeof(int)) ;
    read(clients[num_joueur],&y,sizeof(int)) ;

    // Vérification de l'abscence de tank aux coordonnées demandées
    for(i=0; i<nplayers; i++) {
        // Si tank hors-jeu, on l'ignore
        if(tanks[i].etat_tank == 0) {
            continue;
        }

        if((tanks[i].coordonnees_y == y && tanks[i].coordonnees_x == x) || (tanks[i].coordonnees_dest_y == y && tanks[i].coordonnees_dest_x == x) ) {
            return 0;
        }
    }
    return 1;
}

/*
   Envoie les memes données à tous les clients connectés.*

   Données en entrée :
   - data: les données à envoyer
   - size: la taille des données, en octets
   - nplayers: le nombre de joueurs
*/
void broadcast(void *data, size_t size,int nplayers) {
    int i;
    for (i=0; i<nplayers; i++) {
        write(clients[i],data,size);
    }
}

/*
   Recoit des données de tous les joueurs. Ces données sont rangées dans le tableau data
   data: un tableau à une dimension, dont chaque élément est de taille size
   size: la taille d'une donnée envoyée par un client
   nplayers: le nombre de joueurs
*/
void gather(void *data, size_t size,int nplayers) {
    int i;
    for (i=0; i<nplayers; i++) {
        read(clients[i],data+size*i,size) ;
    }
}

/*
   Termine le serveur
*/
void closeServer() {
    free(clients);
}

/*
   À chaque pas de temps, récupère les demandes de déplacement
   des joueurs pour les valider. Récupère en meme temps le tank
   de chaque joueur avant d'envoyer la liste mise à jour à tous
   les joueurs. Enfin, récupère la liste des  nouveaux obus tirés
   pour transmettre l'information à tous les joueurs.

   Données en entrée :
   - nplayers: le nombre de joueurs
   - tanks : le tableau de tanks
*/
void update_serveur(int nplayers, tank_t * tanks) {
    // Déclaration des variables
    int i; // Compteur de boucle
    tank_t tank; // Un tank
    obus_t obus; // Un obus
    bonus_t bonus; // Un bonus
    int deplacement_demande; // Déplacement demandé par le joueur (1) ou non (0)
    int deplacement_autorise; // Déplacement autorisé (1) ou non (0)
    int tir_effectue; // Tir effectue par le joueur (1) ou non (0)
    char * donnees_obus; // Données des nouveaux obus à envoyer : le nombre d'obus à envoyer suivi de chaque obus
    int nb_nouveaux_obus; // Nombre d'obus qui viennent d'etre tirés

    /* Gestion du déplacement de chaque joueur */
    for(i=0; i<nplayers; i++) {
        // Réception de la demande ou non de déplacement
        read(clients[i],&deplacement_demande,sizeof(int)) ;

        // Si déplacement demandé -> validation des coordonnées
        if(deplacement_demande) {
            deplacement_autorise = validationPosition(i, nplayers,tanks);
            write(clients[i],&deplacement_autorise,sizeof(int));
        }

        // Récupération du tank du joueur
        read(clients[i],&tank,sizeof(tank_t)) ;
        tanks[i]=tank;
    }

    // Envoi le tableau de tanks à tous les joueurs
    broadcast(tanks, sizeof(tank_t)*nplayers,nplayers);

    /* Gestion des obus tirés par chaque joueur */
    nb_nouveaux_obus = 0;
    donnees_obus = (char *) malloc(sizeof(int) + sizeof(obus_t)*nplayers);

    // Réception des obus tirés
    for(i=0; i<nplayers; i++) {
        // Réception de la demande ou non de tir
        read(clients[i],&tir_effectue,sizeof(int)) ;

        // Si tir effectué, récupère l'obus tiré
        if(tir_effectue == 1) {
             read(clients[i],&obus,sizeof(obus_t));

             // Ajout de l'obus dans le tableau de nouveaux obus
             memcpy(donnees_obus + sizeof(int) + sizeof(obus_t)*nb_nouveaux_obus, &obus, sizeof(obus_t));
             nb_nouveaux_obus++;
        }
    }

    // Écrit dans donnees_obus le nombre d'obus que l'on envoie
    memcpy(donnees_obus, &nb_nouveaux_obus, sizeof(int));

    // Envoi le tableau de nouveaux obus à tous les joueurs
    broadcast(donnees_obus, sizeof(int) + sizeof(obus_t)*nb_nouveaux_obus,nplayers);

    // Si on doit recevoir un bonus
    if(delais_avant_bonus <= 0){
		read(clients[id_prochain_client_bonus],&bonus,sizeof(bonus_t));
		id_prochain_client_bonus = (id_prochain_client_bonus+1)%nplayers;
		broadcast(&bonus, sizeof(bonus_t),nplayers);

		delais_avant_bonus = DELAIS_ENTRE_BONUS*1000/TICK_INTERVAL -1;
	}
	else {
		delais_avant_bonus --;
	}


    free(donnees_obus);
}

/*
   Fonction principale du serveur
*/
int main(int argc, char *argv[]) {
    // Déclaration des variables
    int finished; // Indique quand le jeu est terminé (1) ou non (0)
    int nplayers; // Nombre de joueurs
    tank_t * tanks; // Tableau de tanks
    int i; // Compteur de boucles
    int brouillard; // Indique si le brouillard de guerre est activé (1) ou non (0)

    // Vérification du nombre d'arguments : il nous faut le nombre de noueurs
    if (argc<2) {
        printf("Le nombre de joueurs doit être indiqué.\nSyntaxe: %s <nombre de joueurs>\n",argv[0]);
        exit(1);
    }

    // Initialisation du serveur
    finished=0; // Le programme n'est pas terminé
    nplayers=atoi(argv[1]); // Récupération du nombre de joueurs

    // Brouillard
    if(argc == 3) {
        brouillard = atoi(argv[2]);
    }
    else {
        brouillard = 0;
    }

    // Initialisation du tableau de tanks
    tanks =(tank_t *) malloc(sizeof(tank_t)*nplayers);

    // Au départ, on ne connait pas les coordonnées des joueurs, on les initialise à
    // -1 pour etre certains de n'avoir aucune position prise sans le vouloir
    for(i=0; i<nplayers; i++) {
        tanks[i].coordonnees_x=-1;
        tanks[i].coordonnees_y=-1;
    }

    // Initialisation de la partie
    initServer(nplayers, tanks, brouillard); // Connexion des joueurs
    initPartie(nplayers, tanks); // Position de départ des tanks

    delais_avant_bonus = 0;
    nombre_bonus = 0;
    id_prochain_client_bonus = 0;

    // Boucle principale
    while (!finished) { // Boucle principale du jeu : boucle infinie
        update_serveur(nplayers, tanks);
    }

    closeServer();
    return 0;
}
