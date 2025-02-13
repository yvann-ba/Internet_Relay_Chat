#include "../includes/server.hpp"
#include "../includes/utils.hpp"
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

Server::Server() : _serverSocket(-1), _port(0), _password("") {}

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
}

bool Server::checkIsRegistered(int i)
{
	if (_clients.find(i) == _clients.end() || _clients[i] == NULL) {
		std::cerr << "Error: Client at index " << i << " does not exist or is uninitialized!" << std::endl;
		return false;
	}
	if (_clients[i]->getNickname() == "" || _clients[i]->getUsername() == "" || _clients[i]->getRegistered() == false)
	{
		sendError(_clients[i]->getFDSocket(), 451, "");
		return false;
	}
	return true;
}

void Server::sendServ(int fd, const std::string &msg)
{
    if (fd < 0) {
        std::cerr << "Error: Invalid file descriptor\n";
        return;
    }

    std::string formattedMsg = msg + "\r\n";
    ssize_t bytesSent = send(fd, formattedMsg.c_str(), formattedMsg.length(),  MSG_NOSIGNAL | MSG_DONTWAIT);

    if (bytesSent == -1)
        std::cerr << "Error: Failed to send message\n";
}


void Server::sendError(int client_fd, int error_code, const std::string &param)
{
    std::map<int, std::string> error_messages;

    error_messages[401] = ":server_name 401 " + param + " :No such nick/channel\r\n";
    error_messages[402] = ":server_name 402 " + param + " :No such server\r\n";
    error_messages[403] = ":server_name 403 " + param + " :No such channel\r\n";
    error_messages[404] = ":server_name 404 " + param + " :Cannot send to channel\r\n";
    error_messages[405] = ":server_name 405 " + param + " :You have joined too many channels\r\n";
    error_messages[411] = ":server_name 411 * :No recipient given\r\n";
    error_messages[412] = ":server_name 412 * :No text to send\r\n";
    error_messages[421] = ":server_name 421 " + param + " :Unknown command\r\n";
    error_messages[431] = ":server_name 431 * :No nickname given\r\n";
    error_messages[432] = ":server_name 432 " + param + " :Erroneous nickname\r\n";
    error_messages[433] = ":server_name 433 " + param + " :Nickname is already in use\r\n";
    error_messages[451] = ":server_name 451 * :You are not registered\r\n";
    error_messages[461] = ":server_name 461 " + param + " :Not enough parameters\r\n";
    error_messages[462] = ":server_name 462 * :You may not reregister\r\n";
    error_messages[464] = ":server_name 464 * :Password incorrect\r\n";
    error_messages[471] = ":server_name 471 " + param + " :Channel is full\r\n";
    error_messages[473] = ":server_name 473 " + param + " :Invite-only channel\r\n";
    error_messages[475] = ":server_name 475 " + param + " :Bad channel key (incorrect password)\r\n";
    error_messages[482] = ":server_name 482 " + param + " :You're not a channel operator\r\n";

    if (error_messages.find(error_code) != error_messages.end())
        send(client_fd, error_messages[error_code].c_str(), error_messages[error_code].length(), 0);
}

