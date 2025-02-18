#include "bot.hpp"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

Bot::Bot(Server* server, const std::string& botName)
	: Client(), _server(server), _botName(botName), _connected(false)
{
	std::cout << "Bot " << _botName << " initialized." << std::endl;
}

Bot::~Bot() {}

void Bot::registerBot()
}


void Bot::sendBotMessage(int clientFd, const std::string& msg) {
	std::string formattedMsg = ":" + _botName + " PRIVMSG " + msg + "\r\n";
	_server->sendServ(clientFd, formattedMsg);
}

void Bot::respondToMessage(const std::string& message, int clientFd) {
	if (message == "!hello") {
		sendBotMessage(clientFd, "Hello! I am " + _botName + ", your friendly bot.");
	} else if (message == "!help") {
		sendBotMessage(clientFd, "Commands: !hello, !joke, !help.");
	} else if (message == "!joke") {
		sendBotMessage(clientFd, "Why don't programmers like nature? It has too many bugs!");
	} else {
		sendBotMessage(clientFd, "I don't understand that command. Try !help.");
	}
}
