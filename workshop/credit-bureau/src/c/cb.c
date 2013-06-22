
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
#include <unistd.h>

#define PORT 3550 /* El puerto que será abierto */
#define BACKLOG 5 /* El número de conexiones permitidas */

void doprocessing(int sock) {
  int n;
  char buffer[256];

  memset(&(buffer), '0', 256);
  int recvMsgSize;

  /* Receive message from client */
  if ((recvMsgSize = recv(sock, buffer, 256, 0)) < 0)
    perror("ERROR reading to socket 1");
  int i;

  /* Send received string and receive again until end of transmission */
  while (recvMsgSize > 0) /* zero indicates end of transmission */
  {
    buffer[recvMsgSize - 1] = '\0';

    leerfile(buffer, sock);
    memset(&(buffer), '0', 256);
    /* Echo message back to client */
    if (send(sock, buffer, recvMsgSize, 0) != recvMsgSize)
      perror("ERROR writing to socket 2");

    /* See if there is more data to receive */
    if ((recvMsgSize = recv(sock, buffer, 256, 0)) < 0)
      perror("ERROR reading to socket 3");
  }

  closesocket(sock); /* Close client socket */
}

BOOL initW32() {
  WSADATA wsaData;
  WORD version;
  int error;

  version = MAKEWORD( 2, 0 );

  error = WSAStartup(version, &wsaData);

  /* check for error */
  if (error != 0) {
    /* error occured */
    return FALSE;
  }

  /* check for correct version */
  if (LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0) {
    /* incorrect WinSock version */
    WSACleanup();
    return FALSE;
  }
}



int leerfile(char* buffer, int sock) {

  FILE * pFile;
  char mystring[100];
  boolean found = FALSE;

  pFile = fopen("Loans.txt", "r");

  if (pFile == NULL )
    perror("Error opening file");
  else {

    while (feof(pFile) == 0) {
      fgets(mystring, sizeof(mystring), pFile);

      if (strncmp(mystring + 4, buffer, 10) == 0) {
        puts(mystring);
        send(sock, mystring, strlen(mystring), 0);
        found = TRUE;
      }
    }

    if (found == FALSE)
      send(sock, "No se encontro\n", 15, 0);
  }
  fclose(pFile);
  return 0;
}

int main() {

  initW32(); 
  int fd, fd2; 
  struct sockaddr_in server; 
  struct sockaddr_in client;

  unsigned int sin_size;

  pid_t pid;

  /* A continuación la llamada a socket() */
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("error en socket()\n");
    exit(-1);
  }

  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = INADDR_ANY;/* INADDR_ANY coloca nuestra dirección IP automáticamente */
  //bzero(&(server.sin_zero),8);

  memset(&(server.sin_zero), '0', 8); /* escribimos ceros en el reto de la estructura */

  /* A continuación la llamada a bind() */
  if (bind(fd, (struct sockaddr*) &server, sizeof(struct sockaddr)) == -1) {
    printf("error en bind() \n");
    exit(-1);
  }

  if (listen(fd, BACKLOG) == -1) { /* llamada a listen() */
    printf("error en listen()\n");
    exit(-1);
  }

  while (1){
    sin_size = sizeof(struct sockaddr_in);
    /* A continuación la llamada a accept() */
    if ((fd2 = accept(fd, (struct sockaddr *) &client, &sin_size)) == -1) {
      printf("error en accept()\n");
      exit(-1);
    }

    printf("Se obtuvo una conexion desde %s\n", inet_ntoa(client.sin_addr));
    /* que mostrará la IP del cliente */
    //send(fd2,"Bienvenido a mi servidor.\n",99,0);
    /* que enviará el mensaje de bienvenida al cliente */
    doprocessing(fd2);

  } 
}
