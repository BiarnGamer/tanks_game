/**************************************************************************/
/**************************************************************************/
/**                                                                      **/
/**    Ce fichier gère un client de jeu :                                **/
/**    - connexion au serveur                                            **/
/**    - initialisation du joueur                                        **/
/**    - récupération de l'ensemble des joueurs                          **/
/**    - affichage de l'ensemble des joueurs                             **/
/**    - gestion des déplacements demandés au claviers par le joueur     **/
/**                                                                      **/
/**************************************************************************/
/**************************************************************************/

/********************************/
/**   Inclusion des en-tetes   **/
/********************************/
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include "engine.h"
#include "client.h"
#include "graphics.h"
#include "timer.h"
#include <time.h>
#include <SDL/SDL_ttf.h>

/*
  Fonction principale
*/
int main(int argc, char *argv[]) {
    // Déclaration des variables
    map_t *m; // Carte du jeu
    SDL_Renderer * s; // Écran et renderer
    int finished; // Indique si le jeu est terminé (1) ou non (0)
    int sock; // Socket de connexion au serveur
    SDL_Event e;

    // Vérification du nombre d'arguments : il nous faut l'adresse du serveur
    if (argc<2) {
        printf("L'adresse du serveur doit être indiqué.\nSyntaxe: %s <Adresse du serveur>\n",argv[0]);
        exit(1);
    }

    // Initialisation du jeu
    init(&m, &s, argv[1], &sock);

    // Boucle principale
    finished=0;
    while (!finished) {
        // Récupère les évènements claviers
        //finished=getEvent(m);
        getEvent(m);

        // Affiche l'écran du jeu à l'instant t
        paint(s,m);

        // Calcul de l'état du jeu pour l'instant t+1
        finished=update(m, sock);

        // Attente du prochain pas de temps
        timerWait();
    }

    // Classement final
    paint(s,m);
    affichage_classement(s,m);

    finished = 0;
    const Uint8 *keystate;
   while (!finished) {
        SDL_WaitEvent(&e);
        keystate = SDL_GetKeyboardState(NULL);
        if (e.type==SDL_QUIT || keystate[SDL_SCANCODE_ESCAPE]) {
            finished = 1;
        }
    }

    // Fin du jeu
    end_game(m);
    return 0;
}
