#include <SDL/SDL_ttf.h>


/************************************/
/**   Déclaration des constantes   **/
/************************************/

// Largeur d'une tuile (en pixel)
#define SIZE 32

#define NOM_CASES_VISIBLES_HAUTEUR 16
#define NOM_CASES_VISIBLES_LARGEUR 32

// Hauteur de l'interface en bas de l'écran
#define HAUTEUR_INTERFACE 75

// Taille de la police de caractère
#define TAILLE_POLICE 20
#define LARGEUR_LETTRE 10 // Largeur d'une lettre en pixels, à changer selon la taille de la police
#define HAUTEUR_LIGNE 18 // Hauteur d'une ligne en pixels, à changer selon la taille de la police

// Couleurs de texte
enum {
    WHITE=0,
    RED,
    ORANGE,
    GREEN,
    COLORS_NUMBER
};

/********************************************/
/**   Déclaration des variables globales   **/
/********************************************/

// Toutes les tuiles du jeu
SDL_Texture *tile[ALL];

// Police utilisée
TTF_Font* font;

/***********************************/
/**   Déclaration des fonctions   **/
/***********************************/
// Initialisation
void loadTiles(SDL_Renderer *s);
int getpixel(SDL_Surface *surface, int x, int y);
map_t *loadMap(char *filename);
SDL_Renderer *openWindow(int w,int h);
void init_sdlttf();
void close_sdlttf();
void init_affichage(map_t * m, SDL_Renderer ** s);

// Affichage des éléments du jeu
void animation_deplacement_obus(SDL_Renderer *s, map_t * m, int u, int v);
void animation_deplacement(SDL_Renderer *s, map_t * m, int u, int v);
void decor(SDL_Renderer *s, map_t * m, int x, int y, int i_min, int i_max, int j_min, int j_max);
void affiche_texte(SDL_Renderer *s, map_t * m, int ligne, int colonne, char * texte, int color);
void texte(SDL_Renderer *s, map_t * m);
void brouillard(SDL_Renderer *s,map_t *m, int u, int v, int i_min, int i_max, int j_min, int j_max);
void paint(SDL_Renderer *s,map_t *m);

void affichage_classement(SDL_Renderer *s,map_t *m);
void affichage_ligne_classement(SDL_Renderer *s,map_t *m, int ligne, int bloc, char * texte, int color);

