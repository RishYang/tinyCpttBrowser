#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFSIZE 256
void Die(char *mess){
  perror(mess); exit(1);
}

typedef struct hostent* SP_hostent;
SP_hostent newSP_hostent(char* hostname){
  SP_hostent host;

  if ((host = gethostbyname(hostname)) == NULL) {
    fprintf(stderr, "(mini) nslookup failed on '%s'\n", hostname);
    exit(1);
  }

  return (SP_hostent)host;
}

int main(int argc, char *argv[]){

  if (argc != 4) {
    fprintf(stderr, "USAGE: TCPecho <hostname> <word> <port>\n");
    exit(1);
  }
  int sock;
  SP_hostent host = newSP_hostent(argv[1]);
  struct sockaddr_in echoserver;
  char *buffer = (char*)malloc(sizeof(char) * BUFFSIZE);
  unsigned int echolen;
  int received = 0;

  if (argc != 4) {
    fprintf(stderr, "USAGE: TCPecho <hostname> <word> <port>\n");
    exit(1);
  }

  if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    Die("Failed to create socket");
  }


  memset(&echoserver, 0, sizeof(echoserver));
  echoserver.sin_family = AF_INET;
  echoserver.sin_addr.s_addr = *((unsigned long*)(host->h_addr_list[0]));
  echoserver.sin_port = htons(atoi(argv[3]));
  if (connect(sock, (struct sockaddr *)&echoserver, sizeof(echoserver)) < 0) {
    Die("Failed to connect with server");
  }
  char *buf = (char*)malloc(sizeof(char)*BUFFSIZE);
  int bytes = recv(sock, buf, BUFFSIZE-1, 0);
  while(bytes != 0){
    if(bytes < 0){
      fprintf(stdout, "Failed to receive bytes form server.\n");
      break;
    }
    buf[bytes] = '\0';
    fprintf(stdout, "%s", buf);
    bytes = recv(sock, buf, BUFFSIZE-1, 0);
  }
  fprintf(stdout, "\n");
  close(sock);
  free(buf);
  return 0;
}
