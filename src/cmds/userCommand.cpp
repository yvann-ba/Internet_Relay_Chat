#include "../../includes/server.hpp"
#include "../../includes/utils.hpp"
#include <sstream>
#include <iostream>

void Server::userCommand(std::string content, int index)
{
    if (!_clients[index]->getPassOk()) {
        sendError(_clients[index]->getFDSocket(), 464, "");
        return;
    }
    
    if (_clients[index]->getRegistered() == true) {
        sendError(_clients[index]->getFDSocket(), 462, "");
        return;
    }

    std::istringstream iss(content);
    std::string username, hostname, servername;
    std::string realname;

    if (!(iss >> username >> hostname >> servername)) {
        sendError(_clients[index]->getFDSocket(), 461, "USER");
        return;
    }
    std::getline(iss, realname);
    while (!realname.empty() && realname[0] == ' ')
        realname.erase(0, 1);
    if (!realname.empty() && realname[0] == ':')
        realname.erase(0, 1);

    if (!isDisplayable(username) || !isDisplayable(realname)) {
        sendError(_clients[index]->getFDSocket(), 432, username);
        return;
    }

    _clients[index]->setUsername(username);
    _clients[index]->setRealName(realname);

    if (!_clients[index]->getNickname().empty() && !_clients[index]->getUsername().empty()) {
        std::stringstream portStream;
        portStream << _port;
        std::string portStr = portStream.str();

        std::string msg = ":localhost:" + portStr + " 001 " + _clients[index]->getNickname() +
                          " :Welcome to IRC " + _clients[index]->getNickname() + "!";
        sendServ(_clients[index]->getFDSocket(), msg);

        _clients[index]->setRegistered(true);
    }
}
