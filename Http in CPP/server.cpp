#include <iostream>     // For input/output operations (cout, endl)
#include <sys/socket.h> // Provides core socket functions (socket, bind, listen, accept, send, recv)
#include <netinet/in.h> // Defines structures for Internet addresses (sockaddr_in) and byte order conversion (htons)
#include <cstring>      // For string manipulation functions (strlen, memset)
#include <unistd.h>     // For POSIX API functions, specifically close() to close file descriptors/sockets
#include <string>       // For std::string and to_string for dynamic Content-Length

using namespace std;

int main()
{
    // 1. Socket Creation:
    // The 'socket()' function creates an endpoint for communication and returns a file descriptor (FD).
    // This FD is an integer handle that refers to an internal data structure maintained by the Operating System (OS).
    // This OS-maintained socket data structure will store all relevant information about this communication endpoint,
    // such as its type (TCP/UDP), current state, associated buffers, and eventually, the local and remote IP addresses and ports.
    // Initially, when 'socket()' is called, this socket FD does not have any specific IP address or port assigned to it.
    // It's like getting an unconfigured phone line.
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0)
    {
        cerr << "Error: Cannot create server socket." << endl; // Use cerr for error messages
        return 1;                                              // Return non-zero to indicate an error
    }

    // 2. Server Address Structure Setup:
    // The 'sockaddr_in' structure is used to define an IPv4 address and a port number.
    // This structure will hold the server's network address details.
    struct sockaddr_in serverIpAddress;

    // Clear the structure to ensure no garbage values are present.
    memset(&serverIpAddress, 0, sizeof(serverIpAddress));

    serverIpAddress.sin_family = AF_INET; // Set the address family to IPv4.
    // 'htons' (host to network short) converts the port number from the host's byte order
    // to the network byte order. This is crucial for network communication as data
    // on the network must adhere to a standard byte order.
    serverIpAddress.sin_port = htons(8000); // Set the port number to 8000.
    // 'INADDR_ANY' is a special IP address (0.0.0.0). It instructs the server to listen
    // for incoming connections on all available network interfaces of the machine
    // (e.g., Wi-Fi, Ethernet). This makes the server accessible from any device
    // that can reach this machine's IP address on port 8000.
    serverIpAddress.sin_addr.s_addr = INADDR_ANY;

    // 3. Binding the Socket:
    // The 'bind()' function assigns the 'serverIpAddress' (which contains the server's IP and port 8000)
    // to the 'serverSocket'.
    // After this step, the OS's internal entry for 'serverSocket' now stores this specific
    // local IP address and port 8000. This effectively makes the socket a "local endpoint"
    // that applications can connect to.
    // The OS also checks if the specified port (8000) is already in use by another application.
    if (bind(serverSocket, (struct sockaddr *)&serverIpAddress, sizeof(serverIpAddress)) < 0)
    {
        cerr << "Error: Failed to bind socket to port 8000." << endl;
        close(serverSocket); // Close the socket before exiting
        return 1;
    }

    cout << "Server socket bound to port 8000." << endl;

    // 4. Listening for Connections:
    // The 'listen()' function puts the 'serverSocket' into a passive listening state.
    // This means the socket is now ready to accept incoming connection requests from clients.
    // Internally, the OS maintains two queues for this listening socket:
    // 1) SYN queue (or incomplete connections queue): This queue holds incoming connection
    //    requests (SYN packets) for which the 3-way handshake is not yet complete.
    //    The server has received the client's SYN but has not yet received the client's final ACK.
    // 2) Accept queue (or completed connections queue): This queue holds connections for which
    //    the 3-way handshake has been successfully completed. These connections are now
    //    waiting for the 'accept()' call to be processed by the server application.
    // The '5' is the 'backlog' value, specifying the maximum number of pending connections
    // that can be held in the accept queue.
    if (listen(serverSocket, 5) < 0)
    {
        cerr << "Error: Failed to listen on socket." << endl;
        close(serverSocket);
        return 1;
    }
    cout << "Server is now listening on Port 8000 for incoming connections." << endl;

    // 5. Accepting a Connection:
    // The 'accept()' function retrieves the first completed connection from the 'serverSocket's
    // accept queue. It then creates a **new, dedicated socket file descriptor** ('clientSocket')
    // for this specific client connection.
    // This 'clientSocket' is now the endpoint for direct communication with that particular client.
    // The OS's internal entry table for this new 'clientSocket' stores all the details of this
    // established connection, including:
    // - The Local Endpoint: Server's IP address and port (inherited from 'serverSocket').
    // - The Remote Endpoint: Client's IP address and ephemeral port number (extracted from the
    //   client's initial SYN packet during the 3-way handshake).
    // By passing 'nullptr, nullptr' for the address arguments, we instruct the OS to store
    // the client's address internally with 'clientSocket' but not to expose it directly to our program
    // at this point.
    // After 'accept()' returns, the 'serverSocket' remains in the listening state, ready to accept
    // new incoming connections.
    int clientSocket = accept(serverSocket, nullptr, nullptr);

    if (clientSocket < 0)
    {
        cerr << "Error: Failed to accept client connection." << endl;
        close(serverSocket);
        return 1;
    }
    cout << "Client connected! New client socket FD: " << clientSocket << endl;

    // 6. Receiving Data:
    // The 'recv()' function attempts to read incoming data from the 'clientSocket' and copy it
    // into the 'buffer'.
    // The OS uses the internal details of 'clientSocket' (specifically the remote IP and Port)
    // to filter and identify network packets that belong to this specific connection.
    // It then places the data from these packets into the socket's receive buffer,
    // which 'recv()' then retrieves.
    char buffer[1024] = {0};                                                    // Initialize buffer to all zeros to prevent garbage data
    ssize_t bytes_received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); // -1 to leave space for null terminator

    if (bytes_received < 0)
    {
        cerr << "Error: Failed to receive data from client." << endl;
        close(clientSocket);
        close(serverSocket);
        return 1;
    }
    buffer[bytes_received] = '\0'; // Null-terminate the received data to treat it as a C string

    cout << "Request from Client Socket FD: " << clientSocket << " is:\n"
         << buffer << endl;

    // 7. Sending Data:
    // The 'send()' function writes the 'http_response' data to the 'clientSocket'.
    // The OS uses the internal details of 'clientSocket' (both local IP/Port and remote IP/Port)
    // to construct the outgoing network packets.
    // This involves:
    // - Segmenting the data into smaller chunks if necessary.
    // - Adding a TCP header to each segment, which includes source and destination port numbers.
    // - Encapsulating each TCP segment within an IP header, which includes source and destination IP addresses.
    // - Finally, sending these IP packets out through the Network Interface Card (NIC) onto the network.
    string html_content = "<!DOCTYPE html><html><body><h1>Hello From Server</h1></body></html>";
    string http_response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/html\r\n"
                           "Content-Length: " +
                           to_string(html_content.length()) + "\r\n"
                                                              "\r\n" +
                           html_content;

    ssize_t bytes_sent = send(clientSocket, http_response.c_str(), http_response.length(), 0);

    if (bytes_sent < 0)
    {
        cerr << "Error: Failed to send data to client." << endl;
    }
    else
    {
        cout << "Response sent to client (" << bytes_sent << " bytes)." << endl;
    }

    // 8. Closing Sockets:
    // The 'close()' function releases the resources associated with a socket file descriptor.
    // It's crucial to close both the client-specific socket and the listening socket
    // to prevent resource leaks and ensure that the port becomes available for other applications
    // or subsequent runs of this server.
    close(clientSocket); // Close the dedicated client socket
    cout << "Client socket closed." << endl;
    close(serverSocket); // Close the main listening socket
    cout << "Server socket closed. Exiting." << endl;

    return 0; // Indicate successful execution
}
