/****************************************************************************/
/****************************************************************************/
/**                                                                        **/
/**    Ce fichier gère le moteur de jeu :                                  **/
/**    - caractéristiques des cases du plateau                             **/
/**    - caractéristiques des tanks                                        **/
/**    - gestion des événements clavier                                    **/
/**    - gestion des déplacements (vérifications et demande au serveur)    **/
/**    - initialisation du tank du joueur                                  **/
/**    - envoie du tank au serveur à chaque tour                           **/
/**    - réception de l'ensemble des tanks à chaque tour                   **/
/**                                                                        **/
/****************************************************************************/
/****************************************************************************/

/********************************/
/**   Inclusion des en-tetes   **/
/********************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "engine.h"
#include "graphics.h"
#include "client.h"
#include "timer.h"
#define SIZE 32


/**********************************/
/**   Définition des fonctions   **/
/**********************************/

/*
   Initialisation du tableau contenant les caractéristiques de
   chaque type de case
*/
void init_tableau_contraintes() {
    contraintes[FORET][NP] = 0;
    contraintes[FORET][NPL] = 0;
    contraintes[FORET][AT] = 1;
    contraintes[FORET][D] = 0;
    contraintes[FORET][V] = TAUX_RALENTISSEMENT;
	contraintes[FORET][NPCNA] = 0;

    contraintes[MAISON_BOIS][NP] = 1;
    contraintes[MAISON_BOIS][NPL] = 1;
    contraintes[MAISON_BOIS][AT] = 1;
    contraintes[MAISON_BOIS][D] = 1;
    contraintes[MAISON_BOIS][V] = TAUX_NORMAL;
	contraintes[MAISON_BOIS][NPCNA] = 1;

    contraintes[MAISON_PIERRE][NP] = 1;
    contraintes[MAISON_PIERRE][NPL] = 1;
    contraintes[MAISON_PIERRE][AT] = 1;
    contraintes[MAISON_PIERRE][D] = 0;
    contraintes[MAISON_PIERRE][V] = TAUX_NORMAL;
	contraintes[MAISON_PIERRE][NPCNA] = 1;

    contraintes[RIVIERE][NP] = 0;
    contraintes[RIVIERE][NPL] = 0;
    contraintes[RIVIERE][AT] = 0;
    contraintes[RIVIERE][D] = 0;
    contraintes[RIVIERE][V] = TAUX_NORMAL;
	contraintes[RIVIERE][NPCNA] = 1;

    contraintes[ROUTE][NP] = 0;
    contraintes[ROUTE][NPL] = 0;
    contraintes[ROUTE][AT] = 0;
    contraintes[ROUTE][D] = 0;
    contraintes[ROUTE][V] = TAUX_ACCELERATION;
	contraintes[ROUTE][NPCNA] = 0;

    contraintes[DEBRIS][NP] = 0;
    contraintes[DEBRIS][NPL] = 1;
    contraintes[DEBRIS][AT] = 0;
    contraintes[DEBRIS][D] = 0;
    contraintes[DEBRIS][V] = TAUX_RALENTISSEMENT;
	contraintes[DEBRIS][NPCNA] = 0;

    contraintes[PONT][NP] = 0;
    contraintes[PONT][NPL] = 0;
    contraintes[PONT][AT] = 0;
    contraintes[PONT][D] = 0;
    contraintes[PONT][V] = TAUX_ACCELERATION;
	contraintes[PONT][NPCNA] = 0;

    contraintes[PONT2][NP] = 0;
    contraintes[PONT2][NPL] = 0;
    contraintes[PONT2][AT] = 0;
    contraintes[PONT2][D] = 0;
    contraintes[PONT2][V] = TAUX_ACCELERATION;
	contraintes[PONT2][NPCNA] = 0;

    contraintes[HERBE][NP] = 0;
    contraintes[HERBE][NPL] = 0;
    contraintes[HERBE][AT] = 0;
    contraintes[HERBE][D] = 0;
    contraintes[HERBE][V] = TAUX_NORMAL;
	contraintes[HERBE][NPCNA] = 0;

    contraintes[DEBRIS][NP] = 0;
    contraintes[DEBRIS][NPL] = 1;
    contraintes[DEBRIS][AT] = 0;
    contraintes[DEBRIS][D] = 0;
    contraintes[DEBRIS][V] = TAUX_RALENTISSEMENT;
	contraintes[DEBRIS][NPCNA] = 0;

    contraintes[MONTAGNE][NP] = 1;
    contraintes[MONTAGNE][NPL] = 1;
    contraintes[MONTAGNE][AT] = 1;
    contraintes[MONTAGNE][D] = 0;
    contraintes[MONTAGNE][V] = TAUX_NORMAL;
	contraintes[MONTAGNE][NPCNA] = 1;
}

