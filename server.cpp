#include "server.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

Server::Server() : _serverSocket(-1), _port(0) {
    // Le constructeur initialise le descripteur à -1
}

Server::~Server() {
    // On ferme le socket s'il est ouvert pour libérer les ressources
    if (_serverSocket != -1) {
        close(_serverSocket);
        std::cout << "Socket serveur fermée." << std::endl;
    }
}

void Server::start(const char* portStr) {
    // --- Conversion du port ---
    _port = std::atoi(portStr);
    if (_port <= 0) {
        throw std::invalid_argument("Port invalide");
    }

    // --- Création du socket ---
    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket < 0) {
        throw std::runtime_error("Erreur lors de la création du socket serveur");
    }
    std::cout << "Socket serveur créé avec succès: " << _serverSocket << std::endl;

    // --- Configuration de l'adresse du serveur ---
    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;                // IPv4
    serverAddr.sin_port = htons(_port);              // Conversion du port au format réseau
    serverAddr.sin_addr.s_addr = INADDR_ANY;         // Écoute sur toutes les interfaces

    // --- Association (bind) du socket à l'adresse ---
    if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Bind échoué");
    }
    std::cout << "Bind effectué avec succès sur le port: " << _port << std::endl;

    // --- Passage en mode écoute ---
    if (listen(_serverSocket, 100) < 0) {  // 100 est le nombre maximum de connexions en attente
        close(_serverSocket);
        throw std::runtime_error("Listen échoué");
    }
    std::cout << "Le serveur est en écoute..." << std::endl;

    // --- Configuration en mode non bloquant ---
    int flags = fcntl(_serverSocket, F_GETFL, 0);
    if (flags < 0) {
        close(_serverSocket);
        throw std::runtime_error("Erreur lors de fcntl F_GETFL");
    }
    if (fcntl(_serverSocket, F_SETFL, flags | O_NONBLOCK) < 0) {
        close(_serverSocket);
        throw std::runtime_error("Erreur lors de fcntl F_SETFL pour O_NONBLOCK");
    }
    std::cout << "Le socket serveur est désormais en mode non bloquant." << std::endl;
}
