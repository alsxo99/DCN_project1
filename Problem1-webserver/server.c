#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "base64.c"

#define PORT 25000 // You can change port number here

int respond (int sock);

char username[] = "username";
char password[] = "password";
#include <stdint.h>
#include <stdlib.h>
//Problem 1 of project 1:simple webserver with authentification
//Both Problem 1 and 2 was tested on WSL enviroments, Linux, and M1 mac
//But If you still have problems on running codes please mail us
//Most importantly please comment your code

//If you are using mac 
//You can install homebrew here :https://brew.sh
//And open terminal and type 
//sudo brew install gcc
//sudo brew install make
//Type make command to build server
//And server binary will be created
//Use ifconfig command to figure out your ip(usually start with 192. or 172.)
//run server with ./server and open browser and type 192.x.x.x:25000



//If you are using Linux or WSL
//You just need to run "make"(If you are using WSL you may need to install gcc and make with apt)
//And server binary will be created
//Use ifconfig command to figure out your ip(usually start with 192. or 172.)
//run server with ./server and open browser and type 192.x.x.x:25000


//It will be better if you run virtual machine or other device to run server
//But you can also test server with opening terminal and run it on local IP 


int main( int argc, char *argv[] ) {
  int sockfd, newsockfd, portno = PORT;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  clilen = sizeof(cli_addr);

  printf("encoding start \n");// We have implemented base64 encoding you just need to use this function
  char *token = base64_encode("username:password", strlen("username:password"));//you can change your userid
  printf("encoding end \n");
  
  //browser will respond with base64 encoded "userid:password" string 
  //You should parse authentification information from http 401 responese and compare it


  /* First call to socket() function */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  // port reusable
  int tr = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == -1) {
    perror("setsockopt");
    exit(1);
  }

  /* Initialize socket structure */

  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* TODO : Now bind the host address using bind() call. 10% of score*/
    //it was mostly same as tutorial

    if ( bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1 ){
    perror("bind error");
    exit(1);
  }

  /* TODO : listen on socket you created  10% of score*/

  if ( listen(sockfd, 10) == -1 ){
    perror("listen error");
    exit(1);
  }

  printf("Server is running on port %d\n", portno);
    
    //it was mostly same as tutorial
    //in the while loop every time request comes we respond with respond function if valid

    //TODO: authentication loop 40 % of score
    while(1){
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      //TODO: accept connection
      //TODO: send 401 message(more information about 401 message : https://developer.mozilla.org/en-US/docs/Web/HTTP/Authentication) and authentificate user
      //close connection
      
      int offset, bytes;
      char buffer[9000];
      bzero(buffer,9000);
      
      offset = 0;
      bytes = 0;
      do {
        // bytes < 0 : unexpected error
        // bytes == 0 : client closed connection
        bytes = recv(newsockfd, buffer + offset, 1500, 0);
        offset += bytes;
        // this is end of http request
        if (strncmp(buffer + offset - 4, "\r\n\r\n", 4) == 0) break;
      } while (bytes > 0);

      if (bytes < 0) {
        printf("recv() error\n");
        return -1;
      } else if (bytes == 0) {
        printf("Client disconnected unexpectedly\n");
        return -1;
      }

      buffer[offset] = 0;
      printf("%s\n", buffer);
      
      // request의 Authorization value에 token이 존재하면 break.
      if ( strstr(buffer, token) )
      {
        printf("WOW\n");
        break;
      }
  
      char message[] = "HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"Access to staging site\"\r\n\r\n";

      int length = strlen(message);
      while(length > 0) {
        printf("send bytes : %d\n", bytes);
        bytes = send(newsockfd, message, length, 0);
        length = length - bytes;
      }
      printf("close\n");
      shutdown(newsockfd, SHUT_RDWR);
      close(newsockfd);
  
    }

    //Respond loop
    while (1) {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if ( newsockfd == -1 ){
      perror("accept error");
        exit(1);
      }
      //printf("test");
      respond(newsockfd);
    }

  return 0;
}

int countDigit (int n) {
  if (n == 0) return 1;

  int count = 0;
  
  while (n != 0)
  {
    n = n / 10;
    count++;
  }
  return count;
}


//TODO: complete respond function 40% of score
int respond(int sock) {

  int offset, bytes;
  char buffer[9000];
  bzero(buffer,9000);
  
  offset = 0;
  bytes = 0;
  do {
    // bytes < 0 : unexpected error
    // bytes == 0 : client closed connection
    bytes = recv(sock, buffer + offset, 1500, 0);
    offset += bytes;
    // this is end of http request
    if (strncmp(buffer + offset - 4, "\r\n\r\n", 4) == 0) break;
  } while (bytes > 0);

  if (bytes < 0) {
    printf("recv() error\n");
    return -1;
  } else if (bytes == 0) {
    printf("Client disconnected unexpectedly\n");
    return -1;
  }

  buffer[offset] = 0;
  printf("%s\n", buffer);

  char* file_start_index = strstr(buffer,"GET") + 5;
  char* file_fin_index = strstr(buffer, "HTTP") - 1;
  int str_size = (int)(file_fin_index - file_start_index);
  //char file_name[str_size];

  char* file_path = malloc(str_size);
  strncpy(file_path, buffer+5, str_size);
  //strncpy(file_name, buffer+5, str_size);
  //printf("%s\n", file_name);

  FILE* source;
  int size;
  char ch;

  source = fopen(file_path, "r");

  // 파일이 존재하지 않을 경우
  if (source == NULL)
  {
    printf("Cannot find source\n");
    exit(1);
  }
  
  fseek(source, 0, SEEK_END);
  size = ftell(source);

  fseek(source, 0, SEEK_SET);

  char buffer2[size];

  int count = 0;

  while ( (ch = fgetc(source)) != EOF )
  {
    buffer2[count] = ch;
    count++;
  }
  

  fclose(source);
  
  int Content_length_digit;
  Content_length_digit = countDigit(size);

  char Content_length[Content_length_digit];

  sprintf(Content_length, "%d", size);



  char msg1[] = "HTTP/1.1 200 OK\r\nContent-length: ";
  int len1 = strlen(msg1);
  int new_len1 = len1 + Content_length_digit;
  char new_msg1[new_len1];


  for (int i = 0; i < len1; i++)
  {
    new_msg1[i] = msg1[i];
  }
  for (int i = 0; i < Content_length_digit; i++)
  {
    new_msg1[len1 + i] = Content_length[i];
  }


  char msg2[] = "\r\nContent-Type: text/html; charset=utf-8\r\nConnection: close\r\n\r\n";
  int len2 = strlen(msg2);
  int new_len2 = len2 + size;
  char new_msg2[new_len2];

  for (int i = 0; i < len2; i++)
  {
    new_msg2[i] = msg2[i];
  }
  for (int i = 0; i < size; i++)
  {
    new_msg2[len2 + i] = buffer2[i];
  }


  int total_len = new_len1 + new_len2;

  char message[total_len];

  for (int i = 0; i < new_len1; i++)
  {
    message[i] = new_msg1[i];
  }
  for (int i = 0; i < new_len2; i++)
  {
    message[new_len1 + i] = new_msg2[i];
  }
  
  printf("%s\n", message);
  


  int length = strlen(message);
  while(length > 0) {
    printf("send bytes : %d\n", bytes);
    bytes = send(sock, message, length, 0);
    length = length - bytes;
  }
  printf("close\n");
  shutdown(sock, SHUT_RDWR);
  close(sock);
  
  return 0;
}


