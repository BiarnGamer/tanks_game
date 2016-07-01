

#define NB_TOUR_AVANT_DECISION 100
#define NB_TYPES_DECISION 3
enum {
    PLUS_PROCHE=0,
    TANK_ALEATOIRE,
    CASE_ALEATOIRE
};

typedef struct {
    int num_bot; // Numéro de joueur du bot
    int sock; // Socket de communication avec le serveur
    int cible_x; // Coordonnées de la cible à atteindre
    int cible_y;
    int id_cible; // Numéro du joeur cible, -1 si c'est une case aléatoire
    int nb_pas_avant_decision; // Nombre de pas avant changement de cible
} bot_t;


void cible_proche(map_t * m, bot_t * bots, int num_bot);
void cible_aleat(map_t * m, bot_t * bots, int num_bot);
void case_aleat(map_t * m, bot_t * bots, int num_bot);
int choix_cible(map_t * m, bot_t * bots, int num_bot);
void deplacement_bot(map_t *m, bot_t * bots, graphe_t * g, int num_bot);
void update_bot(map_t *m, int sock, bot_t * bots, graphe_t * g, int num_bot);
int main(int argc, char *argv[]);