/*
   Initialisation du tableau contenant les caractéristiques de
   chaque type de tank
*/
void init_tableau_char_carac() {
    carac_tank[TANK_LEGER][PT]=25;
    carac_tank[TANK_LEGER][BAV]=2;
    carac_tank[TANK_LEGER][BL]=1;
    carac_tank[TANK_LEGER][BAR]=1;
    carac_tank[TANK_LEGER][DELAIS_DEPLACEMENT]=4;
    carac_tank[TANK_LEGER][DELAIS_TIR]=20;
    carac_tank[TANK_LEGER][PORTEE_VISION]=8;
    carac_tank[TANK_LEGER][PORTEE_MAX_TIR]=20;

    carac_tank[TANK_MOYEN][PT]=50;
    carac_tank[TANK_MOYEN][BAV]=4;
    carac_tank[TANK_MOYEN][BL]=2;
    carac_tank[TANK_MOYEN][BAR]=2;
    carac_tank[TANK_MOYEN][DELAIS_DEPLACEMENT]=6;
    carac_tank[TANK_MOYEN][DELAIS_TIR]=40;
    carac_tank[TANK_MOYEN][PORTEE_VISION]=4;
    carac_tank[TANK_MOYEN][PORTEE_MAX_TIR]=20;

    carac_tank[TANK_LOURD][PT]=75;
    carac_tank[TANK_LOURD][BAV]=10;
    carac_tank[TANK_LOURD][BL]=6;
    carac_tank[TANK_LOURD][BAR]=2;
    carac_tank[TANK_LOURD][DELAIS_DEPLACEMENT]=8;
    carac_tank[TANK_LOURD][DELAIS_TIR]=80;
    carac_tank[TANK_LOURD][PORTEE_VISION]=3;
    carac_tank[TANK_LOURD][PORTEE_MAX_TIR]=20;
	
	carac_tank[TANK_AMPHIBIE][PT]=50;
    carac_tank[TANK_AMPHIBIE][BAV]=2;
    carac_tank[TANK_AMPHIBIE][BL]=1;
    carac_tank[TANK_AMPHIBIE][BAR]=1;
    carac_tank[TANK_AMPHIBIE][DELAIS_DEPLACEMENT]=6;
    carac_tank[TANK_AMPHIBIE][DELAIS_TIR]=80;
    carac_tank[TANK_AMPHIBIE][PORTEE_VISION]=10;
    carac_tank[TANK_AMPHIBIE][PORTEE_MAX_TIR]=20;
    
}

/*
   Capture les evenements clavier/fenetre.
   Retourne 1 si il faut quitter le jeu, 0 sinon.
*/
int getEvent(map_t *m) {
    SDL_Event event;

    // Ecoute les événements qui sont arrivés
    while(SDL_PollEvent(&event) ) {
        // On a fermé la fenetre -> quitter le jeu
        if (event.type==SDL_QUIT) return 1;
    }

    // Récupération de l'état du clavier pour savoir si une touche
    // directionnelle est active --> demande de déplacement
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_LEFT]) {
        m->code_deplacement_demande = SDLK_LEFT;
    }
    if (keystate[SDL_SCANCODE_UP]) {
        m->code_deplacement_demande = SDLK_UP;
    }
    if (keystate[SDL_SCANCODE_DOWN]) {
        m->code_deplacement_demande = SDLK_DOWN;
    }
    if (keystate[SDL_SCANCODE_RIGHT]) {
        m->code_deplacement_demande = SDLK_RIGHT;
    }
    if (keystate[SDL_SCANCODE_ESCAPE]) {
        return -1;
    }
    if (keystate[SDL_SCANCODE_SPACE]) {
        m->tir_demande=1;
    }

    return 0;
}

/*
   Validation des coordonnées demandées par le joueur.
   Validation faite en local par le moteur de jeu, il
   faut ensuite demander l'autorisation au serveur de jeu.
   Retourne 1 en cas de validation et 0 en cas de refus.
*/
int validationCoordonnees(map_t *m, int x, int y, int rotation) {
    // Si on n'a pas terminé le déplacement en cours
    if(m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement) {
        return 0;
    }

    // Nouvelle case hors de la carte
    if(x<0 || x >= m->largeur || y < 0 || y >= m->hauteur) {
        return 0;
    }

    // Nouvelle case non franchissable
    if(contraintes[m->plateau[y][x].type_case][NP] == 1) {
        return 0;
    }

    // Nouvelle case non franchissable pour les chars légers et on a un char léger
    if(m->tanks[m->num_tank_joueur].type_tank == TANK_LEGER && contraintes[m->plateau[y][x].type_case][NPL] == 1) {
        return 0;
    }

    // Si la direction du déplacement est différente de celle du tank
    if(rotation != m->tanks[m->num_tank_joueur].rotation) {
        return 0;
    }
    
    //case franchissable que par le char amphibie
	if(m->tanks[m->num_tank_joueur].type_tank != TANK_AMPHIBIE && contraintes[m->plateau[y][x].type_case][NPCNA] == 1) {
        return 0;
    }

    return 1;
}

