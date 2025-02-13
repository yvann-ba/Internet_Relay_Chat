#include "../includes/channel.hpp"
#include <algorithm>
#include <sys/socket.h> 


Channel::Channel(const std::string &name) : _name(name), _topic("") {}


Channel::~Channel() {}


const std::string &Channel::getName() const {
    return _name;
}


const std::string &Channel::getTopic() const {
    return _topic;
}


const std::vector<int> &Channel::getMembers() const {
    return _members;
}


void Channel::setTopic(const std::string &topic) {
    _topic = topic;
}


void Channel::addMember(int client_fd) {
    if (!isMember(client_fd))
        _members.push_back(client_fd);
}


void Channel::removeMember(int client_fd) {
    _members.erase(std::remove(_members.begin(), _members.end(), client_fd), _members.end());
}


bool Channel::isMember(int client_fd) const {
    return std::find(_members.begin(), _members.end(), client_fd) != _members.end();
}


void Channel::broadcastMessage(const std::string &msg, int sender_fd) const {
    for (size_t i = 0; i < _members.size(); i++) {
        if (_members[i] != sender_fd) {
            send(_members[i], msg.c_str(), msg.size(), MSG_NOSIGNAL);
        }
    }
}
