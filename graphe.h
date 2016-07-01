

int * tableau_distances;
int * tableau_predecesseurs;
int id_graphe_partiel;

typedef struct {
    int noeud; // Numéro du noeud dans le graphe
    int cout; // Cout du passage vers le noeud
}voisin_t; // Voisins car graphe non orienté donc successeurs == prédécesseur

typedef struct {
    int x; // Coordonnées du noeud (position de la case sur le plateau) : ligne (x) et colonne (y)
    int y;
    voisin_t * liste_voisins; // Liste des noeuds voisins
    int nb_voisins; // Nombre de noeuds voisins
    int id_graphe_partiel; // Identifiant du dernier graphe partiel auquel le noeud a appartenu : lors du calcul du meilleur chemin, permet de savoir si noeud a été traité sans devoir tout réinitialiser à chaque fois
}noeud_t;

typedef struct {
    noeud_t * noeuds; // Liste des noeuds
    int nb_noeuds; // Nombre de noeuds du graphe
}graphe_t;


int rechercheNoeud(graphe_t * g, int x, int y);
void generation_graphe(map_t * m, graphe_t * g);
int cout(graphe_t * g, int src, int dst);
int noeud_plus_proche(graphe_t * g);
void prochaineCase(graphe_t * g, map_t * m, int x_src, int y_src, int x_dst, int y_dst, int * x, int * y);
void close_graphe();
