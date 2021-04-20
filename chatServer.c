// Isaiah Garnett
// ig907
// homework6 server
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h> // socket, listen
#include <sys/select.h> // pselect
#include <netinet/in.h> // htonl
#include <string.h> // memset
#include <unistd.h> // read, write


// prototypes
int init_server();
int await_client_connection(int sockfd, int port);
void enter_chat_mode();


int main(int argc, char **argv){
  char username[1024];
  char *endptr;
  int port;
  if(argc > 1){
    port = strtol(argv[1], &endptr, 10); // set user defined port #
  }
  else{
    port = 13000; // set default port
  }
  printf("Please enter a username: ");
  if((fgets(username, 1024, stdin)) == NULL){
    perror("Failed to read username");
    exit(1);
  }
  username[strlen(username)-1] = '\0'; // remove newline char
  printf("Username confirmed. Welcome, %s.\n", username);
  printf("Initializing server...\n");
  int sockfd = init_server(); // init_server opens a socket and returns its fd
  printf("Server initialized\n");
  printf("awaiting connection...\n");
  for(;;){
  int connfd = await_client_connection(sockfd, port); // connfd is fd returned by accept
    enter_chat_mode(connfd, username);
  }
}

void enter_chat_mode(int connfd, char *username){
  /*the two input sources to read from with pselect are stdin and connfd
  where connfd is the fd used to read messages from the client*/
  //there is no need to timeout from a chat application, so those fields are left NULL

  //set up file descriptor set
  fd_set set;
  FD_ZERO(&set);
  int max_fd = connfd + 1;
  int fd;
  for(;;){
    FD_SET(connfd, &set);
    FD_SET(1, &set);
    if((fd = pselect(max_fd, &set, NULL, NULL, NULL, NULL)) == -1){
      perror("pselect failed");
      exit(1);
    }
    char buf[1024]; // max message length is capped at 1024 chars
    if(FD_ISSET(connfd, &set)){ // read from client
      int charsRead = read(connfd, buf, 512);
      if(charsRead == -1){
        perror("read error");
        exit(1);
      }
      buf[charsRead-1] = '\0';
      if(strstr(buf, "exitConvo")){
        printf("Client disconnected\n");
        break;
      }
      puts(buf);
    }
    if(FD_ISSET(1, &set)){ // send message to client
      int charsRead = read(1, buf, 512);
      if(charsRead == -1){
        perror("read error");
        exit(1);
      }
      buf[charsRead] = '\0';
      if(strstr(buf, "exitConvo")){
        if(write(connfd, buf, strlen(buf)) == -1){
          perror("write error");
          exit(1);
        }
        printf("awaiting new connection...\n");
        break;
      }
      if(write(connfd, username, strlen(username)) == -1){
        perror("write error");
        exit(1);
      }
      char *promptColon = ": ";
      if(write(connfd, promptColon, strlen(promptColon)) == -1){
        perror("write error");
        exit(1);
      }
      if(write(connfd, buf, strlen(buf)) == -1){
        perror("write error");
        exit(1);
      }
    }
  }
}

int await_client_connection(int sockfd, int port){
  int connfd;
  if((connfd = accept(sockfd, NULL, NULL)) == -1){
    perror("failed to accept connection");
    exit(1);
  }
  else{
    printf("New client connection detected.\n");
    printf("Entering chat mode, enter \"exitConvo\" to leave.\n");
    return connfd;
  }
}

int init_server(){
  int sockfd;
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("Failed to create socket");
    exit(1);
  }
  //set sockaddr and zero it
  struct sockaddr_in serverAddr;
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(13000);

  if((bind(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr))) == -1){
    perror("failed to bind sockfd");
    exit(1);
  }
  if(listen(sockfd, 1024) == -1){
    perror("listen failed");
    exit(1);
  }
  return sockfd;
}
