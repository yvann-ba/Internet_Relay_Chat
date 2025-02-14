#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include "../includes/client.hpp"
// #include "server_bonus.hpp"
class Server;

class Bot: public Client
{
    private:
        Server      *_server;
        std::string _botName;

    public:
        Bot(Server* server, const std::string& botName);
        ~Bot();

        void respondToMessage(const std::string& message, int clientFd);
        void sendBotMessage(int clientFd, const std::string& msg);
};

#endif