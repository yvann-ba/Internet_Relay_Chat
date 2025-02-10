#ifndef SERVER_HPP
# define SERVER_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "client.hpp"

class Server {
public:
    Server();
    ~Server();

    void start(const char* portStr, const char* password);
    void run();

private:
    int _serverSocket;
    int _port;
    std::string _password;
    
    std::map<int, Client*> _clients;
};

#endif
