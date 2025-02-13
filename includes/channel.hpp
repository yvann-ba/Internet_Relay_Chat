#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <vector>

class Channel {
public:
    
    Channel(const std::string &name);
    ~Channel();

    
    const std::string &getName() const;
    const std::string &getTopic() const;
    const std::vector<int> &getMembers() const;

    
    void setTopic(const std::string &topic);

    
    void addMember(int client_fd);
    void removeMember(int client_fd);
    bool isMember(int client_fd) const;

    
    void broadcastMessage(const std::string &msg, int sender_fd) const;

private:
    std::string _name;         
    std::string _topic;        
    std::vector<int> _members; 
};

#endif
