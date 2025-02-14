#include "../includes/channel.hpp"
#include <sys/socket.h>

Channel::Channel(const std::string &name)
    : _name(name), _topic(""), _inviteOnly(false), _topicRestricted(false),
      _channelKey(""), _userLimit(0) {}

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
    if (!isMember(client_fd)) {
        _members.push_back(client_fd);
        // If first member, automatically grant operator privileges.
        if (_members.size() == 1)
            _operators.push_back(client_fd);
        // Remove invitation if present.
        removeInvite(client_fd);
    }
}

void Channel::removeMember(int client_fd) {
    _members.erase(std::remove(_members.begin(), _members.end(), client_fd), _members.end());
    // Also remove from operators.
    removeOperator(client_fd);
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

// Invite-only mode
bool Channel::isInviteOnly() const {
    return _inviteOnly;
}

void Channel::setInviteOnly(bool flag) {
    _inviteOnly = flag;
}

// Topic restricted mode
bool Channel::isTopicRestricted() const {
    return _topicRestricted;
}

void Channel::setTopicRestricted(bool flag) {
    _topicRestricted = flag;
}

// Channel key management
const std::string &Channel::getChannelKey() const {
    return _channelKey;
}

void Channel::setChannelKey(const std::string &key) {
    _channelKey = key;
}

void Channel::removeChannelKey() {
    _channelKey = "";
}

// User limit management
int Channel::getUserLimit() const {
    return _userLimit;
}

void Channel::setUserLimit(int limit) {
    _userLimit = limit;
}

void Channel::removeUserLimit() {
    _userLimit = 0;
}

// Operator management
void Channel::addOperator(int client_fd) {
    if (!isOperator(client_fd)) {
        _operators.push_back(client_fd);
    }
}

void Channel::removeOperator(int client_fd) {
    _operators.erase(std::remove(_operators.begin(), _operators.end(), client_fd), _operators.end());
}

bool Channel::isOperator(int client_fd) const {
    return std::find(_operators.begin(), _operators.end(), client_fd) != _operators.end();
}

const std::vector<int> &Channel::getOperators() const {
    return _operators;
}

// Invitation management
void Channel::inviteUser(int client_fd) {
    if (!isInvited(client_fd)) {
        _invited.push_back(client_fd);
    }
}

bool Channel::isInvited(int client_fd) const {
    return std::find(_invited.begin(), _invited.end(), client_fd) != _invited.end();
}

void Channel::removeInvite(int client_fd) {
    _invited.erase(std::remove(_invited.begin(), _invited.end(), client_fd), _invited.end());
}

const std::vector<int> &Channel::getInvited() const {
    return _invited;
}