void	Server::processClientCommand(std::string* clientBuffer, int clientIndex)
{
	std::string tempBuffer;
	std::string currentLine;
	std::string command;
	std::string parameters;
	int			spacePosition;

	tempBuffer = normalizeSpaces(*clientBuffer);
	if (tempBuffer.find("\n") == std::string::npos)
	{
		std::cout << "finish" << std::endl;
		return;
	}
	*clientBuffer = "";

	while (!tempBuffer.empty())
	{
		if (tempBuffer.find("\n") == std::string::npos)
			currentLine = tempBuffer;
		else
			currentLine = tempBuffer.substr(0, tempBuffer.find("\n"));

		if (currentLine.find('\r') != std::string::npos)
			currentLine.erase(currentLine.find('\r'), 1);

		spacePosition = currentLine.find(" ");
		if (spacePosition == -1) {
			command = currentLine;
			parameters = "";
		} else {
			command = currentLine.substr(0, spacePosition);
			parameters = currentLine.substr(spacePosition + 1);
		}

		if (command == "PASS")
			passCommand(parameters, clientIndex);
		else if (command == "NICK")
			nickCommand(parameters, clientIndex);
		else if (command == "USER")
			userCommand(parameters, clientIndex);
			else if (command == "QUIT")
				std::cout << "quitCommand(clientIndex);";
        else if (command == "INFO") {
            std::cout << _clients[clientIndex]->getFDSocket() << std::endl;
            std::cout << _clients[clientIndex]->getNickname() << std::endl;
            std::cout << _clients[clientIndex]->getUsername() << std::endl;
            std::cout << _clients[clientIndex]->getRealName() << std::endl;
        }
		else if (checkIsRegistered(clientIndex - 1))
		{
			if (command == "PRIVMSG")
				std::cout << "prvMessageCommand(parameters, clientIndex);";
			else if (command == "JOIN")
				std::cout << "joinCommand(parameters, clientIndex);";
			else if (command == "INVITE")
				std::cout << "inviteCommand(parameters, clientIndex);";
			else if (command == "TOPIC")
				std::cout << "topicCommand(parameters, clientIndex);";
			else if (command == "KICK")
				std::cout << "kickCommand(parameters, clientIndex);";
			else if (command == "PART")
				std::cout << "partCommand(parameters, clientIndex);";
			else if (command == "MODE")
				std::cout << "modeCommand(parameters, clientIndex);";
			else
			{
				std::ostringstream portStream;
				portStream << _port;
				std::string errorMsg = ":localhost:" + portStream.str() + " 421 " + _clients[clientIndex]->getNickname() + " :Unknown command";
				std::cout << "servSend(_fds[clientIndex].fd, errorMsg);";
			}
		}

		if (tempBuffer.find('\n') == std::string::npos)
			break;
		tempBuffer = tempBuffer.substr(tempBuffer.find('\n') + 1);
	}
}

void Server::joinCommand(const std::string &parameters, int client_fd) {
    std::string channelName = parameters;
    if (channelName.empty()) {
        sendError(client_fd, 461, "JOIN :Not enough parameters");
        return;
    }
    
    Channel* channel = NULL;
    if (_channels.find(channelName) == _channels.end()) {
        channel = new Channel(channelName);
        _channels[channelName] = channel;
    } else {
        channel = _channels[channelName];
    }
    channel->addMember(client_fd);

    
    std::string joinMsg = ":" + _clients[client_fd]->getNickname() + " JOIN " + channelName + "\r\n";
    send(_clients[client_fd]->getFDSocket(), joinMsg.c_str(), joinMsg.size(), MSG_NOSIGNAL);
    channel->broadcastMessage(joinMsg, client_fd);
}

void Server::privmsgCommand(const std::string &parameters, int client_fd) {
    size_t spacePos = parameters.find(" ");
    if (spacePos == std::string::npos) {
        sendError(client_fd, 461, "PRIVMSG :Not enough parameters");
        return;
    }
    std::string target = parameters.substr(0, spacePos);
    std::string message = parameters.substr(spacePos + 1);
    if (!message.empty() && message[0] == ':')
        message.erase(0, 1);
    std::string fullMsg = ":" + _clients[client_fd]->getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";

    if (target[0] == '#') {
        
        if (_channels.find(target) != _channels.end()) {
            Channel* channel = _channels[target];
            if (!channel->isMember(client_fd)) {
                sendError(client_fd, 442, target + " :You're not on that channel");
                return;
            }
            channel->broadcastMessage(fullMsg, client_fd);
        } else {
            sendError(client_fd, 403, target + " :No such channel");
        }
    } else {
        
        bool found = false;
        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
            if (it->second->getNickname() == target) {
                send(it->second->getFDSocket(), fullMsg.c_str(), fullMsg.size(), MSG_NOSIGNAL);
                found = true;
                break;
            }
        }
        if (!found) {
            sendError(client_fd, 401, target + " :No such nick/channel");
        }
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

    while (true) {
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
