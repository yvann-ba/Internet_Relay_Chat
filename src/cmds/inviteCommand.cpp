#include "../../includes/server.hpp"
#include <sstream>

void Server::inviteCommand(const std::string &parameters, int client_fd) {
    std::istringstream iss(parameters);
    std::string targetNick, channelName;
    iss >> targetNick >> channelName;

    if (targetNick.empty() || channelName.empty()) {
        sendError(client_fd, 461, "INVITE :Not enough parameters");
        return;
    }
    
    if (_channels.find(channelName) == _channels.end()) {
        sendError(client_fd, 403, channelName + " :No such channel");
        return;
    }
    Channel* channel = _channels[channelName];
    
    if (!channel->isMember(client_fd)) {
        sendError(client_fd, 442, channelName + " :You're not on that channel");
        return;
    }
    
    if (channel->isInviteOnly() && !channel->isOperator(client_fd)) {
        sendError(client_fd, 482, channelName + " :You're not a channel operator");
        return;
    }
    
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
    
    channel->inviteUser(target_fd);
    
    // Send confirmation to the inviter.
    std::string inviteMsg = ":" + _clients[client_fd]->getNickname() + " INVITE " + targetNick + " " + channelName + "\r\n";
    sendServ(_clients[client_fd]->getFDSocket(), inviteMsg);
    
    // Notify the invited user.
    std::string notifyMsg = ":localhost INVITE " + targetNick + " " + channelName + " by " + _clients[client_fd]->getNickname() + "\r\n";
    send(_clients[target_fd]->getFDSocket(), notifyMsg.c_str(), notifyMsg.size(), MSG_NOSIGNAL);
}
