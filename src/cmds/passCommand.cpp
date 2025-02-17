#include "../../includes/server.hpp"

void Server::passCommand(std::string content, int index)
{
    if (_clients[index]->getRegistered() == true) {
        sendError(_clients[index]->getFDSocket(), 462, "");
        return;
    }
    if (content != _password) {
        sendError(_clients[index]->getFDSocket(), 464, "");
        return;
    }
    _clients[index]->setPassOk(true);

    std::string msg = "Correct password";
    sendServ(_clients[index]->getFDSocket(), msg);
}
