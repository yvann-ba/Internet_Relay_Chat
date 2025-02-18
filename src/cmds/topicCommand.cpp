#include "../../includes/server.hpp"

// Process the TOPIC command: view or change the channel topic.
// If a new topic is provided and the channel has topic restrictions enabled,
// only channel operators are allowed to modify the topic.
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
    
    // If no new topic is provided, send the current topic to the client.
    if (topic.empty()) {
        std::string reply = ":" + _clients[client_fd]->getNickname() + " TOPIC " + channelName + " :" + channel->getTopic() + "\r\n";
        sendServ(_clients[client_fd]->getFDSocket(), reply);
    } else {
        // If the channel is topic restricted, only operators can change the topic.
        if (channel->isTopicRestricted() && !channel->isOperator(client_fd)) {
            sendError(client_fd, 482, channelName + " :You're not a channel operator");
            return;
        }
        // Update the topic and broadcast the change.
        channel->setTopic(topic);
        std::string topicMsg = ":" + _clients[client_fd]->getNickname() + " TOPIC " + channelName + " :" + topic + "\r\n";
        channel->broadcastMessage(topicMsg, client_fd);
        sendServ(_clients[client_fd]->getFDSocket(), topicMsg);
    }
}
