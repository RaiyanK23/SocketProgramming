#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    // Check to see that user provides 3 command line arguments
    if (argc != 3)
    {
        perror("usage: ./client <Server IP> <Port Number>\n");
        exit(1);
    }
    //	Create a client-side TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // Error checking
    if (sock == -1)
    {
        return 1;
    }

    //	Create a hint structure for the server we're connecting with

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_addr.s_addr = inet_addr(argv[1]); // User input for IP address
    hint.sin_port = htons(atoi(argv[2]));      // User input for port

    //	Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr *)&hint, sizeof(hint));
    // Error checking
    if (connectRes == -1)
    {
        printf("Connect to %s %s failed!\n", argv[1], argv[2]);
        return 1;
    }
    else
        printf("Connected to server %s %s!\n", argv[1], argv[2]);

    // char buf array used for storing recv values and also sending to server
    char buf[4096];
    // General strings used for program operations
    string userInput;
    string input;
    string id_input;
    string comparison = "Welcome! Please select one of the following services: \n 1. Translator Service \n 2. Currency Converter Service \n 3. Voting \n 4. Exit \n";
    string encrypt; // Stores the encrypt key
    string request = "LIST";

    //	While loop:
    while (1)
    {

        //		Wait for response
        memset(buf, 0, 4096);
        int bytesReceived = recv(sock, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cout << "There was an error getting response from server\n";
        }
        else
        {
            //		Display response
            if (strcmp(buf, "5") == 0)
            {
            }
            else
            {
                cout << "SERVER> " << string(buf, bytesReceived) << "\n";
            }
        }

        // Close the socket and exit the program upon termination
        if (strcmp(buf, "Server connection terminated.") == 0)
        {
            close(sock);
            exit(0);
        }
        // User input needed for translaion micro server
        else if (strcmp(buf, "Enter an English word: \n") == 0)
        {
            getline(cin, input);
            send(sock, input.c_str(), input.size() + 1, 0);
            continue;
        }
        // User input needed for currecy micro server
        else if (strcmp(buf, "Please enter the desired currency conversion in the following: <Amount> <Source Currency> <Destination Currency>") == 0)
        {
            getline(cin, input);
            send(sock, input.c_str(), input.size() + 1, 0);
            continue;
        }
        // User input needed for voting micro server
        else if (strcmp(buf, "Choose 1 of the following: \n 1. Show Candidates \n 2. Secure Voting \n 3. Voting Summary") == 0)
        {
            getline(cin, input);
            send(sock, input.c_str(), input.size() + 1, 0);
            continue;
        }

        else if (strcmp(buf, "5") == 0)
        {
            // Request a list of candidates from micro server
            encrypt = buf;
            int sendRes = send(sock, request.c_str(), request.size() + 1, 0);
            // error checking
            if (sendRes == -1)
            {
                cout << "Could not send to server! Whoops!\r\n";
                continue;
            }

            int bytesReceived = recv(sock, buf, 4096, 0);
            if (bytesReceived == -1)
            {
                cout << "There was an error getting response from server\n";
            }

            else
            {
                //		Display response
                cout << "SERVER> " << string(buf, bytesReceived) << "\n";
            }

            getline(cin, id_input);
            // Encoding the candidate ID with encryption key
            int id_input1 = stoi(id_input);
            int encrypt_num = stoi(encrypt);
            int result = id_input1 * encrypt_num;
            string encoded = to_string(result);
            // Sending encrypted key back to micro server
            send(sock, encoded.c_str(), encoded.size() + 1, 0);
            continue;
        }
        // Get user input for the prompted menu
        getline(cin, userInput);
        int sendRes = send(sock, userInput.c_str(), userInput.size() + 1, 0);
        if (sendRes == -1)
        {
            cout << "Could not send to server! Whoops!\r\n";
            continue;
        }
    }

    //	Close the socket
    close(sock);

    return 0;
}
