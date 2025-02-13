#ifndef SERVER_HPP
# define SERVER_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "client.hpp"
#include "channel.hpp"

# define SIZE_MSG    1024
# define MAX_CLIENT    100

class Server {
    public:
        Server();
        ~Server();

        void start(const char* portStr, const char* password);
        void run();

        void sendError(Client& client, std::string errorCode, std::string errorMsg);
        bool checkIsRegistered(int client_fd);
        void processClientCommand(std::string* clientBuffer, int client_fd);

        
        void joinCommand(const std::string &parameters, int client_fd);
        void privmsgCommand(const std::string &parameters, int client_fd);
        

    private:
        int _serverSocket;
        int _port;
        std::string _password;
        std::map<int, Client*> _clients;
        
        std::map<std::string, Channel*> _channels;
};

#endif
