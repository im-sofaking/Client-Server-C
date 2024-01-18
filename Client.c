#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 100

void cleanBuffer();
char line[MAX_LINE];

int main(int argc, char *argv[]){
    char *ip;
    int port;
    char *clientName;

    // Set default values if not provided through command line arguments
    if (argc >= 4) {
        ip = argv[1];
        port = atoi(argv[2]);
        clientName = argv[3];
    } else {
        // Set default values here
        ip = "127.0.0.1";  // Default IP address
        port = 7777;       // Default port number
        clientName = "Client1";  // Default client name
    }

  int sock;

  struct sockaddr_in addr;
  
  
  char *result;
 

  // creation of the socket

  sock = socket(AF_INET,SOCK_STREAM,0);

  //check if the socket creation is failed 
  if( sock < 0 ){
    perror("Socket creation error");
    exit(1);
  }

  printf("TCP server socket created\n");

  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr(ip);

  if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(1);
    }

  printf("Connected to the server\n");
  // Send the client name to the server
  send(sock, clientName, strlen(clientName), 0);

   while (1) {
    
        printf("\nPlease enter a string (type 'EOF' to exit):\n");
        if ((result = fgets(line, MAX_LINE, stdin)) != NULL) {
            // Check for the exit condition
            if (strcmp(line, "EOF\n") == 0) {
                break; // Exit the loop
            }

            // Print and send the message to the server
            printf("Client: %s", line);
            send(sock, line, strlen(line), 0);

            // Receive and print the server's response
            cleanBuffer();
            
        } else if (ferror(stdin)) {
            perror("Error");
            break; // Exit the loop on error
        }

        cleanBuffer(); // Clear the buffer for the next input
    }

    shutdown(sock,SHUT_RDWR);
    close(sock);
    
    printf("Disconnected from the server\n");
  
  return 0;
}


void cleanBuffer(){
    bzero(line,MAX_LINE);
}
