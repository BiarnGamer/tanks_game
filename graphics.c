/******************************************************************/
/******************************************************************/
/**                                                              **/
/**    Ce fichier gère l'ensemble de la partie graphique :       **/
/**    - chargement de la carte                                  **/
/**    - chargement des tuiles                                   **/
/**    - affichage du décor                                      **/
/**    - affichage des tanks et animation de leurs mouvements    **/
/**    - affichage des obus et animation de leurs mouvements     **/
/**    - affichage des bonus                                     **/
/**    - affichacge du brouillard de guerre                      **/
/**                                                              **/
/******************************************************************/
/******************************************************************/

/********************************/
/**   Inclusion des en-tetes   **/
/********************************/
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <math.h>
#include "engine.h"
#include "graphics.h"
#include "timer.h"
#include <SDL/SDL_ttf.h>


/*******************************************/
/**   Définition des variables globales   **/
/*******************************************/

/*
   Charge les différents sprites.
   Les noms des fichiers images doivent etre
   dans le meme ordre que l'enum dans le .h
*/
const char *tilenames[]= {
    "data/road.bmp",
    "data/grass.bmp",
    "data/hardhouse.bmp",
    "data/hardhouse2.bmp",
    "data/house.bmp",
    "data/house2.bmp",
    "data/wood4.bmp",
    "data/wood3.bmp",
    "data/wood5.bmp",
    "data/river.bmp",
    "data/team1_small.bmp",
    "data/team2_small.bmp",
    "data/team1_medium.bmp",
    "data/team2_medium.bmp",
    "data/team1_big.bmp",
    "data/team2_big.bmp",
	"data/team1_amphi.bmp",
	"data/team2_amphi.bmp",
    "data/bridge.bmp",
    "data/bridge2.bmp",
    "data/mountain.bmp",
    "data/debris.bmp",
    "data/brouillard_32.bmp",
    "data/bomb.bmp",
    "data/fire.bmp",
    "data/blindage.bmp",
    "data/vie.bmp",
    "data/puissance.bmp"
};


/**********************************/
/**   Définition des fonctions   **/
/**********************************/

/*
   Lit un pixel d'une carte (pixel de l'image source en .bmp).
   Retourne la couleur RGB du pixel aux coordonnées (x,y).
   On retourne un entier de 32 bits avec : 20 bits inutiles; 4 bits pour le rouge; 4 pour le vert et 4 pour le bleu.
   On concatène donc tout dans cet entier. Pour s'y retrouver par la suite, le mieux sera d'utiliser la notation hexadécimale,
   On aura alors 1 lettres par couleur : 0xRGB -> on pourra modifier facilement les valeurs comme ça !
*/
int getpixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 r,g,b,*p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    Uint32 pixel;
    switch(bpp) {
    case 1:
        pixel= *p;
        break;
    case 2:
        pixel= *(Uint16 *)p;
        break;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) pixel= p[0] << 16 | p[1] << 8 | p[2];
        else pixel= p[0] | p[1] << 8 | p[2] << 16;
        break;
    case 4:
        pixel= *(Uint32 *)p;
        break;
    default:
        pixel= 0;
    }
    SDL_GetRGB(pixel, surface->format, &r, &g, &b);
    r=r>>4;
    g=g>>4;
    b=b>>4;
    return (r<<8)+(g<<4)+b;
}

/*
   Charge toutes les tuiles du jeu
*/
void loadTiles(SDL_Renderer *s) {
    int i;
    SDL_SetRenderDrawColor(s, 0, 0, 0, 0);
    for (i=0; i<ALL; i++)  {
        SDL_Surface *loadedImage=SDL_LoadBMP(tilenames[i]);
        if (loadedImage !=NULL) {
            Uint32 colorkey = SDL_MapRGB(loadedImage->format,0xff,0xff,0xff);
            SDL_SetColorKey(loadedImage,SDL_TRUE,colorkey);
            tile[i]=SDL_CreateTextureFromSurface(s, loadedImage );
            SDL_FreeSurface(loadedImage);
        } else fprintf(stderr,"Missing file %s:%s\n",tilenames[i],SDL_GetError());
    }
}

