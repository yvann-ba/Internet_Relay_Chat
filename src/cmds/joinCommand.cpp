#include "../../includes/server.hpp"
#include <sstream>
#include <cstdlib>

void Server::joinCommand(const std::string &parameters, int client_fd) {
    std::istringstream iss(parameters);
    std::string channelName;
    std::string providedKey;
    iss >> channelName;
    iss >> providedKey; // Optional key parameter

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

    // Check if client is already a member
    if (channel->isMember(client_fd))
        return;

    // If channel is invite-only, verify the invitation.
    if (channel->isInviteOnly() && !channel->isInvited(client_fd)) {
        sendError(client_fd, 473, channelName + " :Invite-only channel");
        return;
    }

    // If a channel key is set, verify the provided key.
    if (!channel->getChannelKey().empty() && providedKey != channel->getChannelKey()) {
        sendError(client_fd, 475, channelName + " :Bad channel key");
        return;
    }

    // Enforce user limit if set.
    if (channel->getUserLimit() > 0 && channel->getMembers().size() >= (size_t)channel->getUserLimit()) {
        sendError(client_fd, 471, channelName + " :Channel is full");
        return;
    }
    
    channel->addMember(client_fd);

    std::string joinMsg = ":" + _clients[client_fd]->getNickname() + " JOIN " + channelName + "\r\n";
    send(_clients[client_fd]->getFDSocket(), joinMsg.c_str(), joinMsg.size(), MSG_NOSIGNAL);
    channel->broadcastMessage(joinMsg, client_fd);
}
