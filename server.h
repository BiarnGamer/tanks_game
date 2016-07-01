/********************************************/
/**   Déclaration des variables globales   **/
/********************************************/
int *clients;
int delais_avant_bonus;
int nombre_bonus;
int id_prochain_client_bonus; // ID du prochain client a avoir envoyé un bonus

/***********************************/
/**   Déclaration des fonctions   **/
/***********************************/
// Initialisation du serveur et de la partie
void initServer(int nplayers, tank_t *  tanks, int brouillard);
void initPartie(int nplayers, tank_t *  tanks);

// Validation des coordonnées des tanks
int validationPosition(int num_joueur, int nplayers, tank_t * tanks);

// Diffusion et rassemblement d'information
void broadcast(void *data, size_t size,int nplayers);
void gather(void *data, size_t size,int nplayers);

// Fermeture du serveur
void closeServer();

// Fonctions principales
void update_serveur(int nplayers, tank_t * tanks);
int main(int argc, char *argv[]);
