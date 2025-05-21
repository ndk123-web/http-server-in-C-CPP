#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h> // needed for inet_ntop which converts the ip4 and assign to string
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h> // it has free()

// #define INET_ADDRSTRLEN 16

void *threadSendResponse(void *arg)
{

    int clientSocket = *(int *)arg;
    free(arg);

    // \r\n is imporatant for the http protocol
    // in third \r\n tells that header completed now body starts
    char responseData[] = "HTTP/1.1\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n"
                          "This is Server";

    printf("It's Thread for client Socket: %d \n",clientSocket);
 
    // send the request to client using client Socket
    send(clientSocket, responseData, strlen(responseData), 0);
    close(clientSocket);

    printf("Client req Socket: %d disconnected \n\n",clientSocket);

    return NULL;
}

int main()
{

    // created the socket of TCP IPV4
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // if negative then error
    if (serverSocket < 0)
    {
        printf("There is error in creating the server Socket\n");
        return 0;
    }

    // created the actual IP of localhost or 127.0.0.1 with port 8000
    struct sockaddr_in serverIp;

    serverIp.sin_family = AF_INET;
    serverIp.sin_port = htons(8000);
    serverIp.sin_addr.s_addr = INADDR_ANY; // localhost

    // bind the ipv4 and tcp
    bind(serverSocket, (struct sockaddr *)&serverIp, sizeof(serverIp));

    printf("Listening on port 8000\n");

    // it waits until req
    // maximum 5 clients can connect
    listen(serverSocket, 5);

    while (1)
    {

        // whether to check is client enter or not
        struct sockaddr_in clientIp;         // for getting client Info
        int clientIpSize = sizeof(clientIp); // for getting size of clientIp

        // it creates a bridge btw server socket and clientSocket
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientIp, &clientIpSize);
        printf("Client Socket: %d \n",clientSocket);

        // create buffer to get the ip4 address
        char clientIpPrint[INET_ADDRSTRLEN];

        // parse the ip4 address and after convert put that into buffer
        inet_ntop(AF_INET, &(clientIp.sin_addr), clientIpPrint, INET_ADDRSTRLEN);

        // parse the port and put that into clientPort
        int clientPort = ntohs(clientIp.sin_port);

        printf("Client Connected with IPV4: %s and Port: %d \n", clientIpPrint, clientPort);

        // getting req and showing in terminal
        char requestData[1024];
        recv(clientSocket, requestData, sizeof(requestData), 0);

        printf("Request Data From Client: \n %s", requestData);

        // to send data we will use the threads
        pthread_t t;

        int *newClientSocket = malloc(sizeof(int));
        *newClientSocket = clientSocket;

        int checkThread = pthread_create(&t, NULL, threadSendResponse, newClientSocket);
        pthread_detach(t);  // when t thread completes automatically close the thread
        if (checkThread != 0)
        {
            printf("Error in Creating Thread For Client: %d \n", clientSocket);
            return 1;
        }
    }

    close(serverSocket);
    printf("Server Shuts Down");

    return 0;
}

// Most Important 

/*
    1. Each Request From Client Browser has unique tcp socket (whether its same user pr not)
        - EX : user1 (req1)-> 1 ,  user2 (req1)-> 2 , user1 (req2) -> 3
    2. if Same User sending data then ip will be same but port will be diff for diff req (talking about source ip and source port)
        - EX: user1 (req1 for index.html)-> 127.0.0.1:32232 , user1 (req2 for favico.co)-> 127.0.0.1:32235
    3. Browser sends multiple tcp req to the server for its own performance
        - EX: like for dummy req inside that no http req server waiting 
*/