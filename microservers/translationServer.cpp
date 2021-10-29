#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#define PORT 8010

using namespace std;

int main()
{

    ////////////////////////////////////////////////////////////
    // SOCKET CREATION AND BINDING
    ////////////////////////////////////////////////////////////

    // Create a socket, notice that it is a user datagram socket (UDP)
    int in = socket(AF_INET, SOCK_DGRAM, 0);

    // Create a server hint structure for the server
    sockaddr_in serverHint;
    serverHint.sin_addr.s_addr = INADDR_ANY; // Us any IP address available on the machine
    serverHint.sin_family = AF_INET;         // Address format is IPv4
    serverHint.sin_port = htons(PORT);       // Convert from little to big endian

    // Try and bind the socket to the IP and port
    if (bind(in, (sockaddr *)&serverHint, sizeof(serverHint)) == -1)
    {
        perror("Bind() has failed");
        exit(1);
    }
    else
    {
        printf("Server now running on port %d\n", PORT);
    }

    // Strings needed for program operations
    sockaddr_in client;                      // Use to hold the client information (port / ip address)
    socklen_t clientLength = sizeof(client); // The size of the client information
    string comparison = "Welcome! Please select one of the following services: \n 1. Translator Service \n 2. Currency Converter Service \n 3. Voting \n 4. Exit \n";
    string err = "This word is unforunately not supported. \n" + comparison;

    // Char buf array to hold the request from indirection server and to send response
    char buf[4096];

    // Enter a loop
    while (true)
    {
        memset(&client, 0, sizeof(client)); // Clear the client structure
        memset(buf, 0, 4096);               // Clear the receive buffer;

        // Wait for message
        int bytesIn = recvfrom(in, buf, 4096, 0, (sockaddr *)&client, &clientLength);
        if (bytesIn == -1)
        {
            perror("Failed to receive.");
            exit(1);
        }
        else
        {
            cout << "Received from client: " << buf << endl;
        }

        // Display message and client info
        char clientIp[256];       // Create enough space to convert the address byte array
        memset(clientIp, 0, 256); // to string of characters

        // Convert from byte array to chars
        inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

        // Display the message / who sent it
        cout << "Message recv from " << clientIp << " : " << buf << endl;

        // Translation options
        string hello = "French translation: Bonjour \n" + comparison;
        string boy = "French translation: Garcon \n" + comparison;
        string girl = "French translation: Fille \n" + comparison;
        string cat = "French translation: Chat \n" + comparison;
        string dog = "French translation: Chien \n" + comparison;

        // See if user input is hello, send back the translation
        if (strcmp(buf, "Hello") == 0 || strcmp(buf, "hello") == 0)
        {
            int sendOk = sendto(in, hello.c_str(), hello.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }
        // See if user input is boy, send back the translation
        else if (strcmp(buf, "Boy") == 0 || strcmp(buf, "boy") == 0)
        {
            int sendOk = sendto(in, boy.c_str(), boy.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }
        // See if user input is girl, send back the translation
        else if (strcmp(buf, "Girl") == 0 || strcmp(buf, "girl") == 0)
        {
            int sendOk = sendto(in, girl.c_str(), girl.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }
        // See if user input is cat, send back the translation
        else if (strcmp(buf, "Cat") == 0 || strcmp(buf, "cat") == 0)
        {
            int sendOk = sendto(in, cat.c_str(), cat.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }
        // See if user input is dog, send back the translation
        else if (strcmp(buf, "Dog") == 0 || strcmp(buf, "dog") == 0)
        {
            int sendOk = sendto(in, dog.c_str(), dog.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }
        // For any other inputs, display an error message
        else
        {
            int sendOk = sendto(in, err.c_str(), err.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }
    }

    // Close socket
    close(in);

    return 0;
}