#include "../../includes/server.hpp"
// INVITE command: stub implementation (not yet implemented)
void Server::inviteCommand(const std::string &parameters, int client_fd) {
    (void)parameters;

    std::string msg = "INVITE command not implemented\r\n";
    sendServ(_clients[client_fd]->getFDSocket(), msg);
}