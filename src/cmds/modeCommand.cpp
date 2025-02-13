#include "../../includes/server.hpp"

// MODE command: stub implementation (not yet implemented)
void Server::modeCommand(const std::string &parameters, int client_fd) {
    (void)parameters;
    std::string msg = "MODE command not implemented\r\n";
    sendServ(_clients[client_fd]->getFDSocket(), msg);
}