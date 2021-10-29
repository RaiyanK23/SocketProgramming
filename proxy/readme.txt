Author: Mohammed Kabir
UCID: 30092222
Ver: 1.2
Tutorial: T06

Welcome to this C++ program which is a proxy to filter certain blocked words.

Please ensure that you have selected an appropriate IP address and port for your convinience, this
will need to be changed in the code on lines 68 and 70 of the program based on the port and IP address you would like.
Next, configure a web browser to use a HTTP proxy and enter the port and IP address.

**IMPORTANT NOTE**: Ensure that "proxy.cpp" and "forbidden.txt" are in the same directory

To compile the proxy, please put the following in a linux terminal:

g++ -o proxy proxy.cpp

To run the proxy: 
./proxy

The program is now listening for connections, simply open any browser and connect to any HTTP website.

DYNAMIC BLOCKING:
1. Please open the "forbidden.txt" text file
2. Add or remove blocked words that you would like, ensuring each word is on its own line
3. Save the forbidden.txt file and refresh your browser to clear cache and see changes

**NOTE** 
The .txt file used for dynamic blocking MUST be called forbidden.txt
If there are no blocked words in the file, ensure that there is only one line in the file

FEATURES:
Handle HTTP GET interactions between client and servers
Ability to parse and redirect HTTP responses with forbidden keywords
Dynamically update list of key words including both adding and deleting blocked key words
Parse text ot html response to identify forbidden keywords and redirect client

Testing was conducted on my local machine (running linux) and on linux lab (CPSC servers)

CTRL (or CMD) + C as needed to terminate the program




