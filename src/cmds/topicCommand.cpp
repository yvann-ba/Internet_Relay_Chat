#include "../../includes/server.hpp"




void Server::topicCommand(const std::string &parameters, int client_fd) {
    size_t spacePos = parameters.find(" ");
    std::string channelName, topic;
    if (spacePos == std::string::npos) {
        channelName = parameters;
    } else {
        channelName = parameters.substr(0, spacePos);
        topic = parameters.substr(spacePos + 1);
        if (!topic.empty() && topic[0] == ':')
            topic.erase(0, 1);
    }
    
    if (channelName.empty()) {
        sendError(client_fd, 461, "TOPIC :Not enough parameters");
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
    
    
    if (topic.empty()) {
        std::string reply = ":" + _clients[client_fd]->getNickname() + " TOPIC " + channelName + " :" + channel->getTopic() + "\r\n";
        sendServ(_clients[client_fd]->getFDSocket(), reply);
    } else {
        
        if (channel->isTopicRestricted() && !channel->isOperator(client_fd)) {
            sendError(client_fd, 482, channelName + " :You're not a channel operator");
            return;
        }
        
        channel->setTopic(topic);
        std::string topicMsg = ":" + _clients[client_fd]->getNickname() + " TOPIC " + channelName + " :" + topic + "\r\n";
        channel->broadcastMessage(topicMsg, client_fd);
        sendServ(_clients[client_fd]->getFDSocket(), topicMsg);
    }
}
