#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "client.h"
#define PORT 2048
#define ERROR(X) { perror(X); exit(1); }

/* Ouverture d'une connection vers le serveur.
   host: adresse internet du serveur
   id: le numero du joueur se connectant (défini par le serveur, de 0 à nplayers-1)
   nplayers: le nombre de joueurs
   retourne un entier qui est l'identification de la connection
*/
int openConnection(const char *host, int *id,int *nplayers, int * b) {
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int t[3];
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) ERROR("opening socket");
    server = gethostbyname(host);
    if (server == NULL) ERROR("no such host");
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(PORT);
    if (connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        ERROR("connecting");
    read(sock, &t, sizeof(int)*3) ;
    *id=t[1];
    *nplayers=t[0];
    *b=t[2];
    printf("connecté au serveur %s, joueur %d/%d\n",host,*id,*nplayers);
    return sock;
}

/* Termine la connection (et termine le jeu)
   sock: l'identification de la connection
*/
void closeConnection(int sock) {
    close(sock);
}
/* Recoit des données du serveur
   sock: l'identificaiton de la connection
   data: un pointeur sur une zone pouvant contenir les données à recevoir
   size: la taille en octets des données à recevoir
 */
void *receiveData(int sock,void *data, size_t size) {
    read(sock,data,size);
    return data;
}
/* Envoie des données du serveur
   sock: l'identificaiton de la connection
   data: un pointeur sur les données à envoyer
   size: la taille en octets des données à envoyer
 */
void sendData(int sock,void *data, size_t size) {
    write(sock,data,size);
}


