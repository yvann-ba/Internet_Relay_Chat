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
    // Constructor initializes _serverSocket to -1 indicating it is not created yet.
}

Server::~Server() {
    // Close the socket if it is open to free resources.
    if (_serverSocket != -1) {
        close(_serverSocket);
        std::cout << "Server socket closed." << std::endl;
    }
}

void Server::start(const char* portStr) {
    // --- Step 1: Convert the port ---
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
    serverAddr.sin_family = AF_INET;                // IPv4
    serverAddr.sin_port = htons(_port);              // Convert the port to network byte order
    serverAddr.sin_addr.s_addr = INADDR_ANY;         // Listen on all available interfaces

    // --- Step 4: Bind the socket to the address ---
    if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Bind failed");
    }
    std::cout << "Bind successful on port: " << _port << std::endl;

    // --- Step 5: Put the socket in listening mode ---
    if (listen(_serverSocket, 100) < 0) {  // 100 is the maximum number of pending connections
        close(_serverSocket);
        throw std::runtime_error("Listen failed");
    }
    std::cout << "Server is now listening..." << std::endl;

    // --- Step 6: Set the socket to non-blocking mode ---
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

// Implementation of the Server::run() method, which accepts and handles client connections
void Server::run() {
    // Infinite loop to keep the server running and waiting for client connections
    while (true) {
        // Attempt to accept a new client connection
        int clientSocket = accept(_serverSocket, NULL, NULL);

        // Check if accept() failed
        if (clientSocket < 0) {
            // If no connection is waiting, non-blocking mode causes accept() to return -1 with EWOULDBLOCK or EAGAIN
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No client connection is pending; wait briefly before trying again
                usleep(100000); // Sleep for 100 milliseconds
                continue;
            } else {
                // An actual error occurred; print the error message and break out of the loop
                std::cerr << "Error accepting connection: " 
                          << std::strerror(errno) << std::endl;
                break;
            }
        }

        // A new client connection has been accepted
        std::cout << "New connection accepted, client socket: " 
                  << clientSocket << std::endl;

        // Buffer to store data received from the client
        char buffer[1024];

        // Receive data from the client
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            // Terminate the received data to form a valid C-string
            buffer[bytesReceived] = '\0';
            std::cout << "Message received: " << buffer << std::endl;

            // Prepare a response message for the client
            const char* response = "Message received by the server\n";
            // Send the response to the client
            send(clientSocket, response, std::strlen(response), 0);
        } else if (bytesReceived == 0) {
            // If bytesReceived is 0, the client has closed the connection
            std::cout << "Client closed the connection." << std::endl;
        } else {
            // An error occurred while receiving data; print the error message
            std::cerr << "Error receiving data: " 
                      << std::strerror(errno) << std::endl;
        }

        // Close the client socket to free the associated resources
        close(clientSocket);
    }
}