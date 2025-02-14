#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>

class Channel {
public:
    Channel(const std::string &name);
    ~Channel();

    // Basic getters
    const std::string &getName() const;
    const std::string &getTopic() const;
    const std::vector<int> &getMembers() const;

    // Topic management
    void setTopic(const std::string &topic);

    // Member management
    void addMember(int client_fd);
    void removeMember(int client_fd);
    bool isMember(int client_fd) const;
    void broadcastMessage(const std::string &msg, int sender_fd) const;

    // Channel mode management
    bool isInviteOnly() const;
    void setInviteOnly(bool flag);

    bool isTopicRestricted() const;
    void setTopicRestricted(bool flag);

    const std::string &getChannelKey() const;
    void setChannelKey(const std::string &key);
    void removeChannelKey();

    int getUserLimit() const;
    void setUserLimit(int limit);
    void removeUserLimit();

    // Operator management
    void addOperator(int client_fd);
    void removeOperator(int client_fd);
    bool isOperator(int client_fd) const;
    const std::vector<int> &getOperators() const;

    // Invitation management
    void inviteUser(int client_fd);
    bool isInvited(int client_fd) const;
    void removeInvite(int client_fd);
    const std::vector<int> &getInvited() const;

private:
    std::string _name;
    std::string _topic;
    std::vector<int> _members;

    // Channel mode variables
    bool _inviteOnly;
    bool _topicRestricted;
    std::string _channelKey;
    int _userLimit;
    std::vector<int> _operators;
    std::vector<int> _invited;
};

#endif
