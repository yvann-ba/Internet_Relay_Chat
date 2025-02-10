#include "../includes/server.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

Server::Server() : _serverSocket(-1), _port(0) {
    // Constructor initializes _serverSocket to -1, meaning the socket is not yet created.
}

Server::~Server() {
    // Close the server socket if it is open, freeing resources.
    if (_serverSocket != -1) {
        close(_serverSocket);
        std::cout << "Server socket closed." << std::endl;
    }
}

void Server::start(const char* portStr) {
    // --- Step 1: Convert the port string to an integer ---
    _port = std::atoi(portStr);
    if (_port <= 0) {
        throw std::invalid_argument("Invalid port number");
    }

    // --- Step 2: Create the server socket ---
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0) {
        throw std::runtime_error("Error creating the server socket");
    }
    std::cout << "Server socket created successfully: " << _serverSocket << std::endl;

    // --- Step 3: Configure the server address ---
    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;       // IPv4
    serverAddr.sin_port = htons(_port);     // Convert port to network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY;// Listen on all interfaces

    // --- Step 4: Bind the socket to the address ---
    if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Bind failed");
    }
    std::cout << "Bind successful on port: " << _port << std::endl;

    // --- Step 5: Put the socket in listening mode ---
    if (listen(_serverSocket, 100) < 0) {  // 100 is the backlog (max pending connections)
        close(_serverSocket);
        throw std::runtime_error("Listen failed");
    }
    std::cout << "Server is now listening..." << std::endl;

    // --- Step 6: Set the server socket to non-blocking mode ---
    int flags = fcntl(_serverSocket, F_GETFL, 0);
    if (flags < 0) {
        close(_serverSocket);
        throw std::runtime_error("Error getting socket flags via fcntl F_GETFL");
    }
    if (fcntl(_serverSocket, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Error setting O_NONBLOCK via fcntl F_SETFL");
    }
    std::cout << "Server socket is now set to non-blocking mode." << std::endl;
}

// The run() method now handles client connections in a persistent manner.
void Server::run() {
    while (true) {
        // --- Accept new client connections ---
        int clientSocket = accept(_serverSocket, NULL, NULL);
        if (clientSocket < 0) {
            // If no connection is pending, non-blocking mode will set errno to EWOULDBLOCK or EAGAIN.
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                usleep(100000); // Sleep 100 ms to prevent busy-waiting.
                continue;
            } else {
                std::cerr << "Error accepting connection: " 
                          << std::strerror(errno) << std::endl;
                break;
            }
        }

        std::cout << "New connection accepted, client socket: " 
                  << clientSocket << std::endl;

        // --- Optionally set the client socket to non-blocking mode ---
        int clientFlags = fcntl(clientSocket, F_GETFL, 0);
        if (clientFlags < 0) {
            std::cerr << "Error getting client socket flags: " 
                      << std::strerror(errno) << std::endl;
            close(clientSocket);
            continue;
        }
        if (fcntl(clientSocket, F_SETFL, clientFlags | O_NONBLOCK) < 0) {
            std::cerr << "Error setting client socket non-blocking: " 
                      << std::strerror(errno) << std::endl;
            close(clientSocket);
            continue;
        }

        // --- Process communication with the connected client ---
        bool clientConnected = true;
        while (clientConnected) {
            char buffer[1024];
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived > 0) {
                // Null-terminate and display the received message.
                buffer[bytesReceived] = '\0';
                std::cout << "Message received: " << buffer << std::endl;
                
                // Here, you would normally parse IRC commands (e.g., CAP, NICK, USER, etc.)
                // For simplicity, we send a fixed confirmation response.
                const char* response = "Message received by the server\r\n";
                int bytesSent = send(clientSocket, response, std::strlen(response), 0);
                if (bytesSent < 0) {
                    std::cerr << "Error sending response: " 
                              << std::strerror(errno) << std::endl;
                    clientConnected = false;
                }
            } else if (bytesReceived == 0) {
                // bytesReceived == 0 indicates the client has closed the connection.
                std::cout << "Client closed the connection." << std::endl;
                clientConnected = false;
            } else {
                // An error occurred while reading.
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    // No data available right now; yield and continue the loop.
                    usleep(100000); // Sleep briefly
                    continue;
                } else {
                    std::cerr << "Error receiving data: " 
                              << std::strerror(errno) << std::endl;
                    clientConnected = false;
                }
            }
        }
        // Close the client socket after communication ends.
        close(clientSocket);
    }
}
