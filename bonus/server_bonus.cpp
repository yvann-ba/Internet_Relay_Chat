#include "server_bonus.hpp"
#include "../includes/utils.hpp"
#include "bot.hpp"
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
#include <sstream>


Server::Server() : _serverSocket(-1), _port(0), _password(""), _bot(NULL){}

Server::~Server() {
    if (_serverSocket != -1) {
        close(_serverSocket);
        std::cout << "Server socket closed." << std::endl;
    }
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
    delete _bot;
}

void Server::start(const char* portStr, const char* password) {
    _port = std::atoi(portStr);
    if (_port < 1024 || _port > 65535) {
        throw std::invalid_argument("Port number must be between 1024 and 65535");
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
    _bot = new Bot(this, "bot");
    if (_bot == NULL) {
        close(_serverSocket);
        throw std::runtime_error("Error creating bot");
    }
}

bool Server::checkIsRegistered(int client_fd) {
    if (_clients.find(client_fd) == _clients.end() || _clients[client_fd] == NULL) {
        std::cerr << "Error: Client with fd " << client_fd << " does not exist or is uninitialized!" << std::endl;
        return false;
    }
    if (_clients[client_fd]->getNickname() == "" || _clients[client_fd]->getUsername() == "" || _clients[client_fd]->getRegistered() == false) {
        sendError(_clients[client_fd]->getFDSocket(), 451, "");
        return false;
    }
    return true;
}

void Server::sendServ(int fd, const std::string &msg) {
    if (fd < 0) {
        std::cerr << "Error: Invalid file descriptor\n";
        return;
    }
    std::string formattedMsg = msg + "\r\n";
    ssize_t bytesSent = send(fd, formattedMsg.c_str(), formattedMsg.length(), MSG_NOSIGNAL | MSG_DONTWAIT);
    if (bytesSent == -1)
        std::cerr << "Error: Failed to send message\n";
}

void Server::processClientCommand(std::string* clientBuffer, int client_fd) {
    std::string tempBuffer;
    std::string currentLine;
    std::string command;
    std::string parameters;
    size_t spacePosition;

    tempBuffer = normalizeSpaces(*clientBuffer);
    if (tempBuffer.find("\n") == std::string::npos) {
        
        return;
    }
    *clientBuffer = "";

    while (!tempBuffer.empty()) {
        size_t newlinePos = tempBuffer.find("\n");
        if (newlinePos == std::string::npos)
            currentLine = tempBuffer;
        else
            currentLine = tempBuffer.substr(0, newlinePos);

        size_t pos = currentLine.find('\r');
        if (pos != std::string::npos)
            currentLine.erase(pos, 1);

        spacePosition = currentLine.find(" ");
        if (spacePosition == std::string::npos) {
            command = currentLine;
            parameters = "";
        } else {
            command = currentLine.substr(0, spacePosition);
            parameters = currentLine.substr(spacePosition + 1);
        }

        if (command == "PASS")
            passCommand(parameters, client_fd);
        else if (command == "NICK")
            nickCommand(parameters, client_fd);
        else if (command == "USER")
            userCommand(parameters, client_fd);
        else if (command == "QUIT")
            quitCommand(parameters, client_fd);
        else if (checkIsRegistered(client_fd)) {
            if (command == "PRIVMSG")
                privmsgCommand(parameters, client_fd);
            else if (command == "BOT") {
                _bot->respondToMessage(parameters, client_fd);
            }
            else if (command == "JOIN")
                joinCommand(parameters, client_fd);
            else if (command == "INVITE")
                inviteCommand(parameters, client_fd);
            else if (command == "TOPIC")
                topicCommand(parameters, client_fd);
            else if (command == "KICK")
                kickCommand(parameters, client_fd);
            else if (command == "MODE")
                modeCommand(parameters, client_fd);
            else {
                std::ostringstream portStream;
                portStream << _port;
                std::string errorMsg = ":localhost:" + portStream.str() + " 421 " + _clients[client_fd]->getNickname() + " :Unknown command\r\n";
                sendServ(_clients[client_fd]->getFDSocket(), errorMsg);
            }
        }

        if (newlinePos == std::string::npos)
            break;
        tempBuffer = tempBuffer.substr(newlinePos + 1);
    }
}

void Server::run() {
    std::vector<pollfd> pollfds;
    pollfd serverPoll;
    memset(&serverPoll, 0, sizeof(serverPoll));
    serverPoll.fd = _serverSocket;
    serverPoll.events = POLLIN;
    pollfds.push_back(serverPoll);

    std::map<int, std::string> clientBuffers;

    while (g_running) {
        int ret = poll(pollfds.data(), pollfds.size(), 100);
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
                memset(&newfd, 0, sizeof(newfd));
                newfd.fd = client_fd;
                newfd.events = POLLIN;
                pollfds.push_back(newfd);

                Client* newClient = new Client();
                newClient->setFDSocket(client_fd);
                _clients[client_fd] = newClient;
                clientBuffers[client_fd] = "";

                std::cout << "New connection accepted, client socket: " << client_fd << std::endl;
            }
        }

        for (size_t j = 1; j < pollfds.size(); j++) {
            if (pollfds[j].revents & POLLIN) {
                char buffer[1024];
                int n = recv(pollfds[j].fd, buffer, sizeof(buffer) - 1, 0);
                if (n < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                        continue;
                    else {
                        std::cerr << "Error on socket " << pollfds[j].fd << ": " << strerror(errno) << std::endl;
                        close(pollfds[j].fd);
                        delete _clients[pollfds[j].fd];
                        _clients.erase(pollfds[j].fd);
                        clientBuffers.erase(pollfds[j].fd);
                        pollfds.erase(pollfds.begin() + j);
                        j--;
                        continue;
                    }
                } else if (n == 0) {
                    std::cout << "Client on socket " << pollfds[j].fd << " disconnected." << std::endl;
                    close(pollfds[j].fd);
                    delete _clients[pollfds[j].fd];
                    _clients.erase(pollfds[j].fd);
                    clientBuffers.erase(pollfds[j].fd);
                    pollfds.erase(pollfds.begin() + j);
                    j--;
                    continue;
                }
                buffer[n] = '\0';
                clientBuffers[pollfds[j].fd] += buffer;
                if (clientBuffers[pollfds[j].fd].find("\n") != std::string::npos) {
                    processClientCommand(&clientBuffers[pollfds[j].fd], pollfds[j].fd);
                }
                std::cout << "Message received from socket " << pollfds[j].fd << ": " << buffer << std::endl;
            }
        }
    }
}
