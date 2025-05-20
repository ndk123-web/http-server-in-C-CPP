#include <stdio.h>      // Standard I/O functions like printf, scanf
#include <sys/socket.h> // Core socket functions: socket(), bind(), listen(), accept(), send(), recv()
#include <netinet/in.h> // Structures for internet addresses: sockaddr_in, INADDR_ANY, htons()
#include <unistd.h>     // POSIX functions: close()
#include <string.h>     // String functions: strlen(), memset()

int main()
{
    // 1. Create a socket
    // socket() creates an endpoint for communication and returns a file descriptor (an integer)
    // file descriptor is just for OS to identify the socket in which is being used
    // AF_INET: Address Family - IPv4
    // SOCK_STREAM: Type - TCP (reliable, connection-oriented)
    // 0: Protocol - 0 means "choose the default protocol" for the given family and type (here, TCP)
    // For AF_INET + SOCK_STREAM, the default is TCP.
    // For AF_INET + SOCK_DGRAM, the default is UDP.    
//      Family	  Type	      Protocol	             Meaning
//      AF_INET	 SOCK_STREAM	0 or IPPROTO_TCP	TCP socket
//      AF_INET	 SOCK_DGRAM 	0 or IPPROTO_UDP	UDP socket
//      AF_INET	 SOCK_STREAM	5	                Invalid,error
    int sockerDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    // Check if socket creation was successful
    // If socket() returns -1, there was an error (e.g., not enough resources, permissions)
    if (sockerDescriptor < 0)
    {
        printf("Error in Creating Socket\n");
        return 0;
    }

    // 2. Prepare the server address structure
    // sockaddr_in is a structure specifically for IPv4 addresses
    struct sockaddr_in serverIpAddress;

    // Set the address family to IPv4
    serverIpAddress.sin_family = AF_INET;

    // Set the port number
    // htons() converts the port number from host byte order to network byte order (big-endian)
    // Port 8000 is arbitrary; it must not be used by another service
    serverIpAddress.sin_port = htons(8000);

    // Set the IP address to listen on
    // INADDR_ANY means "listen on all available network interfaces" (0.0.0.0)
    // This allows the server to accept connections from any of the host's IP addresses
    serverIpAddress.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind the socket to the specified IP address and port
    // bind() associates the socket with the address and port specified in serverIpAddress
    // The second parameter must be a pointer to a struct sockaddr, so we cast sockaddr_in* to sockaddr*
    // sizeof(serverIpAddress) tells bind() how much memory to read for the address
    // If bind() fails (returns -1), the address/port may already be in use or you may lack permissions
    if (bind(sockerDescriptor, (struct sockaddr *)&serverIpAddress, sizeof(serverIpAddress)) < 0) {
        printf("Error in Binding\n");
        return 0;
    }

    // 4. Listen for incoming connections
    // listen() marks the socket as passive, meaning it will be used to accept incoming connection requests
    // The second argument (5) is the backlog: the maximum number of pending connections the queue will hold
    // If more clients try to connect, they may be refused or experience delays
    listen(sockerDescriptor, 5);

    printf("Listening to the port 8000 and Waiting for Connection\n");

    // 5. Accept a connection from a client
    // accept() blocks and waits for an incoming connection
    // When a client connects, accept() returns a new socket descriptor for communication with that client
    // The original sockerDescriptor continues to listen for new connections
    // The second and third parameters can be used to get the client's address, but we pass NULL since we don't need it
    int newClientSockerDescriptor = accept(sockerDescriptor, NULL, NULL);

    // Check if accept() was successful
    if (newClientSockerDescriptor < 0)
    {
        printf("Error in Accepiting Connection\n");
        return 0;
    }

    // 6. Receive data from the client
    // recv() reads data sent by the client and stores it in buffer
    // The third argument is the maximum number of bytes to read (buffer size)
    // The last argument (0) means "no special flags"
    // recv() returns the number of bytes actually received, or -1 on error
    char buffer[1024];
    int bytes_received = recv(newClientSockerDescriptor, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        // Null-terminate the received data to safely print as a string
        buffer[bytes_received] = '\0';
        printf("Received Data from Client: \n%s\n", buffer);
    } else {
        printf("No data received or error.\n");
    }

    // 7. Prepare and send an HTTP response to the client
    // HTTP/1.1 200 OK: Status line indicating success
    // Content-Type: text/plain: Tells the client the type of data being sent
    // \r\n: Required by HTTP o end each header line
    // The blank line (\r\n) after headers signals the end of headers and start of the body
    // "Hello From Server" is the body of the response
    char response[] = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html\r\n"
                      "\r\n"
                      "<!DOCTYPE html>\n"
                      "<html> <body> <h1> Hello From Server </h1> </body> </html>\n";
    // send() transmits the response to the client
    // strlen(response) ensures only the actual response is sent (not extra memory)
    int bytes_sent = send(newClientSockerDescriptor, response, strlen(response), 0);
    if (bytes_sent < 0) {
        printf("Error sending response\n");
    } else {
        printf("Response Sent to Client: \n %s \n", response);
    }

    // 8. Close the client socket
    // This ends the connection with the current client
    close(newClientSockerDescriptor);
    printf("Connection Closed\n");

    // 9. Close the server socket
    // This releases the port and resources used by the server
    close(sockerDescriptor);
    printf("Server Closed\n");

    return 0;
}