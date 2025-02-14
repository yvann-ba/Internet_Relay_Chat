#include "bot.hpp"
#include <iostream>
#include "server_bonus.hpp"

Bot::Bot(Server* server, const std::string& botName) 
    : _server(server), _botName(botName) {
    std::cout << "Bot " << _botName << " initialized." << std::endl;
}

Bot::~Bot() {}

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