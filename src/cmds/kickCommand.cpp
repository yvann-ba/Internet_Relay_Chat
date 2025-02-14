#include "../../includes/server.hpp"

void Server::kickCommand(const std::string &parameters, int client_fd) {
    (void)parameters;

    std::string msg = "KICK command not implemented\r\n";
    sendServ(_clients[client_fd]->getFDSocket(), msg);
}