/*
   Initialisation tank du joueur :
   - type de tank (random)
   - équipe (random)
   - coordonnées de départ (random), validées par le moteur et le serveur
*/
void init_tank(map_t *m, int sock) {
    // Déclaration des variables
    int data[2]; // Coordonnées que l'on soumet à la validation du serveur
    int coordonnees_ok; // Coordonnées validées (1) ou non (0)
    int x, y; // Coordonnées de départ du tank

    // Initialisation du tank du joueur
    m->tanks[m->num_tank_joueur].type_tank=TANK_LEGER + rand()%NOMBRE_TANK;
    m->tanks[m->num_tank_joueur].rotation=90 * (rand()%4);
    m->tanks[m->num_tank_joueur].rotation_dest=m->tanks[m->num_tank_joueur].rotation;
    m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement=0;
    m->tanks[m->num_tank_joueur].nb_tour_avant_tir=0;
    m->tanks[m->num_tank_joueur].etat_tank=1;
    m->tanks[m->num_tank_joueur].point_vie_tank=POINT_DE_VIE;
    m->tanks[m->num_tank_joueur].score=0;

    m->tanks[m->num_tank_joueur].equipe=m->num_tank_joueur%2;
    m->tanks[m->num_tank_joueur].bonus=AUCUN;
    m->tanks[m->num_tank_joueur].delais_fin_bonus=0;
    m->tanks[m->num_tank_joueur].valeur_bonus=0;

    m->tanks[m->num_tank_joueur].id_joueur = m->num_tank_joueur;

    // Affectation du code sprite correspondant au type du tank et à l'équipe du joueur
    switch(m->tanks[m->num_tank_joueur].type_tank) {
    case TANK_LEGER:
        m->tanks[m->num_tank_joueur].code_sprite=TANK_T1_LEGER + m->tanks[m->num_tank_joueur].equipe;
        break;
    case TANK_LOURD:
        m->tanks[m->num_tank_joueur].code_sprite=TANK_T1_LOURD + m->tanks[m->num_tank_joueur].equipe;
        break;
    case TANK_MOYEN:
        m->tanks[m->num_tank_joueur].code_sprite=TANK_T1_MOYEN + m->tanks[m->num_tank_joueur].equipe;
        break;
	case TANK_AMPHIBIE:
        m->tanks[m->num_tank_joueur].code_sprite=TANK_T1_AMPHIBIE + m->tanks[m->num_tank_joueur].equipe;
        break;
    }

    // Choix des coordonnées de départ
    coordonnees_ok = 0;
    while(!coordonnees_ok) {
        // Coordonnées aléatoires
        x = rand()%m->largeur;
        y = rand()%m->hauteur;

        // Validation par le moteur de jeu
        coordonnees_ok = validationCoordonnees(m, x, y, m->tanks[m->num_tank_joueur].rotation);

        // Envoie ces coordonnées au serveur afin de les valider
        if(coordonnees_ok) {
            // Envoi
            data[0] = x;
            data[1] = y;
            sendData(sock,data, 2*sizeof(int));

            // Attente de la réponse
            receiveData(sock,&coordonnees_ok, sizeof(int));
        }
    }

    // On conserve les coordonnées choisies
    m->tanks[m->num_tank_joueur].coordonnees_x=x;
    m->tanks[m->num_tank_joueur].coordonnees_y=y;
    m->tanks[m->num_tank_joueur].coordonnees_dest_x=x;
    m->tanks[m->num_tank_joueur].coordonnees_dest_y=y;
}


void init(map_t ** m, SDL_Renderer **s, char * adresse_serveur, int * sock) {
    int id; // Numéro du joueur
    int nb; // Nombre de joueurs
    int b; // Brouillard

    // Initialisations
    srand(time(NULL)); // initialisation de rand
    init_tableau_contraintes(); // Initialisation tableau des contraintes
    init_tableau_char_carac();// Initialisation du tableau de caractéritiques des tanks

    // Connexion au serveur, pour récupérer numéro du joueur et nombre de joueur
    *sock=openConnection(adresse_serveur, &id ,&nb, &b);
    if(sock > 0) {
        printf("Connexion réussi.\nNombre de joueurs : %d\nNuméro : %d\n", nb, id);
    }
    else {
        perror("Échec de connexion:");
        exit(1);
    }

    // Chargement de la carte
    *m=loadMap(MAP);
    (*m)->nombre_tank_actif=nb; // Nombre de tanks actifs
    (*m)->num_tank_joueur=id; // Tank utilisé par le joueur --> il faudra trouver un autre moyen de l'identifier
    (*m)->brouillard=b; // Brouillard

    // Initialisation des tanks
    init_jeu(*m, *sock);

    // Initialisation de la partie graphique
    init_affichage(*m,s);

    // Initialise le timer pour temporiser le jeu : on va avancer pas par pas
    timerInit();
}

void end_game(map_t * m) {
    // Déclaration des variables
    int i; // compteur de boucles

    // Arret de SDL_TTF
    close_sdlttf();

    // Destruction du plateau de jeu
    free(m->tab_obus);
    free(m->tanks);
    for(i=0; i<m->hauteur; i++) {
        free(m->plateau[i]);
    }
    free(m->plateau);
    free(m);
}

