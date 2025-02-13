#include "../../includes/server.hpp"
#include "../../includes/utils.hpp"

void    Server::nickCommand(std::string content, int index)
{
    std::string msg;

    if (_clients[index]->getFDSocket() == -1) {
        sendError(_clients[index]->getFDSocket(), 451, "");
        return ;
    }
    if (countWords(content) < 1) {
        sendError(_clients[index]->getFDSocket(), 461, "");
        return ;
    }
    if (!isDisplayable(content)) {
        sendError(_clients[index]->getFDSocket(), 432, "");
        return ;
    }
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (_clients[i]->getNickname() == _clients[index]->getNickname()) {
            sendError(_clients[index]->getFDSocket(), 433, "");
        }
    }
    std::stringstream portStream;
    portStream << _port;
    std::string portStr = portStream.str();
    if (_clients[index]->getNickname() == "") {
        if (_clients[index]->getNickname() != "" && _clients[index]->getRegistered() == true) {

            msg = ":localhost:" + portStr + " 001 " + content + " :Registered nickname " + content;
            sendServ(_clients[index]->getFDSocket(), msg);
        }
        else {
            msg = ":localhost:" + portStr + " 001 " + content + " :Registered nickname " + content;
            sendServ(_clients[index]->getFDSocket(), msg);
        }
    }
    else {
        msg = ":" + _clients[index]->getNickname() + "!@localhost:" +  " NICK :" + content;
        sendServ(_clients[index]->getFDSocket(), msg);
    }
    _clients[index]->setNickname(content);
}