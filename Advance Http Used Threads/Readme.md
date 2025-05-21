# Simple Multi-threaded HTTP Server

This is a basic implementation of a multi-threaded HTTP server in C that handles client connections concurrently using pthreads.

## Overview

This server:
- Creates a TCP socket listening on port 8000
- Accepts incoming client connections
- Spawns a new thread for each client request
- Sends a simple HTTP response
- Handles connection cleanup

## Key Features

- **Multi-threaded Design**: Uses pthread library to handle multiple client connections simultaneously
- **TCP/IP Socket Programming**: Implements low-level network communication
- **Dynamic Thread Management**: Creates and detaches threads to handle requests independently
- **Resource Management**: Properly frees resources after use

## Technical Details

### HTTP Connection Behavior

This server implementation demonstrates several important characteristics of HTTP connections:

1. **Unique Socket per Request**
   - Each client request receives a unique socket descriptor
   - Even multiple requests from the same user/browser get different socket descriptors
   - Example: user1 (req1) → socket 1, user2 (req1) → socket 2, user1 (req2) → socket 3

2. **Dynamic Source Ports**
   - When the same client makes multiple requests, the source IP remains the same but the source port changes
   - Example: user1 (request for index.html) → 127.0.0.1:32232, user1 (request for favicon.ico) → 127.0.0.1:32235
   - This allows the client's OS to track different connections

3. **Browser Connection Behavior**
   - Browsers often send multiple TCP requests to servers for performance optimization
   - Some connections may be "dummy" requests with no HTTP payload
   - This is part of browsers' connection management strategies

### Code Structure

- **Socket Creation**: Creates a server socket with IPv4 and TCP
- **Binding**: Binds the socket to port 8000 on all interfaces (0.0.0.0)
- **Listening**: Listens for incoming connections with a backlog queue of 5
- **Client Handling**: For each client connection:
  - Accepts the connection
  - Receives the request data
  - Creates a new thread to handle the response
  - Detaches the thread so it can clean up independently
  - Goes back to listening for new connections

## HTTP Protocol Notes

- The server sends a basic HTTP response with proper headers
- Headers and body are separated by a double CRLF (`\r\n\r\n`)
- Content-Type is set to text/plain
- The server does not implement keepalive connections

## Build and Run

### Prerequisites
- GCC or compatible C compiler
- pthread library
- POSIX-compliant system (Linux, macOS, etc.)

### Compilation
```bash
gcc server.c -o server -lpthread
```

### Running
```bash
./server
```
The server will start listening on port 8000.

### Testing
Open a web browser and navigate to:
```
http://localhost:8000
```
You should see the message "This is Server" displayed.

## Limitations

- Only supports basic HTTP responses
- No request parsing or routing
- Does not handle complex HTTP features like cookies, sessions, etc.
- No error handling for failed socket operations
- No graceful shutdown mechanism

## Future Improvements

- Add proper HTTP request parsing
- Implement response routing based on URL paths
- Add support for serving static files
- Implement HTTP keepalive connections
- Add proper error handling and logging
- Create a graceful shutdown mechanism
