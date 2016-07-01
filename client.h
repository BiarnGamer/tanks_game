/* Fonctions a appeler dans le main, dans tank.c */
int openConnection(const char *host,int *id,int *nplayers, int * b);
void closeConnection(int sock);
void sendData(int sock,void *data, size_t size);
void *receiveData(int sock, void *data, size_t size);

