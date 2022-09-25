/* run using ./server <port> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

void error(char *msg) {
  perror(msg);
  exit(1);
}


void *sendRecieve(void *newsockfd){
    int fd = *((int *)newsockfd);
    ssize_t n;
    char buffer[256];
    while(1) {
        bzero(buffer, 256);
        n = read(fd, buffer, 255);
        if (n < 0) {
            error("ERROR reading from socket");
        }
        printf("Here is the message: %s", buffer);

        /* send reply to client */
        n = write(fd, "I got your message", 18);
        if (n < 0){
            error("ERROR writing to socket");
        }
    }
}

int main(int argc, char *argv[]) {
  int sockfd, newsockfd, portno;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  clilen = sizeof(cli_addr);

  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
     */
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* bind socket to this port number on this machine */
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    /* listen for incoming connection requests */
    listen(sockfd, 5);

    while(1){
        /* declare thread */
        pthread_t t;

        /* reset client address to zero */
        bzero((char *)&cli_addr, clilen);

        /* accept a new request, create a newsockfd */
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        /* create thread */
        pthread_create(&t,NULL,sendRecieve,(void *)&newsockfd);
    }
  return 0;
}
