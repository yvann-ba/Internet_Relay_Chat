#include "../../includes/server.hpp"

void Server::inviteCommand(const std::string &parameters, int client_fd) {
    (void)parameters;

    std::string msg = "INVITE command not implemented\r\n";
    sendServ(_clients[client_fd]->getFDSocket(), msg);
}