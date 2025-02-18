#ifndef SERVER_HPP
# define SERVER_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <csignal>
#include "client.hpp"
#include "channel.hpp"

# define SIZE_MSG    1024
# define MAX_CLIENT  100

extern volatile sig_atomic_t g_running;

class Server {
public:
    
    Server();
    ~Server();

    void start(const char* portStr, const char* password);
    void run();
    
    std::string getPassword() const { return _password; }
    
    void sendError(int client_fd, int error_code, const std::string &param);
    void sendServ(int fd, const std::string &msg);
    
    
    bool checkIsRegistered(int client_fd);
    
    
    void processClientCommand(std::string* clientBuffer, int client_fd);
    
    
    void passCommand(std::string content, int client_fd);
    void nickCommand(std::string content, int client_fd);
    void userCommand(std::string content, int client_fd);
    
    
    void joinCommand(const std::string &parameters, int client_fd);
    void privmsgCommand(const std::string &parameters, int client_fd);
    
    
    void quitCommand(const std::string &parameters, int client_fd);
    void inviteCommand(const std::string &parameters, int client_fd);
    void topicCommand(const std::string &parameters, int client_fd);
    void kickCommand(const std::string &parameters, int client_fd);
    void modeCommand(const std::string &parameters, int client_fd);
    void removeClientFromChannels(int client_fd);


private:
    
    int _serverSocket;
    int _port;
    std::string _password;

    
    std::map<int, Client*> _clients;

    
    std::map<std::string, Channel*> _channels;
};

#endif


