#include <stdio.h>      // has functions like printf , scanf
#include <sys/socket.h> // has socket functions
#include <netinet/in.h> // has socketaddr_in function which which is used to create ip4 address
#include <unistd.h>     // has close function
#include <string.h>     // has strlen function which is used to get the length of the string

int main()
{

    // here 0 means use default protocol which is specified by the socket type
    // here SOCK_STREAM means we are using TCP protocol
    // here SOCK_DGRAM means we are using UDP protocol
    // here AF_INET means we are using IPv4 address
    // here AF_INET6 means we are using IPv6 address
    //   if we get -1 then there is an error in creating socket
    //   else it returns a positive integer which is the socket descriptor
    //        which is used to identify the socket
    int sockerDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (sockerDescriptor < 0)
    {
        printf("Error in Creating Socket\n");
        return 0;
    }

    // now we need to create the ip4 address
    struct sockaddr_in serverIpAddress;

    serverIpAddress.sin_family = AF_INET;         // here we are using IPv4 address
    serverIpAddress.sin_port = htons(8000);       // here we are using port 8000
    serverIpAddress.sin_addr.s_addr = INADDR_ANY; // any address which listens to the port 8000

    // now we need to bind the socket to the address which are listening to the port 8000
    // do u have quetion ? bind second parameter expects sockaddr address
    // but we have created sockaddr_in address
    // ans -> sockaddr_in and sockaddr has same fields that only can be used by bind function
    // ans -> so it's safe to cast sockaddr_in to sockaddr address
    // so we can use sockaddr_in address as sockaddr address
    // we can also say that sockaddr_in is compatible with sockaddr
    bind(sockerDescriptor, (struct sockaddr *)&serverIpAddress, sizeof(serverIpAddress));

    // now we need to listen to the port 8000
    // here 5 is the maximum number of connections that can be queued
    listen(sockerDescriptor, 5);

    printf("Listening to the port 8000 and Waiting for Connection\n");

    // now we need to accept the connection from the client
    // here we are creating another socket for client that is connecting to the server i.e.sockerDescriptor
    /*
        The accept() function creates a new socket for the incoming client connection.
        This new socket (newClientSockerDescriptor) is used for communication with that specific client.
        Your original sockerDescriptor (the server socket) continues listening for more connections.
    */
    // second parameter is used to get the address of the client
    // third parameter is used to get the size of the address of the client
    // for now we are passing NULL means we are not interested in the address of the client
    int newClientSockerDescriptor = accept(sockerDescriptor, NULL, NULL);

    if (newClientSockerDescriptor < 0)
    {
        printf("Error in Accepiting Connection\n");
        return 0;
    }

    /*
        newClientSockerDescriptor: The socket connected to the client.
        buffer: Where the received data will be stored.
        sizeof(buffer): Maximum number of bytes to receive.
        0 (last parameter): Flags.
        0 means "no special options", just receive normally.
        So, this line receives data from the client and puts it into buffer.
        The 0 is standard for most simple use cases.
    */
    char buffer[1024];
    recv(newClientSockerDescriptor, buffer, sizeof(buffer), 0);

    printf("Received Data from Client: \n %s \n", buffer);

    // now we need to send the data to the client via client socket (newClientSockerDescriptor)
    // make sure to send the length of data not bytes (use strlen function)
    // http response must end with \r\n  which is carraige return and line feed
    // that is how the http response is formed
    // same for the http request 
    char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello From Server";
    send(newClientSockerDescriptor, response, strlen(response), 0);

    printf("Response Sent to Client: \n %s \n", response);

    // now we need to close the socket
    close(newClientSockerDescriptor);
    printf("Connection Closed\n");

    close(sockerDescriptor);
    printf("Server Closed\n");


    return 0;
}