/*
   Chargement d'une carte
*/
map_t *loadMap(char *filename) {
    // Déclaration des variables
    int i, j; // compteurs de boucle
    map_t *m; // plateau de jeu
    int largeur_map, hauteur_map; // dimensions de la carte
    int c; // couleur d'un pixel

    // Charge le fichier décrivant la map (ex : map.bmp - 1 pixel = 1 case)
    SDL_Surface *s=SDL_LoadBMP(filename);

    // Crée une carte que l'on va compléter en lisant l'image
    m = (map_t *) malloc(sizeof(map_t));

    // Initialise un plateau vide
    largeur_map = s->w;
    hauteur_map = s->h;
    m->hauteur=hauteur_map;
    m->largeur=largeur_map;

    if(largeur_map < NOM_CASES_VISIBLES_LARGEUR) {
	 m->largeur_affichage=largeur_map;
    }
    else {
	m->largeur_affichage=NOM_CASES_VISIBLES_LARGEUR;
    }

    if(hauteur_map < NOM_CASES_VISIBLES_HAUTEUR) {
	 m->hauteur_affichage=hauteur_map;
    }
    else {
	m->hauteur_affichage=NOM_CASES_VISIBLES_HAUTEUR;
    }

    m->plateau = (case_plateau **) malloc(sizeof(case_plateau*)*hauteur_map);

    if(m->plateau == NULL) {
        perror("Erreur initialisation carte jeu ");
        exit(1);
    }

    for(i=0; i<hauteur_map; i++) {
        m->plateau[i] = (case_plateau*) malloc(sizeof(case_plateau)*largeur_map);
        if(m->plateau[i] == NULL) {
            perror("Erreur initialisation carte jeu ");
            exit(1);
        }
    }

    // Lecture de la map pixel par pixel (ligne puis colonne)
    for(i=0; i<hauteur_map; i++) {
        for(j=0; j<largeur_map; j++) {

            // Récupère la couleur
            c=getpixel(s,j,i);

            // Type de case et code du sprite selon la couleur du pixel
            switch(c) {
            // Noir : Maison en pierre
            case 0x000:
                m->plateau[i][j].type_case=MAISON_PIERRE;
                m->plateau[i][j].code_sprite=HARDH+rand()%2; // 2 srpites possibles, choisis aléatoirement
                break;
            // Blanc : Herbe
            case 0xfff:
                m->plateau[i][j].type_case=HERBE;
                m->plateau[i][j].code_sprite=GRASS;
                break;
            // Bleu : Rivière
            case 0x00f:
                m->plateau[i][j].type_case=RIVIERE;
                m->plateau[i][j].code_sprite=RIVER;
                break;
            // Vert : Foret
            case 0x0f0:
                m->plateau[i][j].type_case=FORET;
                m->plateau[i][j].code_sprite=WOOD+rand()%3; // 3 srpites possibles, choisis aléatoirement
                break;
            // Rouge : Maison en bois
            case 0xf00:
                m->plateau[i][j].type_case=MAISON_BOIS;
                m->plateau[i][j].code_sprite=SOFTH+rand()%2; // 2 srpites possibles, choisis aléatoirement
                break;

            // Gris foncé : Pont horizontal
            case 0x666:
                m->plateau[i][j].type_case=PONT;
                m->plateau[i][j].code_sprite=BRIDGE;
                break;

            // Gris clair : Pont vertical
            case 0x888:
                m->plateau[i][j].type_case=PONT2;
                m->plateau[i][j].code_sprite=BRIDGE2;
                break;

            // Jaune : route
            case 0xff0:
                m->plateau[i][j].type_case=ROUTE;
                m->plateau[i][j].code_sprite=ROAD;
                break;

            // Gris très foncé : montagne
            case 0x444:
                m->plateau[i][j].type_case=MONTAGNE;
                m->plateau[i][j].code_sprite=MOUNTAIN;
                break;
            }
        }
    }

    // Retourne le plateau de jeu que l'on vient d'initialiser en lisant le fichier représentant la carte
    return m;
}

/*
   Initialisation de la bibliotheque SDL, ouverture d'une fenetre de taille
   w*SIZE x h*SIZE
 */