/*
   Initialisation du jeu :
   - initialisation du tank du joueur
   - envoie du tank au serveur
   - récupération de l'ensemble des tanks
*/
void init_jeu(map_t *m, int sock) {
    // Déclaration des variables
    tank_t tank; // Un tank
    tank_t * tanks; // Tableau de tanks

    // Initialisation du tableau de tanks
    m->tanks = (tank_t *) malloc(m->nombre_tank_actif*sizeof(tank_t));
    m->tab_obus = NULL;
    m->nombre_obus=0;
    m->taille_tableau_obus=0;
    m->code_deplacement_demande=0;
    m->tir_demande=0;
    m->score_equipe_adverse=0;
    m->score_equipe_joueur=0;
    m->nb_bonus=0;
    m->delais_avant_bonus=(DELAIS_ENTRE_BONUS*1000/TICK_INTERVAL) * m->num_tank_joueur;
    m->delais_avant_reception_bonus=0;

    // Nombre de joueurs de chaque équipe
    if(m->nombre_tank_actif%2 == 0) {
        m->nb_tank_advserses_restants=m->nombre_tank_actif/2;
        m->nb_tank_allies_restants=m->nombre_tank_actif/2;
    }
    else if(m->num_tank_joueur % 2 == 0){
        m->nb_tank_advserses_restants=m->nombre_tank_actif/2;
        m->nb_tank_allies_restants=m->nombre_tank_actif/2+1;
    }
    else {
        m->nb_tank_advserses_restants=m->nombre_tank_actif/2 + 1;
        m->nb_tank_allies_restants=m->nombre_tank_actif/2;
    }


    // Initialisation du tank du joueur
    init_tank(m, sock);

    // Envoi notre strucure tank au serveur
    tank = m->tanks[m->num_tank_joueur];
    sendData(sock,&tank, sizeof(tank_t));

    // Attente de la réponse : le serveur nous envoie le tableau de tanks
    tanks = (tank_t *) malloc(sizeof(tank_t)*m->nombre_tank_actif);
    receiveData(sock,tanks, sizeof(tank_t)*m->nombre_tank_actif);
    memcpy(m->tanks, tanks,  sizeof(tank_t)*m->nombre_tank_actif);
    free(tanks);
}


/*
   Initialisation d'un obus lorsqu'un tir a été validé
*/
obus_t init_obus(map_t * m){
    obus_t obus;
    int type_tank = m->tanks[m->num_tank_joueur].type_tank;
    int rotation = m->tanks[m->num_tank_joueur].rotation;

    obus.nb_tour_avant_fin_deplacement = 0;
    obus.id_joueur = m->num_tank_joueur;
    obus.distance_restante = carac_tank[type_tank][PORTEE_MAX_TIR];
    obus.etat = 0;

	switch(rotation) {
        case 270:
            obus.delta_x = 0;
            obus.delta_y = -1;
            break;
        case 90:
            obus.delta_x = 0;
            obus.delta_y = +1;
            break;

        case 0:
            obus.delta_x = -1;
            obus.delta_y = 0;
            break;

        case 180:
            obus.delta_x = +1;
            obus.delta_y = 0;
            break;
    }

    obus.coordonnees_obus_x = m->tanks[m->num_tank_joueur].coordonnees_y + obus.delta_x;
    obus.coordonnees_obus_y = m->tanks[m->num_tank_joueur].coordonnees_x + obus.delta_y;
	obus.coordonnees_obus_dest_x = m->tanks[m->num_tank_joueur].coordonnees_y + obus.delta_x;
    obus.coordonnees_obus_dest_y = m->tanks[m->num_tank_joueur].coordonnees_x + obus.delta_y;

    int v = validationCoordonneesObus(m, obus.coordonnees_obus_x, obus.coordonnees_obus_y);
    if (v == 2) {
        obus.distance_restante = 1;
        obus.nb_tour_avant_fin_deplacement = 0;
    }
    v = validationCoordonneesObus(m, obus.coordonnees_obus_x + obus.delta_x, obus.coordonnees_obus_y + obus.delta_y);
    if(v == 1) {
        obus.nb_tour_avant_fin_deplacement = DELAIS_DEPLACEMENT;
    }

    return obus;
}


