#include "../../includes/server.hpp"
// KICK command: stub implementation (not yet implemented)
void Server::kickCommand(const std::string &parameters, int client_fd) {
    (void)parameters;

    std::string msg = "KICK command not implemented\r\n";
    sendServ(_clients[client_fd]->getFDSocket(), msg);
}