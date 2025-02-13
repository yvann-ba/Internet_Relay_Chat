#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <string>
#include <vector>

class Channel {
public:
    // Constructeur qui initialise le canal avec son nom et un topic vide
    Channel(const std::string &name);
    ~Channel();

    // Accesseurs
    const std::string &getName() const;
    const std::string &getTopic() const;
    const std::vector<int> &getMembers() const;

    // Modifie le topic du canal
    void setTopic(const std::string &topic);

    // Gestion des membres
    void addMember(int client_fd);
    void removeMember(int client_fd);
    bool isMember(int client_fd) const;

    // Diffuse un message à tous les membres sauf l'expéditeur
    void broadcastMessage(const std::string &msg, int sender_fd) const;

private:
    std::string _name;         // Nom du canal, par exemple "#general"
    std::string _topic;        // Sujet du canal
    std::vector<int> _members; // Liste des descripteurs de socket des membres
};

#endif
