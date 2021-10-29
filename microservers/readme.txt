Author: Mohammed Kabir
UCID: 30092222
Ver: 1.4
Tutorial: T06

Welcome to this C++ program which connects a client to an indirection server using a TCP connection and connects the indirection
server to microservers using a UDP connection.

Please ensure that you have selected an appropriate IP address to connect to the UDP servers, these may need to be changed on lines
120, 202 and 256 in indrectionServer.cpp, they are currently configured for this IP address: 136.159.5.25 (CPSC servers).

*Note*: Each file will require its own console/terminal

1. Compile and run all three microservers (the compilation of the servers can be done in any order):

Translation microserver: 
g++ -o translation translationServer.cpp
./translation

Currency Conversion microserver:
g++ -o currency currencyServer.cpp
./currency

Voting microserver:
g++ -o voting votingServer.cpp
./voting

2. Compile and run the indirection server:
g++ -o indirection indirectionServer.cpp
./indirection

3. Compile and run the client:
g++ -o client client.cpp
./client <IP ADDRESS> <PORT>

**NOTE**: the indirection server will listen on port 8989, so please choose 8989
as the port to connect to when running the client, the IP address will be the IP
of the local network (run an ifconfig).


On the client terminal/console, the user will be prompted to enter numbers that correspond
to the microservices or exit the program. Please choose any one of the options and follow
the instructions.

Translation micro server translates to french and supports the 5 english words: Hello, Boy, Girl, Cat, Dog

Currency micro server supports conversion of CAD to: EUR, USD, GBP and BTC



FEATURES:
Client and indirection server connection using TCP connection and interacts with one another
Indirection server and micro servers are connected using UDP connection and interact with one another
3 micro (UDP servers) that offer 3 services: translation (in French), currency conversion and voting
Indirection server redirects client requests to UDP server and sends UDP server response back to client
Supports running 3 micro servers on one system and running client and indirection server on a different system


Testing was conducted on my LAN (running linux) and on linux lab (CPSC servers)

CTRL (or CMD) + C as needed to terminate the program(s)




