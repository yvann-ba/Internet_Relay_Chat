#include "../../includes/server.hpp"
#include <sstream>

void Server::kickCommand(const std::string &parameters, int client_fd) {
    std::istringstream iss(parameters);
    std::string channelName, targetNick, comment;
    iss >> channelName >> targetNick;
    std::getline(iss, comment);

    if (channelName.empty() || targetNick.empty()) {
        sendError(client_fd, 461, "KICK :Not enough parameters");
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
    
    if (!channel->isOperator(client_fd)) {
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
    if (target_fd == -1 || !channel->isMember(target_fd)) {
        sendError(client_fd, 401, targetNick + " :No such nick/channel");
        return;
    }
    
    std::ostringstream oss;
    oss << ":" << _clients[client_fd]->getNickname() << " KICK " << channelName << " " << targetNick;
    if (!comment.empty())
        oss << " :" << comment;
    oss << "\r\n";
    std::string kickMsg = oss.str();
    
    
    channel->broadcastMessage(kickMsg, -1);
    
    
    channel->removeMember(target_fd);
}
