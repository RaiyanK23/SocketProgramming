#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <sys/time.h>

#define PORT 8989
#define BUFFSIZE 4096

typedef struct sockaddr_in SOCK_IN;

using namespace std;

int main()
{
    // Create a server-side TCP socket
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    // Error checking
    if (listening == -1)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        exit(1);
    }

    //	Create a hint structure for the client we're connecting with
    sockaddr_in hint;
    // FIlling in the values: IPV4, IP ADDRESS and PORT
    hint.sin_family = AF_INET;
    hint.sin_port = htons(PORT);
    hint.sin_addr.s_addr = INADDR_ANY;

    // Bind the ip address and port to a socket
    if (bind(listening, (sockaddr *)&hint, sizeof(hint)) == -1)
        printf("Bind failed!\n");
    else
        printf("Server now running on port %d\n", PORT);

    // Listen on the IP address and PORT
    if (listen(listening, 5) == -1)
        printf("Listen failed!\n");
    else
        printf("Server now listening for incoming client connections...\n");

    sockaddr_in client; // needed to store client sockaddr_in info
    socklen_t clientSize = sizeof(client);

    // Wait for a connection
    int clientSocket = accept(listening, (sockaddr *)&client, &clientSize);
    // Error checking
    if (clientSocket == -1)
    {
        perror("Accept() has failed.");
        exit(1);
    }
    else
        printf("Got a client connection!\n");

    string display = "Welcome! Please select one of the following services: \n 1. Translator Service \n 2. Currency Converter Service \n 3. Voting \n 4. Exit \n";
    send(clientSocket, display.c_str(), display.size() + 1, 0); // Send the menu options as soon as we accept a connection

    // Close listening socket
    close(listening);

    // buf array to send and receive from both client and micro servers
    char buf[4096];

    // While loop: Create UDP client sockets and make/receive requests from micro servers

    while (true)
    {
        memset(buf, 0, 4096);
        // General strings needed for program operations
        string translation_servc = "Enter an English word: \n";
        string currency_servc = "Please enter the desired currency conversion in the following: <Amount> <Source Currency> <Destination Currency>";
        string voting_servc = "Choose 1 of the following: \n 1. Show Candidates \n 2. Secure Voting \n 3. Voting Summary";
        string err = "That is not a valid option. Please try again. \n";
        string exit = "Exiting the program...";
        string exit_client = "Server connection terminated.";

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        // Error checking
        if (bytesReceived == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }

        else
            cout << "Got " << bytesReceived << " bytes from client" << endl;

        if (bytesReceived == 0)
        {
            cout << "Client disconnected " << endl;
            break;
        }

        // If the client chooses the translation service:
        if (strcmp(buf, "1") == 0)
        {
            // Prompt them for user input
            int bytes = send(clientSocket, translation_servc.c_str(), translation_servc.size() + 1, 0);
            printf("Sent %d bytes to client\n", bytes);
            // store user input
            bytes = recv(clientSocket, buf, 4096, 0);
            printf("Received %d bytes from client\n", bytes);
            cout << "'" << buf << "'" << endl;

            // Creating UDP client
            // Create a hint structure for the server
            sockaddr_in server;
            socklen_t server_size = sizeof(server);
            server.sin_family = AF_INET;                        // AF_INET = IPv4 addresses
            server.sin_port = htons(8010);                      // Little to big endian conversion
            server.sin_addr.s_addr = inet_addr("136.159.5.25"); // Convert from string to byte array

            // Socket creation, note that the socket type is datagram
            int out = socket(AF_INET, SOCK_DGRAM, 0);

            // Timeout UDP socket if no connection after 5 seconds
            struct timeval tv;
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            if (setsockopt(out, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
            {
                perror("Error");
            }

            // Send a request to the micro server

            int sendOk = sendto(out, buf, BUFFSIZE, 0, (sockaddr *)&server, sizeof(server));

            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
            else
                cout << "Sent " << sendOk << " bytes to UDP server" << endl;

            // Receive response from micro server
            int bytesIn = recvfrom(out, buf, BUFFSIZE, 0, (sockaddr *)&server, &server_size);
            // Error checking
            if (bytesIn == -1)
            {
                perror("Failed to receive.");
            }
            else
                cout << "Received " << bytesIn << " bytes from UDP server" << endl;
            cout << "Received from UDP: " << buf << endl;

            // Close the socket
            close(out);

            send(clientSocket, buf, 4096, 0); // Send the response from micro server back to client

            continue;
        }

        // If the client chooses the currency service:

        else if (strcmp(buf, "2") == 0)
        {
            // Ask for user input from client
            send(clientSocket, currency_servc.c_str(), currency_servc.size() + 1, 0);
            // Receive the user input from the client
            recv(clientSocket, buf, 4096, 0);
            cout << buf << endl;
            // <-------- Checking to make sure user input is valid (error handling) ----------->

            int space_counter = 0;

            for (int i = 0; i < strlen(buf); i++)
            {
                if (isspace(buf[i]))
                {
                    space_counter++;
                }
            }

            cout << space_counter << endl;

            if (space_counter < 2 || space_counter > 2)
            {
                send(clientSocket, err.c_str(), err.size() + 1, 0);
                send(clientSocket, display.c_str(), display.size() + 1, 0);
                continue;
            }

            // <---------------------- End of error handling ------------>

            // Creating UDP client
            // Create a hint structure for the client
            sockaddr_in server1;
            socklen_t server_size1 = sizeof(server1);
            server1.sin_family = AF_INET;                        // AF_INET = IPv4 addresses
            server1.sin_port = htons(8050);                      // Little to big endian conversion
            server1.sin_addr.s_addr = inet_addr("136.159.5.25"); // Convert from string to byte array

            // Socket creation, note that the socket type is datagram
            int out = socket(AF_INET, SOCK_DGRAM, 0);

            // Timeout UDP socket if no connection after 5 seconds
            struct timeval tv;
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            if (setsockopt(out, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
            {
                perror("Error");
            }

            // Sending a request to the currency microserver
            int sendOk = sendto(out, buf, BUFFSIZE, 0, (sockaddr *)&server1, sizeof(server1));

            // Error checking
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }

            // Receive a response from the currency microserver
            int bytesIn = recvfrom(out, buf, BUFFSIZE, 0, (sockaddr *)&server1, &server_size1);
            // Error checking
            if (bytesIn == -1)
            {
                perror("Failed to receive.");
            }
            cout << "Received from UDP: " << buf << endl;

            // Close the socket
            close(out);

            send(clientSocket, buf, 4096, 0); // Sending the response from micro server back to the client

            continue;
        }

        // If the client chooses the voting service:

        else if (strcmp(buf, "3") == 0)
        {
            // Ask client for input
            send(clientSocket, voting_servc.c_str(), voting_servc.size() + 1, 0);
            // Receive client input
            recv(clientSocket, buf, 4096, 0);
            cout << buf << endl;

            // Creating UDP client
            // Create a hint structure for the client
            sockaddr_in server2;
            socklen_t server_size2 = sizeof(server2);
            server2.sin_addr.s_addr = inet_addr("136.159.5.25"); // Convert from string to byte array
            server2.sin_port = htons(8100);                      // Little to big endian conversion
            server2.sin_family = AF_INET;                        // AF_INET = IPv4 addresses

            // Socket creation, note that the socket type is datagram
            int out = socket(AF_INET, SOCK_DGRAM, 0);

            // Timeout UDP socket if no connection after 3 seconds
            struct timeval tv;
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            if (setsockopt(out, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
            {
                perror("Error");
            }

            // Send the client request to the micro server
            int sendOk = sendto(out, buf, BUFFSIZE, 0, (sockaddr *)&server2, sizeof(server2));

            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }

            // Receive the micro server response
            int bytesIn = recvfrom(out, buf, BUFFSIZE, 0, (sockaddr *)&server2, &server_size2);
            // Error checking
            if (bytesIn == -1)
            {
                perror("Failed to receive.");
            }
            cout << "Received from UDP: " << buf << endl;

            if (strcmp(buf, "5") == 0)
            {
                // Ask client of input
                send(clientSocket, buf, 4096, 0);
                // Receive client input
                recv(clientSocket, buf, 4096, 0);

                // Send the client request to voting micro server
                int sendOk = sendto(out, buf, BUFFSIZE, 0, (sockaddr *)&server2, sizeof(server2));
                // Error checking
                if (sendOk == -1)
                {
                    perror("Sendto() has failed.");
                }
                // Receive micro srver response
                int bytesIn = recvfrom(out, buf, BUFFSIZE, 0, (sockaddr *)&server2, &server_size2);
                if (bytesIn == -1)
                {
                    perror("Failed to receive.");
                }
                cout << "Received from UDP: " << buf << endl;
                // Send the encrypted key to client
                send(clientSocket, buf, 4096, 0);
                // Receive the encrypted voter ID from client
                recv(clientSocket, buf, 4096, 0);
                cout << "Value of encoded is: " << buf << endl;

                // Send to voting server to decode the voter ID
                int sendOk1 = sendto(out, buf, BUFFSIZE, 0, (sockaddr *)&server2, sizeof(server2));

                if (sendOk1 == -1)
                {
                    perror("Sendto() has failed.");
                }
                // Prompt user menu
                send(clientSocket, display.c_str(), display.size() + 1, 0);
                // Close the UDP socket
                close(out);
                continue;
            }

            // Close the socket
            close(out);

            send(clientSocket, buf, 4096, 0);
            // memset(buf, 0, 4096);

            // send(clientSocket, display.c_str(), display.size() + 1, 0);
            continue;
        }

        // If client chooses the exit option:
        else if (strcmp(buf, "4") == 0)
        {
            // Send the exit message to client and break from the loop
            send(clientSocket, exit_client.c_str(), exit_client.size(), 0);
            cout << "Client disconnected " << endl;
            break;
        }

        // For any other user input that is not defined: Send an error message and re-prompt the menu
        else
        {
            send(clientSocket, err.c_str(), err.size() + 1, 0);
            send(clientSocket, display.c_str(), display.size() + 1, 0);
        }
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
