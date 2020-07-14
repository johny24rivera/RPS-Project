/****************************************************************************
* Program:
*    Lab RPSPclient, RPSP Rock/Paper/Scissors Protocol - Client Code
*    Brother Jones, CSE354
* Author:
*    Jonathan Rivera
* Summary:
*    This is the client side of the Rock, Paper, Scissors web application.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>

using namespace std;

#define PORT 2250  // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

// function to convert LinuxLab argument into IPv4 addr
void findIPaddr(char * addr)
{
   // Assuming LinuxLab## format
   if (addr[0] == 'L' || addr[0] == 'l')
   {
      char server[] = {addr[8], addr[9]};
      strcpy(addr, "157.201.194.2");
      strcat(addr, server);
      addr[15] = NULL; // strcat() adds empty char at the end 
   }

   // If not LinuxLab## format it is assumed to be an IPv4 addr
   // We will let the application to handle addr errors
}

int main(int argc, char *argv[])
{
   int sock = 0;
   int valread;
   struct sockaddr_in serv_addr;
   char hello[1024] = "Hello from client";
   char buffer[1024];
   char response[10];
   int port;

   // client application requires 2 arguments from the user
   if (argc != 3)
   {
      printf("usage: hostname client\n");
      printf("usage: port client\n");
      return 0;
   }

   // Convert user argument into integer format for port
   sscanf(argv[2], "%d", &port);

   // Create Socket
   if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      printf("\n Socket creation error\n");
      return -1;
   }
   printf("Socket established\n");

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(port);

   // Find Ip address of server
   findIPaddr(argv[1]);
   // Convert IPv4 and IPv6 addresses from text to binary form
   if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
   {
      printf("\nInvalid Address/ Address not supported\n");
      return -1;
   }
   printf("Address Accepted\n");

   // Connect to server
   if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
   {
      printf("\nConnection Failed\n");
      return -1;
   }
   printf("Connection Successful\n\n\n");

   // Class Protocol Wait for CONN message
   valread = read(sock, buffer, 10);
   printf("%s\n", buffer);

   // Class Protocol CONN, OPFOUND, READY
   send(sock, "CONN\n", strlen("CONN\n"), 0);
   valread = read(sock, buffer, 10);
   send(sock, "READY\n", strlen("READY\n"), 0);

   // Print Ready Message
   cout << "Ready to Play Rock Paper Scissors.";

   // Class Protocol Recieve ROUND message
   valread = read(sock, buffer, 1024);

   // Game loop
   while(buffer[0] != 'G') // Game over message is the exit case
   {
      
      // Print Rules of the game
      cout << "Enter R P S or Q\n"
         << "R = Rock\n"
         << "P = Paper\n"
         << "S = Scissors\n"
         << "Q = Quit\n";

      // Enter game input
      cout << "Enter input: ";
      cin >> response;

      //error handling
      while (response[0] != 'R' && response[0] != 'P' &&
             response[0] != 'S' && response[0] != 'Q')
      {
         cout << "Invalid entry\n";
         cout << "Enter input: ";
         cin >> response;
      }

      // quit case
      if (response[0] == 'Q')
      {
         printf("Goodbye!");
         send(sock, response, 1, 0);
         send(sock, "\n", 1, 0);
         close(sock);
         return 0;
      }

      // send input
      strcat(response, "\n");
      send(sock, response, 1, 0);

      // read game results
      valread = read(sock, buffer, 1024);
      if (buffer[0] == 'Q')
      {
         printf("OPP Left. Goodbye.");
         close(sock);
         return 0;
      }

      else if (buffer[0] == 'W')
         cout << "Congratulations: You Won!\n\n";

      else if (buffer[0] == 'L')
         cout << "Whoops: You Lost...\n\n";
      else
         cout << "Ehh: it was a draw.\n\n";
      
   }

   return 0;
}

