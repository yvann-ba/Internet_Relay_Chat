#include "server.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

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
