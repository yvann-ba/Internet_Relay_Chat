#include "../includes/server.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <cstdio>

Server::Server() : _serverSocket(-1), _port(0), _password("") {}

Server::~Server() {
    if (_serverSocket != -1) {
        close(_serverSocket);
        std::cout << "Server socket closed." << std::endl;
    }
    
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
}

void Server::start(const char* portStr, const char* password) {
    _port = std::atoi(portStr);
    if (_port <= 0) {
        throw std::invalid_argument("Invalid port number");
    }
    _password = password; 

    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0) {
        throw std::runtime_error("Error creating the server socket");
    }
    std::cout << "Server socket created successfully: " << _serverSocket << std::endl;

    
    int opt = 1;
    if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("setsockopt(SO_REUSEADDR) failed");
    }

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Bind failed");
    }
    std::cout << "Bind successful on port: " << _port << std::endl;

    if (listen(_serverSocket, 100) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Listen failed");
    }
    std::cout << "Server is now listening..." << std::endl;

    
    int flags = fcntl(_serverSocket, F_GETFL, 0);
    if (flags < 0) {
        close(_serverSocket);
        throw std::runtime_error("Error getting socket flags");
    }
    if (fcntl(_serverSocket, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Error setting non-blocking mode");
    }
    std::cout << "Server socket is now set to non-blocking mode." << std::endl;
}

void Server::run() {
    
    std::vector<pollfd> pollfds;
    pollfd serverPoll;
    serverPoll.fd = _serverSocket;
    serverPoll.events = POLLIN;
    pollfds.push_back(serverPoll);

    while (true) {
        int ret = poll(&pollfds[0], pollfds.size(), 100); 
        if (ret < 0) {
            perror("poll");
            break;
        }

        
        if (pollfds[0].revents & POLLIN) {
            int client_fd = accept(_serverSocket, NULL, NULL);
            if (client_fd >= 0) {
                
                int flags = fcntl(client_fd, F_GETFL, 0);
                fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

                pollfd newfd;
                newfd.fd = client_fd;
                newfd.events = POLLIN;
                pollfds.push_back(newfd);

                
                Client* newClient = new Client();
                newClient->setFDSocket(client_fd);
                _clients[client_fd] = newClient;

                std::cout << "New connection accepted, client socket: " << client_fd << std::endl;
            }
        }

        
        for (size_t i = 1; i < pollfds.size(); i++) {
            if (pollfds[i].revents & POLLIN) {
                char buffer[1024];
                int n = recv(pollfds[i].fd, buffer, sizeof(buffer) - 1, 0);
                if (n <= 0) {
                    
                    if (n < 0) {
                        std::cerr << "Error on socket " << pollfds[i].fd << ": " << strerror(errno) << std::endl;
                    } else {
                        std::cout << "Client on socket " << pollfds[i].fd << " disconnected." << std::endl;
                    }
                    close(pollfds[i].fd);
                    delete _clients[pollfds[i].fd];
                    _clients.erase(pollfds[i].fd);
                    pollfds.erase(pollfds.begin() + i);
                    i--;
                    continue;
                }
                buffer[n] = '\0';
                std::string msg(buffer);
                std::cout << "Message received from socket " << pollfds[i].fd << ": " << msg << std::endl;

                // Ici, vous devez analyser la commande envoyée par le client.
                // Par exemple, vous pourriez vérifier si le message commence par "PASS", "NICK", "USER", etc.
                // Vous pourriez appeler newClient->processMessage(msg) pour gérer la commande.

                // Pour l'instant, nous effectuons un simple broadcast du message vers tous les autres clients.
                for (size_t j = 1; j < pollfds.size(); j++) {
                    if (j == i) continue;
                    send(pollfds[j].fd, buffer, n, 0);
                }
            }
        }
    }
}