/*
   Gestion des déplacements :
   - calcul des coordonnées de la prochaine case
   - validation des coordonnées par le moteur
   - demande de confirmation auprès du serveur
   - enregistrement du déplacement au sein de la structure tank
*/
int gestion_deplacements(map_t *m, int sock) {
    if(m->tanks[m->num_tank_joueur].etat_tank == 0) {
        int  deplacement = 0;
        sendData(sock,&deplacement, sizeof(int));
        m->code_deplacement_demande = 0;
        return 0;
    }

    // Déclaration des variables
    int x, y, rotation; // Coordonnées demandées
    int deplacement_autorise; // Déplacement autorisé (1) ou non (0)
    int demande_deplacement[3]; // Demande de déplacement envoyée au serveur
    int code;

    // Décrémente compteur de déplacement
    if(m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement > 0) {
        m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement --;
    }

    // Dans le cas où on vient de terminer le délpacement, les coordonnées
    // sources deviennent égales aux coordonnées cibles
    if(m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement == 0) {
        m->tanks[m->num_tank_joueur].coordonnees_y =  m->tanks[m->num_tank_joueur].coordonnees_dest_y;
        m->tanks[m->num_tank_joueur].coordonnees_x =  m->tanks[m->num_tank_joueur].coordonnees_dest_x;
        m->tanks[m->num_tank_joueur].rotation =  m->tanks[m->num_tank_joueur].rotation_dest;
    }

    // Si le joueur n'a demandé à se déplacer, on informe le serveur et on termine
    if(m->code_deplacement_demande == 0) {
        code=0;
        sendData(sock,&code, sizeof(int));
        return 0;
    }

    // Calcul des coordonnées de la case où se déplacer
    switch(m->code_deplacement_demande) {
    case SDLK_LEFT:
        rotation=270;
        x= m->tanks[m->num_tank_joueur].coordonnees_x - 1;
        y= m->tanks[m->num_tank_joueur].coordonnees_y;
        break;

    case SDLK_RIGHT:
        rotation=90;
        x= m->tanks[m->num_tank_joueur].coordonnees_x + 1;
        y= m->tanks[m->num_tank_joueur].coordonnees_y;
        break;

    case SDLK_UP:
        rotation=0;
        x= m->tanks[m->num_tank_joueur].coordonnees_x;
        y= m->tanks[m->num_tank_joueur].coordonnees_y - 1;
        break;

    case SDLK_DOWN:
        rotation=180;
        x= m->tanks[m->num_tank_joueur].coordonnees_x;
        y= m->tanks[m->num_tank_joueur].coordonnees_y + 1;
        break;

    default:
        rotation= m->tanks[m->num_tank_joueur].rotation;
        x= m->tanks[m->num_tank_joueur].coordonnees_x;
        y= m->tanks[m->num_tank_joueur].coordonnees_y;

    }

    // Validation par le moteur de jeu
    deplacement_autorise = validationCoordonnees(m, x, y, rotation);

    // Mise à jour des futures coordonnées du tank si possible
    if(deplacement_autorise) {
        // Envoi les coordonnées au serveur afin de les valider
        demande_deplacement[0] = 1;
        demande_deplacement[1] = x;
        demande_deplacement[2]= y;
        sendData(sock,demande_deplacement, 3*sizeof(int));

        // Attente de la réponse
        receiveData(sock,&deplacement_autorise, sizeof(int));

        // Si le serveur a validé, on enregistre les coordonnées de là case où se rendre
        if(deplacement_autorise) {
            m->tanks[m->num_tank_joueur].coordonnees_dest_x = x;
            m->tanks[m->num_tank_joueur].coordonnees_dest_y = y;

            // Réinitialise le délais avant le prochain déplacement
            m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement=carac_tank[m->tanks[m->num_tank_joueur].type_tank][DELAIS_DEPLACEMENT]*contraintes[m->plateau[m->tanks[m->num_tank_joueur].coordonnees_y][m->tanks[m->num_tank_joueur].coordonnees_x].type_case][V];

        }
    }
    // sinon, si on peut se déplacer (délai d'attente dépassé), on modifie seulement la future orientation du tank
    else if(m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement == 0) {
        // Indique au serveur pas de déplacement
        demande_deplacement[0] = 0;
        sendData(sock,demande_deplacement, sizeof(int));

        // On effectue une rotation
        m->tanks[m->num_tank_joueur].rotation_dest = rotation;
        // Réinitialise le délais avant le prochain déplacement
        m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement=carac_tank[m->tanks[m->num_tank_joueur].type_tank][DELAIS_DEPLACEMENT]*contraintes[m->plateau[m->tanks[m->num_tank_joueur].coordonnees_y][m->tanks[m->num_tank_joueur].coordonnees_x].type_case][V];
    }
    // Si aucun déplacement n'aura lieu, on l'indique au serveur
    else {
        demande_deplacement[0] = 0;
        sendData(sock,demande_deplacement, sizeof(int));
    }

    // Déplacement traité, on indique qu'il n'y a plus de déplacement
    m->code_deplacement_demande = 0;
    return 1;
}

/*
   Lors du déplacement d'un obus, plusieurs cas possibles pour la prochaine case :
   - elle détruit l'obus (2)
   - elle est hors de la carte (1)
   - l'obus peut s'y déplacer (0)
   On retourne le code correspondant.
*/
int validationCoordonneesObus(map_t *m, int x, int y) {
      // Nouvelle case hors de la carte
    if(x<0 || x >= m->hauteur || y < 0 || y >= m->largeur) {
        return 1;
    }

    // Nouvelle case arrete les tirs
    if(contraintes[m->plateau[x][y].type_case][AT] == 1) {
        return 2;
    }

    return 0;
}


/*
    Gestion des tirs. Vérifie si joueur a demandé à tirer et peut le faire.
    Si oui, crée l'obus et l'écrit dans le paramètre obus qui est passé à l'appel.
    Retourne 1 si l'obus a été créé, 0 sinon.
*/
int gestion_tir(map_t * m, obus_t * obus) {
    if(m->tanks[m->num_tank_joueur].etat_tank == 0) {
        return 0;
    }

    // Si le joueur n'a pas demandé à tirer ou qu'il n'a pas le droit, on ne fait rien
    if(m->tanks[m->num_tank_joueur].nb_tour_avant_tir || !m->tir_demande) {
        if(m->tanks[m->num_tank_joueur].nb_tour_avant_tir > 0) {
            m->tanks[m->num_tank_joueur].nb_tour_avant_tir--;
        }
        m->tir_demande = 0;
        return 0;
    }

    // Création de l'obus
    *obus = init_obus(m);

    m->tir_demande = 0;
    m->tanks[m->num_tank_joueur].nb_tour_avant_tir = carac_tank[m->tanks[m->num_tank_joueur].type_tank][DELAIS_TIR];
    return 1;
}

