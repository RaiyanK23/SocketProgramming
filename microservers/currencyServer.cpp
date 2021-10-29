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

#define PORT 8050

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
    serverHint.sin_port = htons(8050);       // Convert from little to big endian

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

    sockaddr_in client;                      // Use to hold the client information (port / ip address)
    socklen_t clientLength = sizeof(client); // The size of the client information

    // Strings needed for program operations
    string comparison = "Welcome! Please select one of the following services: \n 1. Translator Service \n 2. Currency Converter Service \n 3. Voting \n 4. Exit \n";
    string err = "This currency is unforunately not supported. \n" + comparison;

    // Buf array to receive requests from indirection server and send responses to indirection server
    char buf[4096];

    // Enter a loop
    while (true)
    {
        memset(&client, 0, sizeof(client)); // Clear the client structure
        memset(buf, 0, 4096);               // Clear the receive buffer;

        // Receive the user currency request
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

        // Break up the char array by spaces
        const char *first = strtok(buf, " ");
        const char *second = strtok(NULL, " ");
        const char *third = strtok(NULL, " ");
        // Set the split array as strings
        string amount(first);
        double amnt = stod(amount); // cast this as a double
        string src_curr(second);
        string dest_curr(third);

        cout << amount << endl;
        cout << src_curr << endl;
        cout << dest_curr << endl;

        // Display message and client info
        char clientIp[256];       // Create enough space to convert the address byte array
        memset(clientIp, 0, 256); // to string of characters

        // Convert from byte array to chars
        inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

        // Display the message / who sent it
        cout << "Message recv from " << clientIp << " : " << buf << endl;

        // <------------- Checking the user input and doing operations -------------->

        if (src_curr.compare("CAD") == 0 && (dest_curr.compare("USD") == 0 || dest_curr.compare("US") == 0 || dest_curr.compare("usd") == 0))
        {
            // USD conversion
            double conversion = amnt * 0.81;
            // Send back as string
            string cad_to_usd = to_string(conversion).append(" USD") + "\n" + comparison;
            // Send result back to indirection server
            int sendOk = sendto(in, cad_to_usd.c_str(), cad_to_usd.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }

        else if (src_curr.compare("CAD") == 0 && (dest_curr.compare("EUR") == 0 || dest_curr.compare("eur") == 0))
        {
            // EURO conversion
            double conversion = amnt * 0.70;
            // Send back as a string
            string cad_to_euro = to_string(conversion).append(" EUR") + "\n" + comparison;
            // Send result back to the indirection server
            int sendOk = sendto(in, cad_to_euro.c_str(), cad_to_euro.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }

        else if (src_curr.compare("CAD") == 0 && (dest_curr.compare("GBP") == 0 || dest_curr.compare("gbp") == 0))
        {
            // GBP conversion
            double conversion = amnt * 0.59;
            // Send back as a string
            string cad_to_usd = to_string(conversion).append(" GBP") + "\n" + comparison;
            // Send result back to indirection server
            int sendOk = sendto(in, cad_to_usd.c_str(), cad_to_usd.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }

        else if (src_curr.compare("CAD") == 0 && (dest_curr.compare("BTC") == 0 || dest_curr.compare("btc") == 0))
        {
            // BTC conversion
            double conversion = amnt * 0.000013;
            // Send back as a string
            string cad_to_usd = to_string(conversion).append(" BTC") + "\n" + comparison;
            // Send result back to indirection server
            int sendOk = sendto(in, cad_to_usd.c_str(), cad_to_usd.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }
        // If user doesn't correct choose currencies, then show an error message
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