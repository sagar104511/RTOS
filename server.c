#include <stdio.h>
#include <stdbool.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/types.h>



#define MAX 80
#define PORT 8080
#define SA struct sockaddr

int connfd[100];
char* names[100];
int* group_members[100];
char* group_names[100];


 typedef struct arg_struct
 {
   int myid1;
 } args;

// Function designed for chat between client and server.

void sendNames(int a_socket){
  char buff[MAX];
  int i;
  for(i=0;i<100 && names[i]!=NULL;i++)
  {
    sprintf(buff,"%d. %s\n",i+1,names[i]);
    //printf("%s",buff);
    write(a_socket,buff,MAX);
  }
}

void* func(void * argsi)
{

    args* arg1=argsi;
    char buff[MAX],buff1[MAX];
    int myid,otherid;
    bool talking_to_group=false;
    myid=arg1->myid1;
    // infinite loop for chat
    names[myid]=malloc(sizeof(char)*MAX);
    read(connfd[myid],names[myid],MAX);

    //printf("otherid %d\n",otherid);
    while(1)
    {
      read(connfd[myid],buff,MAX);
      if(strcmp(buff,"LP")==0)//LP=list of person
      {
        //sendNames(connfd[myid]);
        int i;
        for(i=0;i<100 && names[i]!=NULL;i++)
        {
          sprintf(buff,"%d. %s\n",i+1,names[i]);
          //printf("%s",buff);
          write(connfd[myid],buff,MAX);
        }
      }
      else if(strcmp(buff,"LG")==0)//LG=list of groups
      {
        int i;
        for(i=0;i<100 && group_names[i]!=NULL;i++)
        {
          //if the person is not there;
          int j;
          for(j=0;j<100 && group_members[j]!=NULL;j++){
            bool is_in_group=false;

            int i;
            for(i=1;i<=group_members[j][0];i++){
              if(group_members[j][i]==myid){
                is_in_group=true;
                break;
              }
            }
            if(is_in_group){
              bzero(buff,strlen(buff));
              sprintf(buff,"%d. %s\n",j+1,group_names[j]);
              //printf("%s",buff);
              write(connfd[myid],buff,MAX);
            }
          }

        }
      }
      else if(strcmp(buff,"SP")==0)//SP=select person
      {
        read(connfd[myid],buff,MAX);

        talking_to_group=false;
        otherid=atoi(buff)-1;
      }
      else if(strcmp(buff,"SG")==0)//SG=select group
      {
        //do if only he is there in group
        read(connfd[myid],buff,MAX);
        int temp_id=atoi(buff)-1;
        bool is_in_group=false;

        int i;
        for(i=1;i<=group_members[temp_id][0];i++){
          if(group_members[temp_id][i]==myid){
            is_in_group=true;
            break;
          }
        }
        if(is_in_group){
          talking_to_group=true;
          otherid=temp_id;
        }

      }
      else if(strcmp(buff,"CG")==0)//CG=create group
      {
        int i;
        for(i=0;i<100 && group_members[i]!=NULL;i++);
        group_members[i]=malloc(sizeof(int)*10);
        group_names[i]=malloc(sizeof(char)*MAX);
        read(connfd[myid],group_names[i],MAX);
        group_members[i][0]=1;
        group_members[i][1]=myid;
        talking_to_group=true;
        otherid=i;

      }
      else if(strcmp(buff,"AG")==0)//AG=Add to group
      {
        if(talking_to_group)
        {
          bzero(buff,strlen(buff));
          read(connfd[myid],buff,MAX);
          int person_id=atoi(buff)-1;
          group_members[otherid][group_members[otherid][0]]=person_id;
          group_members[otherid][0]++;
        }
      }
      else{
        if(!talking_to_group){
          sprintf(buff1, "%s : %s\n",names[myid],buff);
          write(connfd[otherid],buff1,MAX);
        }
        else{
          // group talk;
          int i;
          for(i=1;i<=group_members[otherid][0];i++){
            bzero(buff1,strlen(buff1));
            sprintf(buff1, "%s : %s : %s\n",group_names[otherid],names[myid],buff);
            write(connfd[group_members[otherid][i]],buff1,MAX);
          }
        }
      }
    }

}

// Driver function
int main()
{
    int sockfd, len;
    struct sockaddr_in servaddr, cli;


    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
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

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);

    // Accept the data packet from client and verification
    int i=0;
    while(1)
    {
      connfd[i] = accept(sockfd, (SA*)&cli, &len);
      if (connfd[i] < 0) {
          printf("server acccept failed...\n");
          exit(0);
      }
      else
          printf("server acccept the client...\n");

      args arg1;
      arg1.myid1=i;
      pthread_t tid;
      pthread_create(&tid,NULL,func,&arg1);
      i++;

    }


    // Function for chatting between client and server
    //func(connfd);

    // After chatting close the socket
    close(sockfd);
}
