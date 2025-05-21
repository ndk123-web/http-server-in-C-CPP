#include <iostream>
#include <sys/socket.h> // for creating , sending , binding , recv
#include <netinet/in.h> // for creatng ipv4
#include <string.h>

using namespace std;

int main()
{

    // 1. Create serverSocket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        cout << "Error in Creating the Socket" << endl;
        return 0;
    }

    // if server socket success

    // create ipv4 server address and port
    struct sockaddr_in serverIpAddress;

    serverIpAddress.sin_family = AF_INET;
    serverIpAddress.sin_port = htons(8000);
    serverIpAddress.sin_addr.s_addr = INADDR_ANY;

    // now bind the ipv4 server address port with tcp socket
    bind(serverSocket, (struct sockaddr *)&serverIpAddress, sizeof(serverIpAddress));

    cout << "Server is Listening on Port 8000" << endl;

    // now listen
    // it means ipv4 now can listen any req in this serverSocket which
    listen(serverSocket, 5);

    int newClientReq = accept(serverSocket, nullptr, nullptr);

    if (newClientReq < 0)
    {
        cout << "New Client Socket Failed ";
        return 0;
    }

    char buffer[1024];
    recv(newClientReq, &buffer, sizeof(buffer), 0);

    cout << "Req From Client: " << buffer << endl;

    char response[] = "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/plain\r\n"
                     "\r\n"
                     "Hello From Server";

    send(newClientReq, &response, strlen(response), 0);

    return 0;
}