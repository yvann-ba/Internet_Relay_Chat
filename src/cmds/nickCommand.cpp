#include "../../includes/server.hpp"
#include "../../includes/utils.hpp"
#include <sstream>

void Server::nickCommand(std::string content, int index)
{
    if (!_clients[index]->getPassOk()) {
        sendError(_clients[index]->getFDSocket(), 464, "");
        return;
    }
    
    if (_clients[index]->getFDSocket() == -1) {
        sendError(_clients[index]->getFDSocket(), 451, "");
        return;
    }
    if (countWords(content) < 1) {
        sendError(_clients[index]->getFDSocket(), 461, "");
        return;
    }
    if (!isDisplayable(content)) {
        sendError(_clients[index]->getFDSocket(), 432, "");
        return;
    }
    for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second->getNickname() == content) {
            sendError(_clients[index]->getFDSocket(), 433, "");
            return;
        }
    }
    std::stringstream portStream;
    portStream << _port;
    std::string portStr = portStream.str();

    if (_clients[index]->getNickname().empty()) {
        std::string msg = ":localhost:" + portStr + " 001 " + content + " :Registered nickname " + content;
        sendServ(_clients[index]->getFDSocket(), msg);
    } else {
        std::string msg = ":" + _clients[index]->getNickname() + "!@localhost:" +  " NICK :" + content;
        sendServ(_clients[index]->getFDSocket(), msg);
    }
    _clients[index]->setNickname(content);
}