/*
   Retourne 1 si l'obus est sur la meme case qu'un tank et 0 sinon
*/
int collision_char(map_t * m, int num_obus, int * num_tank_touche) {
    // Déclaration des variables
    int x, y; // Coordonnées de d'obus
    int i; // Compteur de boucle
    int id; // id du joueur qui a tiré

    x = m->tab_obus[num_obus].coordonnees_obus_x;
    y = m->tab_obus[num_obus].coordonnees_obus_y;
    id = m->tab_obus[num_obus].id_joueur;


    // Vérification de l'abscence de tank aux coordonnées demandées
    for(i=0; i<m->nombre_tank_actif; i++) {
        if((m->tanks[i].coordonnees_y == x && m->tanks[i].coordonnees_x == y) || (m->tanks[i].coordonnees_dest_y == x && m->tanks[i].coordonnees_dest_x == y) ) {
            *num_tank_touche = i;

            // Un tank ne peut pas se toucher et on ne peut pas toucher un tank hors jeu
            if(id == i || m->tanks[i].etat_tank == 0) {
                continue;
            }
            return 1;
        }
    }
    return 0;
}



int gestion_collisions(map_t * m, int num_obus) {
    int num_tank_touche;
    int rotation;
    int id_tireur;
    int puissance_tir;
    int bonus_blindage;

    // Si obus a percuté un tank
    if(collision_char(m, num_obus, &num_tank_touche)) {

        // Récupère les informatoins sur le t
        obus_t obus = m->tab_obus[num_obus];
        rotation = m->tanks[num_tank_touche].rotation;
        id_tireur = obus.id_joueur;

        if(m->tanks[id_tireur].bonus == BONUS_PUISSANCE) {
            puissance_tir = carac_tank[m->tanks[id_tireur].type_tank][PT]+m->tanks[id_tireur].valeur_bonus;
        }
        else {
            puissance_tir = carac_tank[m->tanks[id_tireur].type_tank][PT];
        }

        if(m->tanks[num_tank_touche].bonus == BONUS_BLINDAGE) {
            bonus_blindage = m->tanks[num_tank_touche].valeur_bonus;
        }
        else {
            bonus_blindage = 0;
        }



    /*** TESTER SI TANK DE LA MEME EQUIPE **/

        // Diminution des PDV du tank touché
        if(((rotation==270 || rotation==90)) && ((obus.delta_x == -1 || obus.delta_x == +1))){
            m->tanks[num_tank_touche].point_vie_tank -= puissance_tir / (carac_tank[m->tanks[num_tank_touche].type_tank][BL] + bonus_blindage);
        }
        else if( (rotation==270 && obus.delta_y == -1)  || (rotation==90 && obus.delta_y == +1 )) {
            m->tanks[num_tank_touche].point_vie_tank -= puissance_tir / (carac_tank[m->tanks[num_tank_touche].type_tank][BAR] + bonus_blindage);
        }

        else if( (rotation==270 && obus.delta_y == +1) || (rotation==90 && obus.delta_y == -1) ) {
            m->tanks[num_tank_touche].point_vie_tank -= puissance_tir / (carac_tank[m->tanks[num_tank_touche].type_tank][BAV] + bonus_blindage);
        }

        else if( (rotation==180 || rotation==0) && (obus.delta_y == -1 || obus.delta_y == +1) ) {
            m->tanks[num_tank_touche].point_vie_tank -= puissance_tir / (carac_tank[m->tanks[num_tank_touche].type_tank][BL] + bonus_blindage);
        }

        else if( (rotation==180 && obus.delta_x == -1) || (rotation==0 && obus.delta_x == +1) ) {
            m->tanks[num_tank_touche].point_vie_tank -= puissance_tir / (carac_tank[m->tanks[num_tank_touche].type_tank][BAV] + bonus_blindage);

        }

        else if( (rotation==180 && obus.delta_x == +1) || (rotation==0 && obus.delta_x == -1) ) {
            m->tanks[num_tank_touche].point_vie_tank -= puissance_tir / (carac_tank [m->tanks[num_tank_touche].type_tank][BAR] + bonus_blindage);
        }


        // Tank hors-jeu quand PDV <= 0
        if(m->tanks[num_tank_touche].point_vie_tank <= 0){
            m->tanks[num_tank_touche].point_vie_tank = 0;
            m->tanks[num_tank_touche].etat_tank = 0;

            // Gestion du nombre de tanks
            // Si joueurs de la meme équipe : -1 pour le tireur et l'équipe
            if( (id_tireur % 2) == (num_tank_touche %2)) {
                m->tanks[id_tireur].score--;

                if( (id_tireur % 2) == (m->num_tank_joueur %2) ) {
                    m->score_equipe_joueur--;
                    m->nb_tank_allies_restants--;
                }
                else {
                    m->score_equipe_adverse--;
                    m->nb_tank_advserses_restants--;
                }
            }
            // Sinon : +1 joueur et équipe
            else {
                m->tanks[id_tireur].score++;
                if( (id_tireur % 2) == (m->num_tank_joueur %2) ) {
                    m->score_equipe_joueur++;
                    m->nb_tank_advserses_restants--;
                }
                else {
                    m->score_equipe_adverse++;
                    m->nb_tank_allies_restants--;
                }
            }
        }

        return 1;
    }
    return 0;
}

