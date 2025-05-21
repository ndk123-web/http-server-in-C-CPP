#include <stdio.h>      // For standard input/output operations (printf, etc.)
#include <sys/socket.h> // For socket-related functions (socket, bind, listen, etc.)
#include <netinet/in.h> // For internet domain addresses (struct sockaddr_in)
#include <string.h>     // For string manipulation functions (strlen, etc.)
#include <arpa/inet.h>  // For inet_ntop which converts IP addresses to human-readable strings
#include <unistd.h>     // For close() function to close file descriptors/sockets
#include <pthread.h>    // For threading capabilities (pthread_create, pthread_detach)
#include <stdlib.h>     // For memory allocation functions (malloc, free)

/**
 * Thread function that handles sending HTTP responses to clients
 * This function runs in a separate thread for each client connection
 * @param arg - Pointer to client socket descriptor (passed as void* for pthread compatibility)
 */
void *threadSendResponse(void *arg)
{
    // Cast and extract the client socket descriptor from the argument
    int clientSocket = *(int *)arg;
    free(arg); // Free the memory allocated for the socket descriptor in main()

    // HTTP response with headers and body
    // Note: \r\n line endings are required by HTTP protocol
    // The double \r\n after headers marks the separation between headers and body
    char responseData[] = "HTTP/1.1\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n"
                          "This is Server";

    printf("It's Thread for client Socket: %d \n", clientSocket);

    // Send the HTTP response to the client using their socket
    send(clientSocket, responseData, strlen(responseData), 0);

    // Close the client connection after sending response
    close(clientSocket);

    printf("Client req Socket: %d disconnected \n\n", clientSocket);

    return NULL; // Required return for pthread functions
}

int main()
{
    // Create a TCP/IPv4 socket
    // AF_INET specifies IPv4, SOCK_STREAM specifies TCP, 0 is protocol (default)
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Check if socket creation was successful
    if (serverSocket < 0)
    {
        printf("There is error in creating the server Socket\n");
        return 0;
    }

    // Configure server address structure for binding
    struct sockaddr_in serverIp;

    serverIp.sin_family = AF_INET;         // Address family (IPv4)
    serverIp.sin_port = htons(8000);       // Port number (converted to network byte order)
    serverIp.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces (0.0.0.0)

    // Bind the socket to the configured address and port
    // This associates the socket with a specific address and port number
    bind(serverSocket, (struct sockaddr *)&serverIp, sizeof(serverIp));

    printf("Listening on port 8000\n");

    // Start listening for incoming connections
    // The second parameter (5) is the backlog - maximum pending connections in queue
    listen(serverSocket, 5);

    // Infinite loop to keep accepting client connections
    while (1)
    {
        // Prepare structures to store client information
        struct sockaddr_in clientIp;         // Structure to store client's address info
        int clientIpSize = sizeof(clientIp); // Size of the structure (needed for accept)

        // Accept an incoming client connection
        // This creates a new socket dedicated to this specific client connection
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientIp, &clientIpSize);
        printf("Client Socket: %d \n", clientSocket);

        // Buffer to store client's IP address as a string
        char clientIpPrint[INET_ADDRSTRLEN];

        // Convert the client's binary IP address to a human-readable string
        inet_ntop(AF_INET, &(clientIp.sin_addr), clientIpPrint, INET_ADDRSTRLEN);

        // Convert client's port from network to host byte order
        int clientPort = ntohs(clientIp.sin_port);

        printf("Client Connected with IPV4: %s and Port: %d \n", clientIpPrint, clientPort);

        // Buffer to receive HTTP request data from client
        char requestData[1024];

        // Receive data from the client and store it in the buffer
        recv(clientSocket, requestData, sizeof(requestData), 0);

        printf("Request Data From Client: \n %s", requestData);

        // Create a new thread to handle sending the response
        // This allows the server to immediately go back to accepting new connections
        pthread_t t;

        // Allocate memory for client socket descriptor to pass to thread
        // This is necessary because the clientSocket variable will be reused in the main loop
        int *newClientSocket = malloc(sizeof(int));
        *newClientSocket = clientSocket;

        // Create a new thread that will execute threadSendResponse function
        int checkThread = pthread_create(&t, NULL, threadSendResponse, newClientSocket);

        // Detach the thread so its resources are automatically released when it finishes
        pthread_detach(t);

        // Check if thread creation was successful
        if (checkThread != 0)
        {
            printf("Error in Creating Thread For Client: %d \n", clientSocket);
            return 1;
        }
    }

    // This code will never execute due to the infinite loop above
    close(serverSocket);
    printf("Server Shuts Down");

    return 0;
}