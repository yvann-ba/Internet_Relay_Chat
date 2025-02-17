#include "../../includes/server.hpp"
#include <sstream>

// Improved kickCommand that sends the kick message only once to all members
// and then removes the kicked user from the channel.
void Server::kickCommand(const std::string &parameters, int client_fd) {
    std::istringstream iss(parameters);
    std::string channelName, targetNick, comment;
    iss >> channelName >> targetNick;
    std::getline(iss, comment);

    // Check that channel name and target nickname are provided.
    if (channelName.empty() || targetNick.empty()) {
        sendError(client_fd, 461, "KICK :Not enough parameters");
        return;
    }
    
    // Verify the channel exists.
    if (_channels.find(channelName) == _channels.end()) {
        sendError(client_fd, 403, channelName + " :No such channel");
        return;
    }
    Channel* channel = _channels[channelName];
    
    // Check that the sender is a member of the channel.
    if (!channel->isMember(client_fd)) {
        sendError(client_fd, 442, channelName + " :You're not on that channel");
        return;
    }
    
    // Check that the sender is an operator.
    if (!channel->isOperator(client_fd)) {
        sendError(client_fd, 482, channelName + " :You're not a channel operator");
        return;
    }
    
    // Find the target client.
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
    
    // Build the kick message in the proper IRC format.
    // Format: :<operatorNick> KICK <channel> <targetNick> :<comment>
    std::ostringstream oss;
    oss << ":" << _clients[client_fd]->getNickname() << " KICK " << channelName << " " << targetNick;
    if (!comment.empty())
        oss << " :" << comment;
    oss << "\r\n";
    std::string kickMsg = oss.str();
    
    // Broadcast the kick message to all members (including the target)
    // by using -1 for sender_fd.
    channel->broadcastMessage(kickMsg, -1);
    
    // Remove the target from the channel.
    channel->removeMember(target_fd);
}
