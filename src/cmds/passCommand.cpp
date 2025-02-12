#include "../../includes/server.hpp"
// #include <sstream>
// #include <string>

void Server::passCommand(std::string content, int index)
{
    if (_clients[index]->getRegistered() == true) {
        sendError(_clients[index]->getFDSocket(), 462, "");
        return;
    }
    if (content == _password) {
        std::string msg = "Correct password, " + _clients[index]->getNickname();
        _clients[index]->setRegistered(true);
        sendServ(_clients[index]->getFDSocket(), msg);

        if (!_clients[index]->getNickname().empty() && !_clients[index]->getUsername().empty())
        {
            // Use std::stringstream for the port
            std::stringstream portStream;
            portStream << _port;
            std::string portStr = portStream.str();

            msg = ":localhost:" + portStr + " 001 " + _clients[index]->getNickname() + 
                  " :Welcome to IRC " + _clients[index]->getNickname() + "!";
            sendServ(_clients[index]->getFDSocket(), msg);
        }
    }
    else {
        sendError(_clients[index]->getFDSocket(), 464, "");
        return;
    }
}
