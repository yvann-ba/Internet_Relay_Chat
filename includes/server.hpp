#ifndef SERVER_HPP
# define SERVER_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "client.hpp"
#include "channel.hpp"

# define SIZE_MSG    1024
# define MAX_CLIENT  100

class Server {
public:
    // Constructor / Destructor
    Server();
    ~Server();

    void start(const char* portStr, const char* password);
    void run();
    
    // Basic error handling
    void sendError(int client_fd, int error_code, const std::string &param);
    void sendServ(int fd, const std::string &msg);
    
    // Registration checks
    bool checkIsRegistered(int client_fd);
    
    // Command processing
    void processClientCommand(std::string* clientBuffer, int client_fd);
    
    // User commands
    void passCommand(std::string content, int client_fd);
    void nickCommand(std::string content, int client_fd);
    void userCommand(std::string content, int client_fd);
    
    // Channel commands
    void joinCommand(const std::string &parameters, int client_fd);
    void privmsgCommand(const std::string &parameters, int client_fd);
    
    // Additional commands
    void quitCommand(const std::string &parameters, int client_fd);
    void inviteCommand(const std::string &parameters, int client_fd);
    void topicCommand(const std::string &parameters, int client_fd);
    void kickCommand(const std::string &parameters, int client_fd);
    void modeCommand(const std::string &parameters, int client_fd);

private:
    // Socket info
    int _serverSocket;
    int _port;
    std::string _password;

    // Connected clients
    std::map<int, Client*> _clients;

    // Channels
    std::map<std::string, Channel*> _channels;
};

#endif


