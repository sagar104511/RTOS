#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/types.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

int sockfd;

void* func_write(void* args)
{
    char buff[MAX];
    int n;
    while(1)
    {
      bzero(buff,strlen(buff));
      scanf("%s",buff);
      //printf("OK1\n");
      write(sockfd,buff,MAX);
    }

}

void* func_read(void* args)
{
    char buff[MAX];
    int n;
    //printf("OK3\n");
    while(1)
    {
      //printf("OK2\n");
      buff[0]=0;
      read(sockfd,buff,MAX);
      //printf("OK2\n");
      printf("%s",buff);
      //printf("OK3\n");
    }

}

int main()
{
    int connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // function for chat
    //func(sockfd);
    pthread_t tid;
    pthread_create(&tid,NULL,func_read,NULL);

    func_write(NULL);
    //func_read(NULL);



    // close the socket
    close(sockfd);
}