/*

*/
int gestion_deplacements_obus(map_t * m) {
    int i;
    int x, y;
    int validationDeplacement;

    i=0;
    while(i<m->nombre_obus) {
        // Décrémente compteur de déplacement
        if(m->tab_obus[i].nb_tour_avant_fin_deplacement > 0) {
            m->tab_obus[i].nb_tour_avant_fin_deplacement  --;
        }

        // Dans le cas où on vient de terminer le délpacement ou que l'on effectue le 1er déplacement
        // les coordonnées sources deviennent égales aux coordonnées cibles
        if(m->tab_obus[i].nb_tour_avant_fin_deplacement  == 0) {
            // Disparition de l'obus s'il a terminé d'exploser
            if(m->tab_obus[i].etat == 1 && m->tab_obus[i].nb_tour_avant_fin_deplacement  == 0) {
                // Destruction de la case où l'obus explose (si destructible)
                if(m->tab_obus[i].etat == 1) {
                    x=m->tab_obus[i].coordonnees_obus_x;
                    y=m->tab_obus[i].coordonnees_obus_y;

                    if(contraintes[m->plateau[x][y].type_case][D]) {
                        m->plateau[x][y].type_case = DEBRIS;
                        m->plateau[x][y].code_sprite=DEBRIS_SPRITE;
                    }
                 }

                m->tab_obus[i] = m->tab_obus[--m->nombre_obus];
                // Passe à l'obus suivant
                continue;
            }

            // Coordonnées obus = coordonnées cibles
            m->tab_obus[i].coordonnees_obus_y =  m->tab_obus[i].coordonnees_obus_dest_y;
            m->tab_obus[i].coordonnees_obus_x =  m->tab_obus[i].coordonnees_obus_dest_x;

            // Décrémente la portée restante
            m->tab_obus[i].distance_restante--;

            // Gestion des collisions
            if(gestion_collisions(m, i)) {
                // Obus explose
                m->tab_obus[i].etat = 1;
                m->tab_obus[i].nb_tour_avant_fin_deplacement = DELAIS_DEPLACEMENT_OBUS;
                continue;
            }

            // Cas où l'obus ne peut plus avancer : il explose
            if(m->tab_obus[i].distance_restante == 0) {
                m->tab_obus[i].etat = 1;
                m->tab_obus[i].nb_tour_avant_fin_deplacement = DELAIS_DEPLACEMENT_OBUS;
                continue;
            }

            // Cas où l'obus est arrivé sur une case arretant les tirs : disparait
            if(m->tab_obus[i].etat == 2) {
                // Destruction de la case où l'obus explose (si destructible)
                x=m->tab_obus[i].coordonnees_obus_x;
                y=m->tab_obus[i].coordonnees_obus_y;

                if(contraintes[m->plateau[x][y].type_case][D]) {
                    m->plateau[x][y].type_case = DEBRIS;
                    m->plateau[x][y].code_sprite=DEBRIS_SPRITE;
                }

                m->tab_obus[i] = m->tab_obus[--m->nombre_obus];

                // Passe à l'obus suivant
                continue;
            }


            // Calcule le nouveau déplacement de l'obus s'il n'a pas touché un tank et peut encore se déplacer
            y = m->tab_obus[i].coordonnees_obus_dest_y + m->tab_obus[i].delta_y;
            x = m->tab_obus[i].coordonnees_obus_dest_x + m->tab_obus[i].delta_x;
            validationDeplacement = validationCoordonneesObus(m, x, y);


            // Cas où l'obus disparait : sort de la carte
            if (validationDeplacement == 1) {
                m->tab_obus[i] = m->tab_obus[--m->nombre_obus];
                continue;
            }

            // Cas où l'obus va se déplcer dans une case arretant les tirs : affiche le sprite explosion
            if(validationDeplacement == 2) {
                m->tab_obus[i].etat = 2; // état à 2 car obus va exploser sur la prochaine case -> permet d'afficher sprite FIRE
                m->tab_obus[i].coordonnees_obus_dest_y = y;
                m->tab_obus[i].coordonnees_obus_dest_x = x;
                m->tab_obus[i].nb_tour_avant_fin_deplacement = DELAIS_DEPLACEMENT_OBUS;
            }
            // Cas où tout est ok : l'obus se déplace d'une case
            else {
                m->tab_obus[i].coordonnees_obus_dest_y = y;
                m->tab_obus[i].coordonnees_obus_dest_x = x;
                m->tab_obus[i].nb_tour_avant_fin_deplacement = DELAIS_DEPLACEMENT_OBUS;
            }
        }

        i++;
    }
    return 0;
}


