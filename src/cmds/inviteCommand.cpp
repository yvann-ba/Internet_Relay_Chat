#include "../../includes/server.hpp"
#include <sstream>

void Server::inviteCommand(const std::string &parameters, int client_fd) {
    std::istringstream iss(parameters);
    std::string targetNick;
    std::string channelName;
    iss >> targetNick >> channelName;

    if (targetNick.empty() || channelName.empty()) {
        sendError(client_fd, 461, "INVITE :Not enough parameters");
        return;
    }
    // Verify that the channel exists.
    if (_channels.find(channelName) == _channels.end()) {
        sendError(client_fd, 403, channelName + " :No such channel");
        return;
    }
    Channel* channel = _channels[channelName];
    // Check that the inviter is a member of the channel.
    if (!channel->isMember(client_fd)) {
        sendError(client_fd, 442, channelName + " :You're not on that channel");
        return;
    }
    // If channel is invite-only, only an operator may invite.
    if (channel->isInviteOnly() && !channel->isOperator(client_fd)) {
        sendError(client_fd, 482, channelName + " :You're not a channel operator");
        return;
    }
    // Find the target client by nickname.
    int target_fd = -1;
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getNickname() == targetNick) {
            target_fd = it->first;
            break;
        }
    }
    if (target_fd == -1) {
        sendError(client_fd, 401, targetNick + " :No such nick/channel");
        return;
    }
    // Add the target to the invitation list.
    channel->inviteUser(target_fd);

    // Send confirmation to the inviter.
    std::string inviteMsg = ":" + _clients[client_fd]->getNickname() + " INVITE " + targetNick + " " + channelName + "\r\n";
    sendServ(_clients[client_fd]->getFDSocket(), inviteMsg);

    // Notify the target of the invitation.
    std::string notifyMsg = ":server INVITE " + targetNick + " " + channelName + " by " + _clients[client_fd]->getNickname() + "\r\n";
    send(_clients[target_fd]->getFDSocket(), notifyMsg.c_str(), notifyMsg.size(), MSG_NOSIGNAL);
}
