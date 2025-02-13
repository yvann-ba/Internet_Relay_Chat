#include "../../includes/server.hpp"

void Server::joinCommand(const std::string &parameters, int client_fd) {
    std::string channelName = parameters;
    if (channelName.empty()) {
        sendError(client_fd, 461, "JOIN :Not enough parameters");
        return;
    }
    
    Channel* channel = NULL;
    if (_channels.find(channelName) == _channels.end()) {
        channel = new Channel(channelName);
        _channels[channelName] = channel;
    } else {
        channel = _channels[channelName];
    }
    channel->addMember(client_fd);

    std::string joinMsg = ":" + _clients[client_fd]->getNickname() + " JOIN " + channelName + "\r\n";
    send(_clients[client_fd]->getFDSocket(), joinMsg.c_str(), joinMsg.size(), MSG_NOSIGNAL);
    channel->broadcastMessage(joinMsg, client_fd);
}
