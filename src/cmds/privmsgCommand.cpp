#include "../../includes/server.hpp"

void Server::privmsgCommand(const std::string &parameters, int client_fd) {
    size_t spacePos = parameters.find(" ");
    if (spacePos == std::string::npos) {
        sendError(client_fd, 461, "PRIVMSG :Not enough parameters");
        return;
    }
    std::string target = parameters.substr(0, spacePos);
    std::string message = parameters.substr(spacePos + 1);
    if (!message.empty() && message[0] == ':')
        message.erase(0, 1);
    std::string fullMsg = ":" + _clients[client_fd]->getNickname() + " PRIVMSG " + target + " :" + message + "\r\n";

    if (target[0] == '#') {
        if (_channels.find(target) != _channels.end()) {
            Channel* channel = _channels[target];
            if (!channel->isMember(client_fd)) {
                sendError(client_fd, 442, target + " :You're not on that channel");
                return;
            }
            channel->broadcastMessage(fullMsg, client_fd);
        } else {
            sendError(client_fd, 403, target + " :No such channel");
        }
    } else {
        bool found = false;
        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
            if (it->second->getNickname() == target) {
                send(it->second->getFDSocket(), fullMsg.c_str(), fullMsg.size(), MSG_NOSIGNAL);
                found = true;
                break;
            }
        }
        if (!found) {
            sendError(client_fd, 401, target + " :No such nick/channel");
        }
    }
}
