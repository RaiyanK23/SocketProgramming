#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <cstdio>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unordered_set>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <fstream>

// Author: Mohammed Kabir
// UCID: 30092222
// Ver: 1.2
// Tutorial: T06

// Global variable
int global_client_sock;

using namespace std;

int main()
{
    // FILE object needed to read from our file
    FILE *forbidden_file;

    // Structure for server-side proxy setup
    struct sockaddr_in server_socket;
    // Structure for client-side proxy setup
    struct sockaddr_in client_socket;

    // Hold the return value of the server socket creation
    int server_proxy;
    int recv_bytes;

    socklen_t size_of_sin;
    // Initialize an unordered set to hold the values of the forbidden/blocked words
    unordered_set<string> forbidden = {""};

    // String that will hold the value of the parsed hostname
    string parsed_host;
    // String that will hold the value of the parsed path
    string parsed_path;
    // String that will hold the value of the parsed URL
    string parsed_URL;
    // String that will hold the value of the parsed http request
    string parsed_http;

    // Char array to store client request
    char client_req[2048];
    // Char array for forbidden
    char forbidden_buffer[2048];

    // Will need for forking
    pid_t pid;

    // <---------------------------- Start of server-side proxy code ---------------------------------------->

    // Clear the structure
    memset(&server_socket, 0, sizeof(server_socket));
    // Choose IPV4
    server_socket.sin_family = AF_INET;
    //Choose any port of your choice
    server_socket.sin_port = htons(12345);
    // Use the appropriate IP addr or localhost
    server_socket.sin_addr.s_addr = inet_addr("192.168.48.133");

    // Create server-side proxy socket and error check
    if ((server_proxy = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        // Debugging..
        puts("Failure to create the socket\n");
        exit(1);
    }

    int checker = 1;

    //Open the port
    if (setsockopt(server_proxy, SOL_SOCKET, SO_REUSEADDR, &checker, sizeof(int)) == -1)
    {
        // Debugging..
        puts("Failure in server socket.\n");
        exit(1);
    }

    //Use the bind method to bind the server-side socket to an IP and port
    if (bind(server_proxy, (struct sockaddr *)&server_socket, sizeof(struct sockaddr)) == -1)
    {
        // Debugging..
        puts("bind() has failed\n");
        exit(1);
    }

    //Listening for a connection on the specific port and IP
    if (listen(server_proxy, 10) == -1)
    {
        // Debugging..
        puts("Server listen failed\n");
        exit(1);
    }

    //Get size of sockaddr_in for the accept
    size_of_sin = sizeof(struct sockaddr_in);
    puts("Listening for a connection...");

    // Loop so that program does not terminate after 1 connection
    while (true)
    {
        // Clear the set
        forbidden.clear();
        // Read input of forbidden.txt
        forbidden_file = fopen("forbidden.txt", "r");
        //Add words from the .txt file
        while (fgets(forbidden_buffer, 2048, forbidden_file) != NULL)
        {
            // Add /0 to ensure things are in proper place
            forbidden_buffer[strcspn(forbidden_buffer, "\n")] = '\0';

            // Insert into th set
            forbidden.insert(forbidden_buffer);

            memset(forbidden_buffer, 0, 2048);
        }

        // Close file once done with file-related operations
        fclose(forbidden_file);

        //Output the blocked words
        printf("Forbidden Words: ");
        for (string a : forbidden)
        {
            printf("%s   ", a.c_str());
        }

        // Make a new line
        printf("\n");

        //Accept a connection from web browser/client
        if (global_client_sock = accept(server_proxy, (struct sockaddr *)&client_socket, &size_of_sin) == -1)
        {
            // Debugging...
            puts("client accept failed\n");
            exit(1);
        }

        printf("Received a connection from %s\n", inet_ntoa(client_socket.sin_addr));

        // <---- Used code for Professor Carey A0 for pid and fork ------------------------->
        //Create a child process to handle multiple connections
        pid = fork();

        if (pid < 0)
        {
            fprintf(stderr, "could not use fork()\n");
            exit(1);
        }

        else if (pid == 0)
        {

            close(server_proxy);

            //Creating char arrays for requests
            char request_host[2048];
            // Header files if needed
            char file_header[2048];
            // The response
            char response_host[2048];

            int x;

            // Use the recv() method to receive the GET request and additional information
            if ((recv_bytes = recv(global_client_sock, client_req, 2048, 0)) > 0)
            {
                //Parsing Request
                char copy_arr[2048];
                // Copy the requested array into the copy array
                strcpy(copy_arr, client_req);
                // Check for the tokens in the copy_arr
                char *request_ptr = strtok(copy_arr, "\r\n");

                // If the strcmp returns true then we handle the GET request
                if (strcmp(strtok(request_ptr, " "), "GET") == 0)
                {
                    // Truncates NULL with a space for the URL
                    parsed_URL = strtok(NULL, " ");

                    // Truncates NULL with a space for the http type
                    parsed_http = strtok(NULL, " ");
                    // Searching for the http://
                    if (strstr(parsed_URL.c_str(), "http://") != NULL)
                    {
                        // If found then parse it for the host name
                        parsed_URL = &parsed_URL[7];
                        // Loop through
                        for (x = 0; x < parsed_URL.length(); x++)
                        {
                            if (parsed_URL[x] == '/')
                            {
                                parsed_host = parsed_URL.substr(0, x);
                                // Print the host as a c-str
                                printf("The host is:  %s\n", parsed_host.c_str());

                                break;
                            }
                        }

                        parsed_path = parsed_URL.substr(x, parsed_URL.length() - 1);
                        // <------------------- Provided in tutorial --------------->
                        //Convert host name to IP using the hostent struct
                        struct hostent *he;

                        if ((he = gethostbyname(parsed_host.c_str())) == NULL)
                        {
                            // gethostbyname failed
                            herror("gethostbyname");
                            exit(1);
                        }
                        // <---------------- End of provided code -------------------->

                        // <----------- Start of server-side proxy socket ----------------->
                        //Creating a new socket
                        int host_sockt;
                        struct sockaddr_in host_addr;

                        memset(&host_addr, 0, sizeof(host_addr));
                        // Choose IPV4 as the family
                        host_addr.sin_family = AF_INET;
                        // Listen on port 80
                        host_addr.sin_port = htons(80);
                        // Converting the host name to IP addr
                        host_addr.sin_addr = *((in_addr *)he->h_addr);

                        // Connect the client-side proxy socket
                        if ((host_sockt = socket(PF_INET, SOCK_STREAM, 0)) == -1)
                        {
                            // Debugging...
                            puts("Socket creation for client has failed.\n");
                            exit(1);
                        }

                        if (setsockopt(host_sockt, SOL_SOCKET, SO_REUSEADDR, &checker, sizeof(int)) == -1)
                        {
                            puts("Failed.\n");
                        }
                        // See if you can connect
                        if (connect(host_sockt, (struct sockaddr *)&host_addr, sizeof(host_addr)))
                        {
                            // Debugging...
                            puts("connect() has failed.");
                            exit(1);
                        }

                        //See if there's a forbidden word, and if there is, redirect the user to the error.html provided
                        for (string a : forbidden)
                        {
                            if (parsed_path.find(a) != string::npos || parsed_host.find(a) != string::npos)
                            {
                                // the path to error page
                                parsed_path = "/~carey/CPSC441/ass1/error.html";
                                // host of error page
                                parsed_host = "pages.cpsc.ucalgary.ca";
                                // Print to screen
                                printf("A forbiden word has been detected: \"%s\" , redirecting to: http://%s%s\n", a.c_str(), parsed_host.c_str(), parsed_path.c_str());

                                break;
                            }
                        }

                        // Reset the memory
                        memset(request_host, 0, sizeof(request_host));

                        //Creating send request
                        if (sprintf(request_host, "GET %s %s\r\nHost: %s\r\n\r\n", parsed_path.c_str(), parsed_http.c_str(), parsed_host.c_str()) < 0)
                        {
                            // Debugging... check for errors
                            puts("failed");
                            exit(1);
                        }

                        // Send the information that we have received
                        if (send(host_sockt, request_host, sizeof(request_host), 0) < 0)
                        {
                            // Debugging... check for errors
                            puts("send call failed");
                            exit(1);
                        }

                        //Getting a response from server
                        int server_chunks = 0;
                        // String to hold response from client
                        string response_client;
                        // String to hold the file header
                        string file_header;

                        // Clear both strings
                        response_client.clear();

                        file_header.clear();

                        // CLear the memory of the response host
                        memset(response_host, 0, sizeof(response_host));
                        //
                        response_client.clear();
                        // Receive in a while loop and check that it has not failed
                        while ((server_chunks = recv(host_sockt, response_host, sizeof(response_host), 0)) > 0)
                        {
                            response_client.append(response_host, server_chunks);

                            //Clear the response afterwards
                            memset(response_host, 0, sizeof(response_host));
                        }
                        // Set the file header to the response client as a substr
                        file_header = response_client.substr(0, response_client.find("\r\n\r\n"));
                        // set the response client to the response client as a substr but with a \r\n at the end
                        response_client = response_client.substr(0, response_client.find_last_of("\r\n"));
                        // Check to make sure the send is working as it should
                        if (send(global_client_sock, response_client.c_str(), response_client.length() + 1, 0) < 0)
                        {
                            // Debugging...
                            puts("send() failed");
                            exit(1);
                        }
                        // Reset the response as a precaution
                        memset(response_host, 0, sizeof(response_host));
                        // Finally close the client-side socket
                        close(host_sockt);
                        // <-------------- Client-side socket ends ---------------------------------->
                        exit(0);
                    }
                }
            }
            // Close global client socket and exit on a failure
            close(global_client_sock);
            exit(0);
        }

        else
        {
            // Close the socket on pid/fork-related failures
            close(global_client_sock);
        }
    }

    return 0;
}
