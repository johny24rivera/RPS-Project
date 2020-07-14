/****************************************************************************
* Program:
*    Lab RPSPserver, RPSP Rock/Paper/Scissors Protocol - Server Code
*    Brother Jones, CSE354
* Author:
*    Jonathan Rivera
* Summary:
*    This is the Server Code that creates a socket which allows two clients
*    to connect and play against each other in a game of Rock, Paper,
*    Scissors.
*
*****************************************************************************
* Changes made to my code for the re-submission:
*   (examples:)
*   -I added comments such that they would help me remember what I did 6
        months from now
*   - Used strcpy() to assign string to char arrays
*   - placed all initialized variables at the top on main
*
****************************************************************************
* Sources:
*   - http://www.cplusplus.com/reference/cstring/strcpy/
*   - https://www.thecrazyprogrammer.com/2017/06/socket-programming.html
*   - https://www.geeksforgeeks.org/socket-programming-cc/
*   - https://www.tutorialspoint.com/c_standard_library/c_function_sscanf.htm
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>
#include <signal.h>

using namespace std;

#define PORT 2250  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold

int main(int argc, char const *argv[])
{
   int port;
   int server_fd;
   int socket1;   // Socket for client 1
   int socket2;   // Socket for client 2
   int valread;   // Used for reading messages from clients
   struct sockaddr_in address;
   int opt = 1;
   int addrlen = sizeof(address);
   char buffer1[1024] = {0}; // Messages from client 1
   char buffer2[1024] = {0}; // Messages from client 2
   char hello[1024];         // Hello messages for clients
   bool quit = false;        // will be used for while loop
   char results1[10];      // Game results message for client 1
   char results2[10];      // Game results message fro client 2

   
   // Checking if there are port values
   if (argc != 2)
   {
      perror("Usage: server port");
      return 0;
   }
   // If so set user input to port number
   sscanf(argv[1], "%d", &port);
   
   
   // Creating socket file descriptor
   if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
   {
      perror("socket failed");
      exit(EXIT_FAILURE);
   }
   printf("Socket created\n");

   // Forcefully attaching socket to port
   if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                  &opt, sizeof(opt)))
   {
      perror("setsockopt");
      exit(EXIT_FAILURE);
   }
   

   address.sin_family = AF_INET;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(port);

   // Attching socket to port
   if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0)
   {
      perror("bind failed");
      exit(EXIT_FAILURE);
   }
   
   //Listen for connection from client 1
   if (listen(server_fd, 3) < 0)
   {
      perror("listen");
      exit(EXIT_FAILURE);
   }
   printf("Waiting for connection...\n");

   // Accept connection from client 1
   if ((socket1 = accept(server_fd, (struct sockaddr *) &address,
                            (socklen_t *) &addrlen)) < 0)
   {
      perror("accept");
      exit(EXIT_FAILURE);
   }
   printf("Player 1 connected\n");

   // Class Protocol Send CONN message
   send(socket1, "CONN\n", strlen("CONN\n"), 0);
   printf("Sent CONN message to client 1\n");
   valread = read(socket1, buffer1, 10);
   printf("%s\n", buffer1);


   // Listening for connection from client 2
   if (listen(server_fd, 3) < 0)
   {
      perror("listen");
      exit(EXIT_FAILURE);
   }
   printf("Waiting for Player 2...\n");

   // Accepting connection from client 2
   if ((socket2 = accept(server_fd, (struct sockaddr *) &address,
                            (socklen_t *) &addrlen)) < 0)
   {
      perror("accept");
      exit(EXIT_FAILURE);
   }
   printf("Player 2 connected\n");

   // Class Protocol send CONN message to client 2
   send(socket2, "CONN\n", strlen("CONN\n"), 0);
   printf("Sent CONN message to client 1\n");
   valread = read(socket2, buffer2, 10);
   printf("%s\n", buffer2);

   // Class Protocol send OPFOUND to both clients
   send(socket1, "OPFOUND\n", strlen("OPFOUND\n"), 0);
   printf("Sent OPFOUND message to client 1\n");
   send(socket2, "OPFOUND\n", strlen("OPFOUND\n"), 0);
   printf("Sent OPFOUND message to client 2\n");

   // Class Protocol Wait for "READY" Message from both clients
   valread = read(socket1, buffer1, 10);
   printf("%s", buffer1);
   valread = read(socket2, buffer2, 10);
   printf("%s", buffer2);

   // Class Protocol send "ROUND" message
   send(socket1, "ROUND\n", strlen("ROUND\n"), 0);
   printf("Sent ROUND message to client 1\n");
   send(socket2, "ROUND\n", strlen("ROUND\n"), 0);
   printf("Sent ROUND message to client 2\n");

   // Game Executes in this loop
   while(!quit)
   {

      // Receive ATTACK responses
      valread = read(socket1, buffer1, 10);
      valread = read(socket2, buffer2, 10);

      // Quit condition handling
      if (buffer1[0] == 'Q' || buffer2[0] == 'Q')
      {
         send(socket1, "Q\n", strlen("Q\n"), 10);
         send(socket2, "Q\n", strlen("Q\n"), 10);
         close(socket1);
         close(socket2);
         quit = true;
      }
      
      // Draw condition handling
      else if (buffer1[0] == buffer2[0])
      {
         strcpy(results1, "D\n");
         strcpy(results2, results1);
      }
      
      // Client 1 Win Conditions handling
      else if((buffer1[0] == 'R' && buffer2[0] == 'S') ||
              (buffer1[0] == 'S' && buffer2[0] == 'P') ||
              (buffer1[0] == 'P' && buffer2[0] == 'R'))
      {
         
         strcpy(results1, "W\n");
         strcpy(results2, "L\n");
      }

      // Everything else false under this condition
      else
      {
         strcpy(results1, "L\n");
         strcpy(results2, "W\n");
      }

      // Send the results of the Game
      send(socket1, results1, strlen(results1), 10);
      send(socket2, results2, strlen(results2), 10);
   }
   
   return 0;
}