void generation_bonus(map_t * m, int sock) {
    // Déclaration des variables
    int x, y; // Coordonnées de départ du tank
    int i;
    int bonus_ok;
    bonus_t bonus;

    // Si on doit en générer un
    if(m->delais_avant_bonus <= 0) {

        bonus.type=rand()%AUCUN; // AUCUN est le nombre de bonus possibles...

        switch(bonus.type) {
            case BONUS_BLINDAGE:
                    bonus.valeur=(rand()%10)+1; // Valeur que l'on va ajouter au blindage de 1 à 10
                    bonus.code_sprite = BLINDAGE_SPRITE;
                    break;
            case BONUS_PUISSANCE:
                    bonus.valeur=(rand()%80) + 20; // Valeur que l'on va ajouter à la puissance de 20 à 100
                    bonus.code_sprite = PUISSANCE_SPRITE;
                    break;
            case BONUS_SOIN:
                    bonus.valeur=(rand()%74) + 25; // Régénération du tank de 25 à 99PV
                    bonus.code_sprite = SOIN_SPRITE;
                    break;
            default: break;
        }


        // Choix des coordonnées du bonus
        do{
            x = rand()%m->hauteur;
            y = rand()%m->largeur;

            // Pas 2 bonus sur la meme case
            bonus_ok = 1;
            for(i=0; i<m->nb_bonus; i++) {
                if(m->tab_bonus[i].x == x && m->tab_bonus[i].y == y) {
                    bonus_ok = 0;
                }
            }

        }while(contraintes[m->plateau[x][y].type_case][NP] == 1 || bonus_ok == 0);
        bonus.x=x;
        bonus.y=y;

        // Envoie du bonus au serveur
        sendData(sock,&bonus, sizeof(bonus_t));

        m->delais_avant_bonus=(DELAIS_ENTRE_BONUS*1000/TICK_INTERVAL) * m->nombre_tank_actif - 1;
    }
    else {
        m->delais_avant_bonus--;
    }
}


int recuperation_bonus(map_t * m) {
    int i,j;
    tank_t tank;
    bonus_t bonus;
    // Parcours de tous les tanks pour savoir s'ils sont sur la case d'un bonus
    for(i=0; i<m->nombre_tank_actif; i++) {
        tank=m->tanks[i];
        for(j=0; j<m->nb_bonus; j++) {
            bonus = m->tab_bonus[j];

            if( (tank.coordonnees_dest_y == bonus.x && tank.coordonnees_dest_x == bonus.y) ||  (tank.coordonnees_y == bonus.x && tank.coordonnees_x == bonus.y)) {

                if(bonus.type == BONUS_SOIN) {
                    m->tanks[i].point_vie_tank += bonus.valeur;
                }
                else {
                    m->tanks[i].bonus = bonus.type;
                    m->tanks[i].valeur_bonus = bonus.valeur;
                    m->tanks[i].delais_fin_bonus= DELAIS_FIN_BONUS;
                }

                m->nb_bonus= m->nb_bonus-1;
                m->tab_bonus[j] = m->tab_bonus[m->nb_bonus];

                return(0);
            }
        }
    }
    return (1);
}

void gestion_bonus(map_t * m) {
    int i;
    recuperation_bonus(m);

    for(i=0; i<m->nombre_tank_actif; i++) {
        if(m->tanks[i].delais_fin_bonus <= 0) {
            m->tanks[i].bonus = AUCUN;
        }
        else {
            m->tanks[i].delais_fin_bonus--;
        }
    }
}

/*
   Mise à jour du jeu à chaque pas de temps :
   - gestion des déplacements
   - envoi du tank au serveur
   - réception du tableau de tanks actualisé
   - mise à jour de l'affichage
*/
int update(map_t *m, int sock) {
    // Déclaration des variables
    tank_t tank;
    obus_t obus;
    obus_t * tab_obus_tmp;
    bonus_t bonus;
    char * donnees_obus;
    int demande_tir; // Indique si un tir a été effectué (1) ou non (0)
    int nombre_nouveaux_obus;

    /* Gestion du déplacement du joueur */
    // Décrémente compteur de déplacement
    gestion_deplacements(m, sock);

    // On envoie notre strucure tank au serveur
    tank = m->tanks[m->num_tank_joueur];
    sendData(sock,&tank, sizeof(tank_t));

    // Attente de la réponse : le serveur nous envoie le tableau de tanks
    receiveData(sock,m->tanks, sizeof(tank_t)*m->nombre_tank_actif);


    /* Gestion des obus */
    // Gestion du tir d'un obus
    // Si un obus a été créé, on l'envoie au serveur
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

    // S'il ne reste qu'un joueur ou moins (égalité possible)
    if(m->nb_tank_advserses_restants <= 0 || m->nb_tank_allies_restants <= 0) {
        // On termine le jeu
        return 1;
    }
    else {
        return 0;
    }
}

