SDL_Renderer *openWindow(int w,int h) {
    if (SDL_Init(SDL_INIT_VIDEO)<0) {
        fprintf(stderr,"Initialization error:%s\n",SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);
    SDL_Window *sdlWindow;
    SDL_Renderer *sdlRenderer;
    //if (SDL_CreateWindowAndRenderer(w, h, SDL_WINDOW_RESIZABLE, &sdlWindow, &sdlRenderer)) {
    // MODIF POUR NE PAS AVOIR DE FENETRE REDIMENSIONNABLE
    if (SDL_CreateWindowAndRenderer(w, h, 0, &sdlWindow, &sdlRenderer)) {
        fprintf(stderr,"Couldn't create window and renderer: %s",SDL_GetError());
        exit(1);
    }
    SDL_RenderSetLogicalSize(sdlRenderer, w, h);


    return sdlRenderer;
}

/*
   Initialisation de la SDL_TTF pour l'affichage du texte
*/
void init_sdlttf() {
    TTF_Init();
    font = TTF_OpenFont("./data/UbuntuMono-R.ttf", TAILLE_POLICE);
    if(font == NULL)
    {
        printf("%s\n",TTF_GetError());
        exit (1);
    }
}

/*
   Arret de la SDL_TTF
*/
void close_sdlttf() {
    TTF_Quit();
}

/*
   Initialisation de l'affichage
*/
void init_affichage(map_t * m, SDL_Renderer ** s) {
    // Initialisation de la partie graphique
    // On ouvre une fenêtre représentant la carte : nombre de case * nombre de pixel par case
    //*s = openWindow(m->largeur * SIZE, m->hauteur * SIZE + HAUTEUR_INTERFACE); // Ouvre une fenêtre de largeur et hauteur pixels

    // ÉCran de SIZE*SIZE maxi
    *s = openWindow(m->largeur_affichage * SIZE, m->hauteur_affichage * SIZE + HAUTEUR_INTERFACE); // Ouvre une fenêtre de largeur et hauteur pixels
    loadTiles(*s); // Charge les tuiles (sprites carrés) qu'on va placer sur l'écran pour générer la carte
    init_sdlttf();
}

/*
   Gestion des animations pour les déplacements des tanks. Les déplacement
   durent plusieurs pas de temps. À chaque pas, on avance petit à petit l'animation
   jusqu'à arriver à la position finale.
*/
void animation_deplacement(SDL_Renderer *s, map_t * m, int u, int v) {
   // Déclaration des variables
   int i; // compteur de boucle
   int x, y, rotation; // coordonnées et rotation à un instant t de l'animation du tank
   int x_src, y_src, rotation_src; // coordonnées et rotation sources du tank
   int x_dest, y_dest, rotation_dest; // coordonnées et rotation cibles du tank
   int nombre_de_pas_animation; // Nombre de pas de temps que dure une animation
   int nombre_de_pas_restants; // Nombre de pas de temps restants avant la fin d'une animation
   float delta_x, delta_y, delta_r; // Écarts de positions entre deux pas de temps
   int delta_rotation; // Écart entre l'angle source et cible du tank
   SDL_Rect rect;
   SDL_Rect rect2;

   // Parcours l'ensemble des tanks pour les afficher
   for(i=0; i<m->nombre_tank_actif; i++) {

        // Si tank hors-jeu, on ne l'affiche pas
        if(m->tanks[i].etat_tank == 0) {
            continue;
        }

        // Si le tank est en mouvement, on calcule sa position pour
        // animer ce mouvement d'un pas de temps à l'autre
        if(m->tanks[i].nb_tour_avant_deplacement > 0) {

            // Calcul de la progression dans l'animation afin d'afficher le tank au bon endroit
            nombre_de_pas_animation =  carac_tank[m->tanks[i].type_tank][DELAIS_DEPLACEMENT]* contraintes[m->plateau[m->tanks[i].coordonnees_y][m->tanks[i].coordonnees_x].type_case][V]; //carac_tank[m->tanks[i].type_tank][DELAIS_DEPLACEMENT] * contraintes[m->plateau[m->tanks[i].coordonnees_x][m->tanks[i].coordonnees_y].type_case][V];
            nombre_de_pas_restants = m->tanks[i].nb_tour_avant_deplacement;

            // Récupération des coordonnées et rotation source et cibles du tank
            x_src = m->tanks[i].coordonnees_x * SIZE;
            x_dest = m->tanks[i].coordonnees_dest_x * SIZE;

            y_src = m->tanks[i].coordonnees_y * SIZE;
            y_dest = m->tanks[i].coordonnees_dest_y * SIZE;

            rotation_src = m->tanks[i].rotation;
            rotation_dest = m->tanks[i].rotation_dest;

             // Calcul du pas entre deux positions successives du tank
            delta_x = ((float) x_dest - x_src) / nombre_de_pas_animation;
            delta_y = ((float) y_dest - y_src) / nombre_de_pas_animation;

            // Calcul des coordonnées avec lesquelles afficher le tank
            x = x_src + delta_x * (nombre_de_pas_animation - nombre_de_pas_restants) - v;
            y = y_src + delta_y * (nombre_de_pas_animation - nombre_de_pas_restants) - u;

            // Calcul de l'angle avec lequel afficher le tank
            // Écart entre les deux angles
            delta_rotation = (rotation_dest - rotation_src);
            // Gestion des cas extremes (passages de 0 à 270 degré) pour
            // que la rotation se fasse selon le plus court chemin
            if ( delta_rotation > 180) {
                delta_rotation -= 360;
            }
            else if (delta_rotation < -180) {
                delta_rotation += 360;
            }
            delta_r = delta_rotation / nombre_de_pas_animation;
            rotation = rotation_src + delta_r * (nombre_de_pas_animation - nombre_de_pas_restants);

        } // Fin de la gestion des animations
        // S'il n'y a pas d'animation, on calcule les coordonnées d'affichage
        // directement à partie de celles du tank
        else {
            x = m->tanks[i].coordonnees_x * SIZE - v;
            y = m->tanks[i].coordonnees_y * SIZE - u;
            rotation = m->tanks[i].rotation;
        }

        // Si tank hors de la zone d'affichage, passe au suivant
        if(y+SIZE < 0 || y >= SIZE*NOM_CASES_VISIBLES_HAUTEUR || x+SIZE < 0 || x >= SIZE*NOM_CASES_VISIBLES_LARGEUR) {
            continue;
        }

        // Crée un rectangle d'une certaine taille : une tuile qu'on va afficher sur la carte
        rect.w=SIZE;
        rect.h=SIZE;

        //position du char sur la map
        rect.x=x;
        rect.y=y;

        // Affichage de la tuile du tank en appliquant une certaine rotation
        SDL_RenderCopyEx(s,tile[m->tanks[i].code_sprite],NULL,&rect,rotation,NULL,SDL_FLIP_NONE);

        // Si le tank est sous un arbre, on afiche le sprite de l'arbre par dessus
        if(m->plateau[m->tanks[i].coordonnees_y][m->tanks[i].coordonnees_x].type_case == FORET) {
            rect2.w=SIZE;
            rect2.h=SIZE;


            rect2.x=m->tanks[i].coordonnees_x*SIZE - v;
            rect2.y=m->tanks[i].coordonnees_y*SIZE - u;

            SDL_RenderCopy(s, tile[m->plateau[m->tanks[i].coordonnees_y][m->tanks[i].coordonnees_x].code_sprite], NULL, &rect2);
        }
        // Si le tank sera sous un arbre, on afiche le sprite de l'arbre par dessus
        if( m->plateau[m->tanks[i].coordonnees_dest_y][m->tanks[i].coordonnees_dest_x].type_case == FORET) {
            rect2.w=SIZE;
            rect2.h=SIZE;


            rect2.x=m->tanks[i].coordonnees_dest_x*SIZE - v;
            rect2.y=m->tanks[i].coordonnees_dest_y*SIZE - u;

            SDL_RenderCopy(s, tile[m->plateau[m->tanks[i].coordonnees_dest_y][m->tanks[i].coordonnees_dest_x].code_sprite], NULL, &rect2);
        }
   } // Fin de l'affichage des tanks
}

/*
   Gestion des animations pour les déplacements des obus. Les déplacement
   durent plusieurs pas de temps. À chaque pas, on avance petit à petit l'animation
   jusqu'à arriver à la position finale.
*/
void animation_deplacement_obus(SDL_Renderer *s, map_t * m, int u, int v) {
   // Déclaration des variables
   int i; // compteur de boucle
   int x, y; // coordonnées à un instant t de l'animation de l'obus
   int x_src, y_src; // coordonnées  sources de l'obus
   int x_dest, y_dest; // coordonnées cibles du tank
   int nombre_de_pas_animation; // Nombre de pas de temps que dure une animation
   int nombre_de_pas_restants; // Nombre de pas de temps restants avant la fin d'une animation
   int delta_x, delta_y; // Écarts de positions entre deux pas de temps
   SDL_Rect rect;

   // Parcours l'ensemble des tanks pour les afficher
   for(i=0; i<m->nombre_obus; i++) {

        // Si l'obus est en mouvement, on calcule sa position pour
        // animer ce mouvement d'un pas de temps à l'autre
        if(m->tab_obus[i].nb_tour_avant_fin_deplacement > 0) {

            // Calcul de la progression dans l'animation afin d'afficher l'obus au bon endroit
            nombre_de_pas_animation =  DELAIS_DEPLACEMENT_OBUS;
            nombre_de_pas_restants = m->tab_obus[i].nb_tour_avant_fin_deplacement;

            // Récupération des coordonnées source et cibles de l'obus
            x_src = m->tab_obus[i].coordonnees_obus_x * SIZE;
            x_dest = m->tab_obus[i].coordonnees_obus_dest_x * SIZE;

            y_src = m->tab_obus[i].coordonnees_obus_y * SIZE;
            y_dest = m->tab_obus[i].coordonnees_obus_dest_y * SIZE;

             // Calcul du pas entre deux positions successives de l'obus
            delta_x = (x_dest - x_src) / nombre_de_pas_animation;
            delta_y = (y_dest - y_src) / nombre_de_pas_animation;

            // Calcul des coordonnées avec lesquelles afficher l'obus
            x = x_src + delta_x * (nombre_de_pas_animation - nombre_de_pas_restants) - u;
            y = y_src + delta_y * (nombre_de_pas_animation - nombre_de_pas_restants) - v;

        } // Fin de la gestion des animations
        // S'il n'y a pas d'animation, on calcule les coordonnées d'affichage
        // directement à partie de celles de l'obus
        else {
            x = m->tab_obus[i].coordonnees_obus_x * SIZE - u;
            y = m->tab_obus[i].coordonnees_obus_y * SIZE - v;
        }

      // Si obus hors de la zone d'affichage, passe au suivant
      if(x+SIZE < 0 || x >= SIZE*NOM_CASES_VISIBLES_HAUTEUR || y+SIZE < 0 || y >= SIZE*NOM_CASES_VISIBLES_LARGEUR) {
            continue;
      }


        // Crée un rectangle d'une certaine taille : une tuile qu'on va afficher sur la carte
        rect.w=SIZE;
        rect.h=SIZE;

        //position du char sur la map
        rect.x=y;
        rect.y=x;

        // Affichage de la tuile de l'obus
        if(m->tab_obus[i].etat == 0) {
            SDL_RenderCopy(s, tile[OBUS_SPRITE], NULL, &rect);
        }
        else {
            SDL_RenderCopy(s, tile[FIRE], NULL, &rect);
        }

   } // Fin de l'affichage des obus
}

void affiche_bonus(SDL_Renderer *s, map_t * m, int u, int v) {
    int i;
    SDL_Rect rect;


    int x, y;
    x=m->tanks[m->num_tank_joueur].coordonnees_y;
    y=m->tanks[m->num_tank_joueur].coordonnees_x;




    for(i=0; i<m->nb_bonus; i++) {
        x=m->tab_bonus[i].x * SIZE - u;
        y=m->tab_bonus[i].y * SIZE - v;

        // Si bonus hors de la zone d'affichage, passe au suivant
        if(x+SIZE < 0 || x >= SIZE*NOM_CASES_VISIBLES_HAUTEUR || y+SIZE < 0 || y >= SIZE*NOM_CASES_VISIBLES_LARGEUR) {
            continue;
        }

        // Crée un rectangle d'une certaine taille : une tuile qu'on va afficher sur la carte
        rect.w=SIZE;
        rect.h=SIZE;

        //position du bonus sur la map
        rect.x=y;
        rect.y=x;

        // Affichage de la tuile du bonus
        SDL_RenderCopy(s, tile[m->tab_bonus[i].code_sprite], NULL, &rect);
    }
}

/*
   Affichage des éléments du décor
*/
void decor(SDL_Renderer *s, map_t * m, int x, int y, int i_min, int i_max, int j_min, int j_max) {
    int i,j; // hauteur et largeur
    SDL_Rect rect;

    for(i= i_min ; i<i_max; i++) {
        for(j=j_min; j<j_max; j++) {
            // Crée un rectangle d'une certaine taille : une tuile qu'on va afficher sur la carte
            rect.w=SIZE;
            rect.h=SIZE;

            // Position de la tuile sur la carte
            rect.x=j*SIZE - y;
            rect.y=i*SIZE - x;

            // On affiche la tuile sur la carte, en indiquant le sprite à utiliser
            SDL_RenderCopy(s, tile[GRASS], NULL, &rect);
            SDL_RenderCopy(s, tile[m->plateau[i][j].code_sprite], NULL, &rect);
        }
    }
}

/*
   Affichage de texte dans l'interface utilisateur du jeu (bandeau en bas de l'écran).
   La zone d'affichage est découpée en 2 lignes (0 et 1) et 3 colonnes (0 à 2).
*/
void affiche_texte(SDL_Renderer *s, map_t * m, int ligne, int colonne, char * texte, int color) {
    SDL_Color colors[COLORS_NUMBER];
    colors[WHITE] = (SDL_Color) { 255, 255, 255, 255 }; // white
    colors[RED] = (SDL_Color) { 255, 0, 0, 255 }; // red
    colors[GREEN] = (SDL_Color) { 0, 255, 0, 255 }; // green
    colors[ORANGE] = (SDL_Color) { 255, 120, 0, 255 }; // orange

    SDL_Texture* solidTexture;
    SDL_Rect solidRect;

    // On a donc environ LARGEUR_LETTRE pixels par caractère. On limite donc affichage à ((LARGEUR  / LARGEUR_LETTRE) - 6)/3 caractères.
    int nb_carac_max = ( (m->largeur_affichage * SIZE / LARGEUR_LETTRE) - 6) / 3;

    char txt[nb_carac_max+1]; // +1 pour la fin de chaine
    strncpy (txt, texte,nb_carac_max);
    txt[nb_carac_max]='\0';

    SDL_Surface* solid = TTF_RenderText_Solid(font, txt, colors[color]);


    solidTexture = SDL_CreateTextureFromSurface( s, solid );
	SDL_FreeSurface( solid );
	SDL_QueryTexture( solidTexture, NULL, NULL, &solidRect.w, &solidRect.h );
	solidRect.x = colonne * SIZE * m->largeur_affichage/3;
	//solidRect.y = SIZE * m->hauteur_affichage + ligne * HAUTEUR_INTERFACE/2; // ALigné à gauche
	solidRect.y = SIZE * m->hauteur_affichage + ligne * HAUTEUR_INTERFACE/2; // Colonne centrée
    solidRect.x += ( (m->largeur_affichage * SIZE/3 - 2*LARGEUR_LETTRE)-strlen(txt) * LARGEUR_LETTRE ) /2;

    // Colonne centrée
	/*if(colonne == 1) {
        solidRect.x += ( (m->largeur_affichage * SIZE/3 - 2*LARGEUR_LETTRE)-strlen(txt) * LARGEUR_LETTRE ) /2;
	}
	// Colonne alignée à droite
    if(colonne == 2) {
        solidRect.x += ( (m->largeur_affichage * SIZE/3 - 2*LARGEUR_LETTRE)-strlen(txt) * LARGEUR_LETTRE ) /2;
    //    solidRect.x += ( (m->largeur * SIZE/3)-strlen(txt) * 15 );
	}*/

    SDL_RenderCopy( s, solidTexture, NULL, &solidRect );
}

/*
   Affiche les informations utiles sur l'interface utilisateur (bandeau en bas de l'écran)
*/
void texte(SDL_Renderer *s, map_t * m) {
    SDL_Rect rect;
    int i,j;

      for(i= NOM_CASES_VISIBLES_HAUTEUR ; i<NOM_CASES_VISIBLES_HAUTEUR +  (HAUTEUR_INTERFACE/SIZE)+2; i++) {
        for(j=0; j<NOM_CASES_VISIBLES_LARGEUR; j++) {
            // Crée un rectangle d'une certaine taille : une tuile qu'on va afficher sur la carte
            rect.w=SIZE;
            rect.h=SIZE;

            // Position de la tuile sur la carte
            rect.x=j*SIZE;
            rect.y=i*SIZE;

            // On affiche la tuile sur la carte, en indiquant le sprite à utiliser
            SDL_RenderCopy(s, tile[BROUILLARD_32], NULL, &rect);
        }
    }


    char txt[100];

    // SCORE DES ÉQUIPES
    sprintf(txt, "Score : %d VS %d", m->score_equipe_joueur, m->score_equipe_adverse);
    affiche_texte(s, m, 0, 0, txt, WHITE);

    // Bonus puissance
    if(m->tanks[m->num_tank_joueur].bonus == BONUS_PUISSANCE) {
        sprintf(txt, "Bonus tir : + %d", m->tanks[m->num_tank_joueur].valeur_bonus);
        affiche_texte(s, m, 0, 1, txt, GREEN);
    }
    else {
         affiche_texte(s, m, 0, 1, "Bonus tir : 0", WHITE);
    }


    // POINTS DE VIE
    int vie =m->tanks[m->num_tank_joueur].point_vie_tank;
    int color;
    if(vie > 75) {
        color=GREEN;
    }
    else if(vie > 25) {
        color=ORANGE;
    }
    else {
        color=RED;
    }
    sprintf(txt, "Vie : %d", m->tanks[m->num_tank_joueur].point_vie_tank);
    affiche_texte(s, m, 0, 2, txt, color);

    // SCORE JOUEUR
    sprintf(txt, "Score perso : %d", m->tanks[m->num_tank_joueur].score);
    affiche_texte(s, m, 1, 0, txt, WHITE);


   // Bonus Blindage
    if(m->tanks[m->num_tank_joueur].bonus == BONUS_BLINDAGE) {
        sprintf(txt, "Bonus blindage : + %d", m->tanks[m->num_tank_joueur].valeur_bonus);
        affiche_texte(s, m, 1, 1, txt, GREEN);
    }
    else {
         affiche_texte(s, m, 1, 1, "Bonus blindage : 0", WHITE);
    }

    // NOMBRE TANKS RESTANTS
    sprintf(txt, "%d allies / %d ennemis", m->nb_tank_allies_restants, m->nb_tank_advserses_restants);
    affiche_texte(s, m, 1, 2, txt, WHITE);
}

/*
  Affichage du brouillard de guerre
*/
void brouillard(SDL_Renderer *s,map_t *m, int u, int v, int i_min, int i_max, int j_min, int j_max) {
    // Déclaration des variables
    int i, j; // compteurs de boucle
    int x, y;
    int dist;
    int portee_vision;
    SDL_Rect rect;

    int nombre_de_pas_animation;
    int nombre_de_pas_restants;
    float delta_x, delta_y;
    int x_t, y_t;
    int x_dest_t, y_dest_t;


    x_t = m->tanks[m->num_tank_joueur].coordonnees_y;
    y_t = m->tanks[m->num_tank_joueur].coordonnees_x;
    x_dest_t = m->tanks[m->num_tank_joueur].coordonnees_dest_y;
    y_dest_t = m->tanks[m->num_tank_joueur].coordonnees_dest_x;
    portee_vision = carac_tank[m->tanks[m->num_tank_joueur].type_tank][PORTEE_VISION];

    for(i= i_min-1 ; i<=i_max; i++) {
        for(j=j_min-1; j<=j_max; j++) {
            // Si on est proche du tank (- de 5 case de distance) : on ne met pas de brouillard
            dist = sqrt( pow(i - x_t,2) + pow(j - y_t,2) );
            if(dist <= portee_vision) {
                continue;
            }

             // Progression du mouvement du joueur
           if(m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement > 0) {
                // Calcul de la progression dans l'animation afin d'afficher le tank au bon endroit
                nombre_de_pas_animation =  carac_tank[m->tanks[m->num_tank_joueur].type_tank][DELAIS_DEPLACEMENT]* contraintes[m->plateau[m->tanks[m->num_tank_joueur].coordonnees_y][m->tanks[m->num_tank_joueur].coordonnees_x].type_case][V];
                nombre_de_pas_restants = m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement;

                 // Calcul du pas entre deux positions successives du tank
                delta_x = ((float) (x_dest_t - x_t) * SIZE) / nombre_de_pas_animation;
                delta_y = ((float) (y_dest_t - y_t) * SIZE) / nombre_de_pas_animation;

                x = j*SIZE + delta_y * (nombre_de_pas_animation - nombre_de_pas_restants);
                y = i*SIZE + delta_x * (nombre_de_pas_animation - nombre_de_pas_restants);
            }
            else {
                x = j*SIZE;
                y = i*SIZE;
            }


            // Crée un rectangle d'une certaine taille : une tuile qu'on va afficher sur la carte
            rect.w=SIZE;
            rect.h=SIZE;

            // Position de la tuile sur la carte
            rect.x=x - v;
            rect.y=y - u;

            // On affiche la tuile sur la carte, en indiquant le sprite à utiliser
            SDL_RenderCopy(s, tile[BROUILLARD_32], NULL, &rect);
        }
    }
}

void calcul_infos_fenetre(map_t * m, int * x, int *y, int *i_min, int *i_max, int *j_min, int *j_max) {
    int delta_x, delta_y;
    float delta_x2, delta_y2;
    int x_t, y_t;
    int x_dest_t, y_dest_t;
   int nombre_de_pas_animation; // Nombre de pas de temps que dure une animation
   int nombre_de_pas_restants; // Nombre de pas de temps restants avant la fin d'une animation

    // Coordonnées du tank
    x_t = m->tanks[m->num_tank_joueur].coordonnees_y;
    y_t = m->tanks[m->num_tank_joueur].coordonnees_x;
    x_dest_t = m->tanks[m->num_tank_joueur].coordonnees_dest_y;
    y_dest_t = m->tanks[m->num_tank_joueur].coordonnees_dest_x;

    // Sens du déplacement
    delta_x = x_dest_t - x_t;
    delta_y = y_dest_t - y_t;

    // Cases à afficher en hauteur
    *i_min = x_t-(m->hauteur_affichage/2); // centré sur le tank du joueur
    if(delta_x == -1) { // Si on se déplace vers le haut : une case de plus à afficher
        (*i_min)--;
    }
    if(*i_min < 0) { // Gestion du bord haut
        *i_min=0;
        delta_x=0;
    }

    *i_max = (*i_min) + m->hauteur_affichage + abs(delta_x); // centré sur le tank du joueur et prend en compte les déplacements (un case de plus à prendre si delta = 1 ou -1
    if(*i_max > m->hauteur) { // Gestion du bord bas
        *i_max=m->hauteur;
        *i_min=(*i_max)-m->hauteur_affichage;
        delta_x=0;
    }

    // Cases à afficher en largeur
    *j_min = y_t-(m->largeur_affichage/2); // centré sur le tank du joueur
    if(delta_y == -1) { // Si on se déplace vers la gauche : une case de plus à afficher
        (*j_min)--;
    }
    if(*j_min < 0) { // Gestion du bord gauche
        *j_min=0;
        delta_y = 0;
    }

    *j_max = (*j_min) + m->largeur_affichage + abs(delta_y); // centré sur le tank du joueur et prend en compte les déplacements (un case de plus à prendre si delta = 1 ou -1
    if(*j_max > m->largeur) { // Gestion du bord droit
        *j_max=m->largeur;
        *j_min=(*j_max)-m->largeur_affichage;
        delta_y = 0;
    }

    delta_x2 = 0;
    delta_y2 = 0;

    // Progression du mouvement du joueur
   if(m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement > 0 && (delta_x || delta_y)) {
        // Calcul de la progression dans l'animation afin d'afficher le tank au bon endroit
        nombre_de_pas_animation =  carac_tank[m->tanks[m->num_tank_joueur].type_tank][DELAIS_DEPLACEMENT]* contraintes[m->plateau[m->tanks[m->num_tank_joueur].coordonnees_y][m->tanks[m->num_tank_joueur].coordonnees_x].type_case][V];
        nombre_de_pas_restants = m->tanks[m->num_tank_joueur].nb_tour_avant_deplacement;

         // Calcul du pas entre deux positions successives du tank
        delta_x2 = ((float) (x_dest_t - x_t) * SIZE) / nombre_de_pas_animation;
        delta_y2 = ((float) (y_dest_t - y_t) * SIZE) / nombre_de_pas_animation;
    }

    // Vecteur de déplacement
    if(delta_x == -1) { // Vers le haut
        *x = ((*i_min) +1) * SIZE + delta_x2 * (nombre_de_pas_animation - nombre_de_pas_restants);
    }
    else { // Pas de déplacement
        *x = (*i_min) * SIZE + delta_x2 * (nombre_de_pas_animation - nombre_de_pas_restants);
    }

    if(delta_y == -1) { // Vers la gauche
        *y = ((*j_min) +1) * SIZE + delta_y2 * (nombre_de_pas_animation - nombre_de_pas_restants);
    }
    else { // Pas de déplacement
        *y = (*j_min) * SIZE + delta_y2 * (nombre_de_pas_animation - nombre_de_pas_restants);
    }


}

/*
   Redessine la carte, les joueurs, les effets, ...
*/
void paint(SDL_Renderer *s,map_t *m) {
     // Efface le rendu (on repart d'un écran noir)
    SDL_RenderClear(s);

    // Calcul des informations sur la fenetre d'affichage : vecteur de déplacement et cases du plateau à afficher
    int x, y; // Vecteur de déplacement
    int i_min, i_max, j_min, j_max; // Cases min et max à afficher en hauteur (i) et en largeur (j)
    calcul_infos_fenetre(m, &x, &y, &i_min, &i_max, &j_min, &j_max);


    // AFFICHE LE DÉCOR
    decor(s, m, x, y, i_min, i_max, j_min, j_max);

    // Affiche les bonus
    affiche_bonus(s,m, x, y);

    // AFFICHE LES TANKS
    animation_deplacement(s,m, x, y);

    // Affiche les obus
    animation_deplacement_obus(s, m, x, y) ;

    if(m->brouillard) {
        brouillard(s,m, x, y, i_min, i_max, j_min, j_max);
    }

    // Tableau de bord
    texte(s,m);

    // Affiche le tout à l'écran
    SDL_RenderPresent(s);
}


void affichage_classement(SDL_Renderer *s,map_t *m) {
int x,y, h, w;

x=m->largeur_affichage*SIZE*0.1;
y=m->hauteur_affichage*SIZE/9;
w=m->largeur_affichage*SIZE*0.8;
h=m->hauteur_affichage*SIZE/3;

    SDL_Rect rect1 = {x, y, w, h};


y=(m->hauteur_affichage*SIZE*5)/9;

    SDL_Rect rect2 = {x, y, w, h};

//SDL_FillRect(screen, &pos, SDL_MapRGB(screen->format, (r), (g),(b));


/*
int nb_e1 ;
int nb_e2;

    if(m->nombre_tank_actif%2 == 0) {
        nb_e1=m->nombre_tank_actif/2;
        nb_e2=m->nombre_tank_actif/2;
    }
    else {
        nb_e1=m->nombre_tank_actif/2;
        nb_e2=(m->nombre_tank_actif-1)/2;
    }

tank_t tab_tmp[m->nombre_tank_actif];
memcpy(tab_tmp, m->tanks, m->nombre_tank_actif * sizeof(tank_t));

tank_t equipe1[nb_e1];
tank_t equipe2[nb_e2];
*/

/*
// Tri équipe 0
int k = 0;

for(i=0; i<m->nombre_tank_actif; i+=2) {
    i_max = i;
    max = m->tanks[i].score;

    for(j=i+2; j<m->nombre_tank_actif; j+=2) {
        if( m->tanks[i].score > max) {
            max = m->tanks[i].score;
             i_max = j;
        }
    }
    tmp=tab_tmp[i];
    tab_tmp[i] = tab_tmp[i_max];
    tab_tmp[i_max] =tmp;
    equipe1[k]=tab_tmp[i];
    k++;
}*/
/*
// Tri équipe 0
k = 0;
for(i=1; i<m->nombre_tank_actif; i+=2) {
    i_max = i;
    max = m->tanks[i].score;

    for(j=i+2; j<m->nombre_tank_actif; j+=2) {
        if( m->tanks[i].score > max) {
            max = m->tanks[i].score;
             i_max = j;
        }
    }
    tmp=tab_tmp[i];
    tab_tmp[i] = tab_tmp[i_max];
    tab_tmp[i_max] =tmp;
    equipe2[k]=tab_tmp[i];
    k++;
}
*/


    // CLASSEMENT
    int equipe_joueur = m->num_tank_joueur %2;
    char txt[100];
    int score_equipe_1;
    int score_equipe_2;
    int i,j;

    if(equipe_joueur == 0) {
        score_equipe_1 = m->score_equipe_joueur;
        score_equipe_2 = m->score_equipe_adverse;
    }
    else {
         score_equipe_2 = m->score_equipe_joueur;
         score_equipe_1 = m->score_equipe_adverse;
    }


    if(score_equipe_1 < score_equipe_2) {
        SDL_SetRenderDrawColor(s, 21, 67,  172, 255);
        SDL_RenderDrawRect(s, &rect1);
        SDL_RenderFillRect(s, &rect1);

        SDL_SetRenderDrawColor(s, 71, 158, 73, 255);
        SDL_RenderDrawRect(s, &rect2);
        SDL_RenderFillRect(s, &rect2);
    }
    else {
        SDL_SetRenderDrawColor(s, 71, 158, 73, 255);
        SDL_RenderDrawRect(s, &rect1);
        SDL_RenderFillRect(s, &rect1);

        SDL_SetRenderDrawColor(s, 21, 67,  172, 255);
        SDL_RenderDrawRect(s, &rect2);
        SDL_RenderFillRect(s, &rect2);
    }




    if(score_equipe_1 < score_equipe_2) {
        sprintf(txt, "Equipe 2 : %d", score_equipe_2);
        affichage_ligne_classement(s, m, 1, 0, txt,  WHITE);


        sprintf(txt, "Joueurs  Score");
        affichage_ligne_classement(s, m, 2, 0, txt,  WHITE);

        j=1;
        for(i=1; i<m->nombre_tank_actif; i+=2) {
            sprintf(txt, "  %d         %d", i, m->tanks[i].score);
            if(i == m->num_tank_joueur) {
                affichage_ligne_classement(s, m, 3+j, 0, txt,  RED);
            }
            else {
                affichage_ligne_classement(s, m, 3+j, 0, txt,  WHITE);
            }
            j++;
        }/*
        for(i=1; i<nb_e1; i++) {
            sprintf(txt, "?            %d         %d", i, equipe2[i].score);
            if( equipe2[i].id_joueur == m->num_tank_joueur) {
                affichage_ligne_classement(s, m, 3+j, 0, txt,  RED);
            }
            else {
                affichage_ligne_classement(s, m, 3+j, 0, txt,  WHITE);
            }
        }*/




        sprintf(txt, "Equipe 1 : %d", score_equipe_1);
        affichage_ligne_classement(s, m, 1, 1, txt,  WHITE);


        sprintf(txt, "Joueurs  Score");
        affichage_ligne_classement(s, m, 2, 1, txt,  WHITE);

        j=1;
        for(i=0; i<m->nombre_tank_actif; i+=2) {
            sprintf(txt, " %d         %d", i, m->tanks[i].score);

            if(i == m->num_tank_joueur) {
                affichage_ligne_classement(s, m, 3+j, 1, txt,  RED);
            }
            else {
                affichage_ligne_classement(s, m, 3+j, 1, txt,  WHITE);
            }
            j++;
        }/*
        for(i=0; i<nb_e1; i++) {
            sprintf(txt, "?            %d         %d", i, equipe1[i].score);
            if( equipe2[i].id_joueur == m->num_tank_joueur) {
             affichage_ligne_classement(s, m, 3+j, 1, txt,  RED);
            }
            else {
                affichage_ligne_classement(s, m, 3+j, 1, txt,  WHITE);
            }
        }*/

    }
    else {
        sprintf(txt, "Equipe 1 : %d", score_equipe_1);
        affichage_ligne_classement(s, m, 1, 0, txt,  WHITE);


        sprintf(txt, "Joueurs  Score");
        affichage_ligne_classement(s, m, 2, 0, txt,  WHITE);

        j=1;
        for(i=0; i<m->nombre_tank_actif; i+=2) {
            sprintf(txt, " %d         %d", i, m->tanks[i].score);

            if(i == m->num_tank_joueur) {
                affichage_ligne_classement(s, m, 3+j, 0, txt,  RED);
            }
            else {
                affichage_ligne_classement(s, m, 3+j, 0, txt,  WHITE);
            }

            j++;
        }


        sprintf(txt, "Equipe 2 : %d", score_equipe_2);
        affichage_ligne_classement(s, m, 1, 1, txt,  WHITE);


        sprintf(txt, "Joueurs  Score");
        affichage_ligne_classement(s, m, 2, 1, txt,  WHITE);

        j=1;
        for(i=1; i<m->nombre_tank_actif; i+=2) {
            sprintf(txt, " %d         %d", i, m->tanks[i].score);
            if(i == m->num_tank_joueur) {
                affichage_ligne_classement(s, m, 3+j, 1, txt,  RED);
            }
            else {
                affichage_ligne_classement(s, m, 3+j, 1, txt,  WHITE);
            }
            j++;
        }


    }

    // Affiche le tout à l'écran
    SDL_RenderPresent(s);


}


void affichage_ligne_classement(SDL_Renderer *s,map_t *m, int ligne, int bloc, char * texte, int color) {

int x,y;

x=m->largeur_affichage*SIZE*0.1 + 2*LARGEUR_LETTRE;

if(bloc == 0) {
    y=m->hauteur_affichage*SIZE/9 + HAUTEUR_LIGNE*ligne;
}
else {
    y=(m->hauteur_affichage*SIZE*5)/9 + HAUTEUR_LIGNE*ligne;
}

    SDL_Color colors[COLORS_NUMBER];
    colors[WHITE] = (SDL_Color) { 255, 255, 255, 255 }; // white
    colors[RED] = (SDL_Color) { 255, 0, 0, 255 }; // red
    colors[GREEN] = (SDL_Color) { 0, 255, 0, 255 }; // green
    colors[ORANGE] = (SDL_Color) { 255, 120, 0, 255 }; // orange

    SDL_Texture* solidTexture;
    SDL_Rect solidRect;

    char txt[100]; // +1 pour la fin de chaine
    strncpy (txt, texte,100);
    txt[100]='\0';

    SDL_Surface* solid = TTF_RenderText_Solid(font, txt, colors[color]);


    solidTexture = SDL_CreateTextureFromSurface( s, solid );
	SDL_FreeSurface( solid );
	SDL_QueryTexture( solidTexture, NULL, NULL, &solidRect.w, &solidRect.h );
	solidRect.x = x;
	solidRect.y = y;

    SDL_RenderCopy( s, solidTexture, NULL, &solidRect );

}


