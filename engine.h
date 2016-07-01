#include <SDL.h>
#define MAP "data/map.bmp"


/******************************************************************/
/**   Déclaration des constantes concernant la partie graphique  **/
/******************************************************************/

// Énumère les sprites utilisables
enum {
    ROAD=0,
    GRASS,
    HARDH,
    HARDH2,
    SOFTH,
    SOFTH2,
    WOOD,
    WOOD2,
    WOOD3,
    RIVER,
    TANK_T1_LEGER,
    TANK_T2_LEGER,
    TANK_T1_MOYEN,
    TANK_T2_MOYEN,
    TANK_T1_LOURD,
    TANK_T2_LOURD,
	TANK_T1_AMPHIBIE,
	TANK_T2_AMPHIBIE,
    BRIDGE,
    BRIDGE2,
    MOUNTAIN,
    DEBRIS_SPRITE,
    BROUILLARD_32,
    OBUS_SPRITE,
    FIRE,
    BLINDAGE_SPRITE,
    SOIN_SPRITE,
    PUISSANCE_SPRITE,
    ALL
};


/*******************************************/
/**   Déclarations concernant les bonus   **/
/*******************************************/

#define DELAIS_FIN_BONUS 350
#define NOMBRE_MAX_BONUS 5
#define DELAIS_ENTRE_BONUS 15 // Délai en secondes entre deux bonus
// Énumère les bonus
typedef enum {
    BONUS_BLINDAGE=0,
    BONUS_PUISSANCE,
    BONUS_SOIN,
    AUCUN
} types_bonus_possibles ;

typedef struct{
    int x;
    int y;
    int valeur;
    types_bonus_possibles type;
    int code_sprite;
}bonus_t;


/*******************************************/
/**   Déclarations concernant les tanks   **/
/*******************************************/

#define NOMBRE_TANK 4 // Nombre de types de tanks différents
#define NOMBRE_CARAC 8 // Nombre de caractéristiques par tank
#define POINT_DE_VIE 100 // Point de vies des tanks

// Types de tanks possibles
typedef enum {
    TANK_LEGER=0,
    TANK_MOYEN,
    TANK_LOURD,
	TANK_AMPHIBIE
} types_char_possibles;

// Caractéristiques des tanks
enum {
    PT=0, // Puissance de tir
    BAV, // Blindage avant
    BL, // Blindage latéral
    BAR, // Blindage arrière
    DELAIS_DEPLACEMENT,
    DELAIS_TIR,
    PORTEE_VISION,
    PORTEE_MAX_TIR
};

// Structure tank
typedef struct {
    types_char_possibles type_tank;
    int code_sprite;
    int equipe;
    int coordonnees_x;
    int coordonnees_y;
    int coordonnees_dest_x;
    int coordonnees_dest_y;
    int rotation;
    int rotation_dest;
    int nb_tour_avant_tir;
    int nb_tour_avant_deplacement;
    int etat_tank; // Tank actif (1) ou hors jeu (0)
    int point_vie_tank;// point de vie des tanks
    int score;

    int id_joueur;

    types_bonus_possibles bonus;
    int delais_fin_bonus;
    int valeur_bonus;
} tank_t;



/******************************************/
/**   Déclarations concernant les obus   **/
/******************************************/
#define DELAIS_DEPLACEMENT_OBUS 3

typedef struct{
    int coordonnees_obus_x;
    int coordonnees_obus_y;
    int coordonnees_obus_dest_x;
    int coordonnees_obus_dest_y;
    int distance_restante; // Nombre max de cases que l'obus peut parcourir avant explosion
    int delta_x; // Indique s'il faut incrémenter ou décrémenter la coordonnée x du tank pour se diriger dans la direction du tir
    int delta_y; // Indique s'il faut incrémenter ou décrémenter la coordonnée y du tank pour se diriger dans la direction du tir
    int nb_tour_avant_fin_deplacement;
    int etat; // 0 : obus normal ; 1 : explosion
    int id_joueur;
}obus_t;




