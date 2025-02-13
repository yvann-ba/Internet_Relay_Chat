#include "../../includes/server.hpp"

// QUIT command: disconnects the client and notifies all channels
void Server::quitCommand(const std::string &parameters, int client_fd) {
    std::string quitMsg;
    if (!parameters.empty()) {
        quitMsg = ":" + _clients[client_fd]->getNickname() + " QUIT :" + parameters + "\r\n";
    } else {
        quitMsg = ":" + _clients[client_fd]->getNickname() + " QUIT\r\n";
    }
    
    // Notify all channels and remove the client from them
    for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        Channel* channel = it->second;
        if (channel->isMember(client_fd)) {
            channel->broadcastMessage(quitMsg, client_fd);
            channel->removeMember(client_fd);
        }
    }
    
    // Close client socket and remove client from the map
    close(_clients[client_fd]->getFDSocket());
    delete _clients[client_fd];
    _clients.erase(client_fd);
}