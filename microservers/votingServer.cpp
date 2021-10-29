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

#define PORT 8100

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
    serverHint.sin_port = htons(8100);       // Convert from little to big endian

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
    // Strings needed for program
    string comparison = "Welcome! Please select one of the following services: \n 1. Translator Service \n 2. Currency Converter Service \n 3. Voting \n 4. Exit \n";
    string no_votes = "You have not voted yet. \n" + comparison;
    string thank_you = "Thank you for voting! \n " + comparison;
    string err = "That is not a valid request. \n" + comparison;
    // Candidate names and Id's
    string candidate1 = "Kanye West 10";
    int cand1_id = 10;
    string candidate2 = "John Doe 20";
    int cand2_id = 20;
    string candidate3 = "Tupac Shakur 30";
    int cand3_id = 30;
    string candidate4 = "Natalie Portman 40";
    int cand4_id = 40;
    // Candidate votes
    int cand1_res = 50;
    int cand2_res = 80;
    int cand3_res = 90;
    int cand4_res = 100;

    string display = '\n' + candidate1 + '\n' + candidate2 + '\n' + candidate3 + '\n' + candidate4 + '\n' + '\n' + comparison;
    string vote_display = '\n' + candidate1 + '\n' + candidate2 + '\n' + candidate3 + '\n' + candidate4 + '\n' + "Enter a candidate's ID to vote" + '\n';
    // Key used for encryption
    string encrypt_key = "5";
    // Counter to see if user has voted or not
    int count_votes = 0;

    // Buff array to hold the request from indirect server and send the response back to it
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
        // If user input is 1, display the candidates and ID's
        if (strcmp(buf, "1") == 0)
        {
            int sendOk = sendto(in, display.c_str(), display.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }
        // If user input is 2, send the encryption key
        else if (strcmp(buf, "2") == 0)
        {
            int sendOk = sendto(in, encrypt_key.c_str(), encrypt_key.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }
        // If user requests a list, show them candidates and prompt them to input an ID
        else if (strcmp(buf, "LIST") == 0)
        {
            int sendOk = sendto(in, vote_display.c_str(), vote_display.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }
        // <------------- Encryption decoding------------>
        else if (strcmp(buf, "50") == 0)
        {
            // decoding...
            int secret = stoi(buf);
            int decoder = secret / stoi(encrypt_key);
            if (decoder == cand1_id)
            {
                cand1_res += 1;
                count_votes++;
                cout << "The value of cand1_votes is: " << cand1_res << endl;
            }

            int sendOk = sendto(in, thank_you.c_str(), thank_you.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }

        else if (strcmp(buf, "100") == 0)
        {
            // decoding...
            int secret = stoi(buf);
            int decoder = secret / stoi(encrypt_key);
            if (decoder == cand2_id)
            {
                cand2_res += 1;
                count_votes++;
                cout << "The value of cand2_votes is: " << cand2_res << endl;
            }
            int sendOk = sendto(in, thank_you.c_str(), thank_you.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }

        else if (strcmp(buf, "150") == 0)
        {
            // decoding...
            int secret = stoi(buf);
            int decoder = secret / stoi(encrypt_key);
            if (decoder == cand3_id)
            {
                cand3_res += 1;
                count_votes++;
                cout << "The value of cand3_votes is: " << cand3_res << endl;
            }
            int sendOk = sendto(in, thank_you.c_str(), thank_you.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }

        else if (strcmp(buf, "200") == 0)
        {
            // decoding...
            int secret = stoi(buf);
            int decoder = secret / stoi(encrypt_key);
            if (decoder == cand4_id)
            {
                cand4_res += 1;
                count_votes++;
                cout << "The value of cand4_votes is: " << cand4_res << endl;
            }
            int sendOk = sendto(in, thank_you.c_str(), thank_you.size() + 1, 0, (sockaddr *)&client, sizeof(client));
            if (sendOk == -1)
            {
                perror("Sendto() has failed.");
            }
        }

        // <--------------------------- End of encryption decoding ----------------------------------------------->

        // Show candidate results only if user has voted at least once
        else if (strcmp(buf, "3") == 0)
        {
            string display_all = '\n' + candidate1 + " " + " Votes:" + to_string(cand1_res) + '\n' + candidate2 + " " + " Votes:" + to_string(cand2_res) + '\n' + candidate3 + " " + " Votes:" + to_string(cand3_res) + '\n' + candidate4 + " " + "Votes:" + to_string(cand4_res) + '\n' + '\n' + comparison;

            if (count_votes == 0)
            {
                int sendOk = sendto(in, no_votes.c_str(), no_votes.size() + 1, 0, (sockaddr *)&client, sizeof(client));
                if (sendOk == -1)
                {
                    perror("Sendto() has failed.");
                }
            }

            else
            {
                int sendOk = sendto(in, display_all.c_str(), display_all.size() + 1, 0, (sockaddr *)&client, sizeof(client));
                if (sendOk == -1)
                {
                    perror("Sendto() has failed.");
                }
            }
        }
        // show error message for any other inputs
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