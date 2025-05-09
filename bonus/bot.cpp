#include "bot.hpp"
#include "../includes/server.hpp"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <sstream>

Bot::Bot(Server* server, const std::string& botName)
	: Client(), _server(server), _botName(botName), _connected(false)
{
	std::cout << "Bot " << _botName << " initialized." << std::endl;
}

Bot::~Bot() {}


void Bot::sendBotMessage(int clientFd, const std::string &target, const std::string &msg)
{
	std::string formattedMsg = "PRIVMSG " + target + " :" + msg;
	_server->sendServ(clientFd, formattedMsg);
}

void Bot::respondToMessage(const std::string& message, int clientFd) {
	std::istringstream iss(message);
	std::string prefix, commandWord, target;
	
	if (!(iss >> prefix >> commandWord >> target)) {
		std::cerr << "Message mal formaté : " << message << std::endl;
		return;
	}
	
	std::string sender;
	if (prefix[0] == ':') {
		std::string full = prefix.substr(1);
		size_t exclamPos = full.find('!');
		if (exclamPos != std::string::npos)
			sender = full.substr(0, exclamPos);
		else
			sender = full;
	} else {
		sender = prefix;
	}
	
	std::string rest;
	std::getline(iss, rest);
	size_t posCmd = rest.find(" :");
	std::string command;
	if (posCmd != std::string::npos)
		command = rest.substr(posCmd + 2);
	else
		command = rest;
	
	command.erase(std::remove(command.begin(), command.end(), '\r'), command.end());
	command.erase(std::remove(command.begin(), command.end(), '\n'), command.end());
	
	std::cout << "Sender: " << sender << std::endl;
	std::cout << "Commande extraite: " << command << std::endl;
	
	std::string reply;
	if (command == "!hello")
		reply = "Hello! I am " + _botName + ", your friendly bot.";
	else if (command == "!help")
		reply = "Commands: !hello, !joke, !help.";
	else if (command == "!joke")
		reply = "Why don't programmers like nature? It has too many bugs!";
	else
		reply = "I don't understand that command. Try !help.";
	
	sendBotMessage(clientFd, "#BotConversation", reply);
}


void Bot::registerBot(const std::string &password, int clientFd) {
	std::string passCmd = "PASS " + password;
	_server->sendServ(clientFd, passCmd);

	std::string nickCmd = "NICK " + _botName;
	_server->sendServ(clientFd, nickCmd);

	std::string userCmd = "USER " + _botName + " 0 * :" + _botName;
	_server->sendServ(clientFd, userCmd);

	std::string joinCmd = "JOIN #BotConversation";
	_server->sendServ(clientFd, joinCmd);
}