#include "../../includes/server.hpp"
#include <sstream>
#include <cstdlib>
#include <vector>

// Process the MODE command and send responses as NOTICE messages for HexChat visual display
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
        // Query current modes: send current channel modes as a NOTICE message
        std::string modeString = "+";
        if (channel->isInviteOnly()) modeString += "i";
        if (channel->isTopicRestricted()) modeString += "t";
        if (!channel->getChannelKey().empty()) modeString += "k";
        if (channel->getUserLimit() > 0) modeString += "l";
        std::ostringstream reply;
        reply << ":ircserv NOTICE " << _clients[client_fd]->getNickname() << " :MODE " << channelName << " " << modeString << "\r\n";
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
    
    std::ostringstream overallModes;
    overallModes << sign;
    
    for (size_t i = 1; i < modes.size(); i++) {
        char mode = modes[i];
        if (sign == '+') {
            switch(mode) {
                case 'i':
                    channel->setInviteOnly(true);
                    overallModes << "i";
                    break;
                case 't':
                    channel->setTopicRestricted(true);
                    overallModes << "t";
                    break;
                case 'k': {
                    std::string key;
                    iss >> key;
                    if (key.empty()) {
                        sendError(client_fd, 461, "MODE :Not enough parameters for key");
                        continue;
                    }
                    channel->setChannelKey(key);
                    overallModes << "k " << key;
                    break;
                }
                case 'l': {
                    std::string limitStr;
                    iss >> limitStr;
                    int limit = atoi(limitStr.c_str());
                    if (limitStr.empty() || limit <= 0) {
                        sendError(client_fd, 461, "MODE :Invalid user limit");
                        continue;
                    }
                    channel->setUserLimit(limit);
                    overallModes << "l " << limit;
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
                    // Notify the target about operator status as a NOTICE message
                    std::ostringstream opMsg;
                    opMsg << ":ircserv NOTICE " << targetNick << " :You have been given operator status in " << channelName 
                          << " by " << _clients[client_fd]->getNickname() << "\r\n";
                    sendServ(_clients[target_fd]->getFDSocket(), opMsg.str());
                    overallModes << "o";
                    break;
                }
                default:
                    sendError(client_fd, 472, std::string(1, mode) + " :is unknown mode char to me");
                    break;
            }
        } else { // sign == '-'
            switch(mode) {
                case 'i':
                    channel->setInviteOnly(false);
                    overallModes << "i";
                    break;
                case 't':
                    channel->setTopicRestricted(false);
                    overallModes << "t";
                    break;
                case 'k':
                    channel->removeChannelKey();
                    overallModes << "k";
                    break;
                case 'l':
                    channel->removeUserLimit();
                    overallModes << "l";
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
                    // Notify the target about removal of operator status
                    std::ostringstream opMsg;
                    opMsg << ":ircserv NOTICE " << targetNick << " :Your operator status has been removed in " 
                          << channelName << " by " << _clients[client_fd]->getNickname() << "\r\n";
                    sendServ(_clients[target_fd]->getFDSocket(), opMsg.str());
                    overallModes << "o";
                    break;
                }
                default:
                    sendError(client_fd, 472, std::string(1, mode) + " :is unknown mode char to me");
                    break;
            }
        }
    }
    std::string overallModeStr = overallModes.str();
    if (overallModeStr.length() > 1) {
        // Broadcast the overall mode change as a NOTICE message for visual display
        std::ostringstream modeMsg;
        modeMsg << ":ircserv NOTICE " << _clients[client_fd]->getNickname() << " :MODE " << channelName 
                << " " << overallModeStr << "\r\n";
        channel->broadcastMessage(modeMsg.str(), client_fd);
        sendServ(_clients[client_fd]->getFDSocket(), modeMsg.str());
    }
}
