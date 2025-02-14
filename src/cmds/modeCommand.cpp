#include "../../includes/server.hpp"
#include <sstream>
#include <cstdlib>

void Server::modeCommand(const std::string &parameters, int client_fd) {
    std::istringstream iss(parameters);
    std::string channelName;
    iss >> channelName;
    if (channelName.empty()) {
        sendError(client_fd, 461, "MODE :Not enough parameters");
        return;
    }
    if (_channels.find(channelName) == _channels.end()) {
        sendError(client_fd, 403, channelName + " :No such channel");
        return;
    }
    Channel* channel = _channels[channelName];
    
    std::string modes;
    iss >> modes;
    if (modes.empty()) {
        std::string modeString = "+";
        if (channel->isInviteOnly()) modeString += "i";
        if (channel->isTopicRestricted()) modeString += "t";
        if (!channel->getChannelKey().empty()) modeString += "k";
        if (channel->getUserLimit() > 0) modeString += "l";
        std::ostringstream reply;
        reply << ":" << _clients[client_fd]->getNickname() << " MODE " << channelName << " " << modeString;
        if (channel->getUserLimit() > 0)
            reply << " " << channel->getUserLimit();
        if (!channel->getChannelKey().empty())
            reply << " " << channel->getChannelKey();
        reply << "\r\n";
        sendServ(_clients[client_fd]->getFDSocket(), reply.str());
        return;
    }
    
    if (!channel->isOperator(client_fd)) {
        sendError(client_fd, 482, channelName + " :You're not a channel operator");
        return;
    }
    char sign = modes[0];
    if (sign != '+' && sign != '-') {
        sendError(client_fd, 472, "Invalid mode flag");
        return;
    }
    for (size_t i = 1; i < modes.size(); i++) {
        char mode = modes[i];
        if (sign == '+') {
            switch(mode) {
                case 'i':
                    channel->setInviteOnly(true);
                    break;
                case 't':
                    channel->setTopicRestricted(true);
                    break;
                case 'k': {
                    std::string key;
                    iss >> key;
                    if (key.empty()) {
                        sendError(client_fd, 461, "MODE :Not enough parameters for key");
                        continue;
                    }
                    channel->setChannelKey(key);
                    break;
                }
                case 'l': {
                    std::string limitStr;
                    iss >> limitStr;
                    if (limitStr.empty() || atoi(limitStr.c_str()) <= 0) {
                        sendError(client_fd, 461, "MODE :Invalid user limit");
                        continue;
                    }
                    channel->setUserLimit(atoi(limitStr.c_str()));
                    break;
                }
                case 'o': {
                    std::string targetNick;
                    iss >> targetNick;
                    if (targetNick.empty()) {
                        sendError(client_fd, 461, "MODE :Not enough parameters for operator");
                        continue;
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
                        continue;
                    }
                    channel->addOperator(target_fd);
                    break;
                }
                default:
                    sendError(client_fd, 472, std::string(1, mode) + " :is unknown mode char to me");
                    break;
            }
        } else { 
            switch(mode) {
                case 'i':
                    channel->setInviteOnly(false);
                    break;
                case 't':
                    channel->setTopicRestricted(false);
                    break;
                case 'k':
                    channel->removeChannelKey();
                    break;
                case 'l':
                    channel->removeUserLimit();
                    break;
                case 'o': {
                    std::string targetNick;
                    iss >> targetNick;
                    if (targetNick.empty()) {
                        sendError(client_fd, 461, "MODE :Not enough parameters for operator removal");
                        continue;
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
                        continue;
                    }
                    channel->removeOperator(target_fd);
                    break;
                }
                default:
                    sendError(client_fd, 472, std::string(1, mode) + " :is unknown mode char to me");
                    break;
            }
        }
    }
    std::ostringstream modeMsg;
    modeMsg << ":" << _clients[client_fd]->getNickname() << " MODE " << channelName << " " << modes << "\r\n";
    channel->broadcastMessage(modeMsg.str(), client_fd);
    sendServ(_clients[client_fd]->getFDSocket(), modeMsg.str());
}