/***************************************************/
/**   Déclarations concernant le plateau de jeu   **/
/***************************************************/
#define NOMBRE_TYPE_CASES 10 // Nombre de types de cases différentes sur le plateau
#define NOMBRE_CONTRAINTES 6 // Nombre de caractéristiques par case

#define TAUX_RALENTISSEMENT 3 // Facteur de ralentissement d'une zone lente
#define TAUX_ACCELERATION 1 // Facteur de ralentissement d'une zone rapide
#define TAUX_NORMAL 2 // Facteur de ralentissement d'une zone normale

int contraintes[NOMBRE_TYPE_CASES][NOMBRE_CONTRAINTES]; // Tableau de caractéristiques des cases du plateau
int carac_tank[NOMBRE_TANK][NOMBRE_CARAC]; // Tableau de caractéristiques des tanks


// Énumère les types de cases
typedef enum {
    HERBE=0,
    ROUTE, // BLANC
    MAISON_BOIS, // ROUGE
    MAISON_PIERRE, //NOIR
    RIVIERE, // BLEU
    FORET, // VERT
    PONT , // GRIS
    PONT2,
    MONTAGNE, // 0x444
    DEBRIS
} types_cases_possibles ;

// Caractéristiques des cases
enum {
    NP=0, // Non Passable
    NPL, // Non Passanle véhicule Léger
    AT, // Arrete les tirs
    D, // Destructible
    V, // Vitesse
	NPCNA // Non Passable Char Amphibie
};

// Case du plateau de jeu
typedef struct {
    types_cases_possibles type_case;
    types_bonus_possibles bonus_case;
    int presence_char;
    int code_sprite;
} case_plateau;


// Plateau de jeu
typedef struct {
    case_plateau ** plateau;
    int largeur;
    int hauteur;
    int largeur_affichage;
    int hauteur_affichage;
    tank_t * tanks;
    obus_t * tab_obus;
    int nombre_tank_actif;
    int nombre_obus;
    int taille_tableau_obus;
    int num_tank_joueur;
    int brouillard;
    int code_deplacement_demande; // Contient un code indiquant la direction  dans lequel le joueur a demandé à se déplacer via le clavier
    int tir_demande; // Indique si le joueur demande de tirer (1) ou non (0)
    int score_equipe_joueur;
    int score_equipe_adverse;
    int nb_tank_allies_restants;
    int nb_tank_advserses_restants;
    bonus_t tab_bonus[NOMBRE_MAX_BONUS]; // Bonus de jeu
    int nb_bonus; // Nombre de bonus en jeu
    int delais_avant_bonus; // Délais avant que le client de jeu génère un bonus
    int delais_avant_reception_bonus; // Délais avant réception bonus
} map_t;


/***********************************/
/**   Déclaration des fonctions   **/
/***********************************/
// Initialisation du jeu
void init_tableau_contraintes();
void init_tableau_char_carac();
void init_jeu(map_t *m, int sock);
void init_tank(map_t *m, int sock);
void init(map_t ** m, SDL_Renderer **s, char * adresse_serveur, int * sock);

// Fin du jeu
void end_game();

// Fonctions générales du moteur de jeu
int getEvent(map_t *m);
int update(map_t *m, int sock);

// Gestion des déplacements des joueurs
int gestion_deplacements(map_t *m, int sock);
int validationCoordonnees(map_t *m, int x, int y, int rotation);

// Gestion des tirs
obus_t init_obus(map_t * m);
int validationCoordonneesObus(map_t *m, int x, int y);
int gestion_tir(map_t * m, obus_t * obus);
int collision_char(map_t * m, int num_obus, int * num_tank_touche);
int gestion_deplacements_obus(map_t * m);

// Gestion des bonus
void generation_bonus(map_t * m, int sock);
int recuperation_bonus(map_t * m);
void gestion_bonus(map_t * m);
