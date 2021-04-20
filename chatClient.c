// Isaiah Garnett
// ig907
// homework6 client
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h> // pselect
#include <sys/types.h>
#include <sys/socket.h> // socket, listen
#include <sys/select.h> // pselect
#include <netinet/in.h> // htonl
#include <string.h> // memset
#include <arpa/inet.h> // inet_pton
#include <unistd.h> //read



int connect_to_server(char *src, int port);
void enter_chat_mode(int connfd, char *username);

int main(int argc, char **argv){
  char username[1024];
  int connfd;
  printf("Please enter a username: ");
  if((fgets(username, 1024, stdin)) == NULL){
    perror("Failed to read username");
    exit(1);
  }
  username[strlen(username)-1] = '\0'; // remove newline
  printf("Username confirmed. Welcome, %s.\n", username);

  printf("Initializing connection to server...\n");
  if(argc >2){
    char *endptr;
    int port = strtol(argv[2], &endptr, 10);
    if(endptr == argv[2]){
      perror("invalid arg for argv[2], must be a number");
      exit(1);
    }
    connfd = connect_to_server(argv[1], port);
  }
  else if(argc > 1){
    connfd = connect_to_server(argv[1], -1); // -1 if user gives no port #
  }
  else{
    connfd = connect_to_server("127.0.0.1", -1); // -1 if user gives no port #
  }
  enter_chat_mode(connfd, username);
}

int connect_to_server(char *src, int port){
  if(port == -1){ port = 13000; }
  int sockfd;
  struct sockaddr_in serverAddr;
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("failed to open socket");
    exit(1);
  }
  // init and zero out sockaddr struct
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port); // port used by chatServer.c
  if(inet_pton(AF_INET, src, &serverAddr.sin_addr) == -1){
    perror("Failed to convert src to 'af' network address");
    exit(1);
  }
  if(connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1){
    perror("Failed to connect");
    exit(1);
  }

  printf("Connection established.\nEntering chat mode, enter \"exitConvo\" to leave.\n");
  return sockfd;
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
    if(FD_ISSET(connfd, &set)){ // read from server
      int charsRead = read(connfd, buf, 512);
      if(charsRead == -1){
        perror("read error");
        exit(1);
      }
      buf[charsRead-1] = '\0';
      if(strstr(buf, "exitConvo")){
        printf("Chat ended by server.");
        exit(0);
      }
      puts(buf);
    }
    if(FD_ISSET(1, &set)){ // send message to server
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
