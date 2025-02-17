#include "../includes/utils.hpp"
#include "../includes/server.hpp"
#include <sstream>
#include <cctype>

std::string normalizeSpaces(const std::string& input) {
    std::string result;
    bool inSpaces = false;

    for (std::string::const_iterator it = input.begin(); it != input.end(); ++it) {
        char ch = *it;

        if (ch == ' ') {
            if (!inSpaces) {
                result += ' ';
                inSpaces = true;
            }
        } else {
            result += ch;
            inSpaces = false;
        }
    }

    return result;
}

int countWords(const std::string &str) {
    std::istringstream stream(str);
    std::string word;
    int count = 0;

    while (stream >> word) {
        count++;
    }
    return count;
}

bool isDisplayable(const std::string &str) {
    for (size_t i = 0; i < str.length(); i++) {
        if (!isprint(str[i])) {
            return false;
        }
    }
    return true;
}

void Server::sendError(int client_fd, int error_code, const std::string &param) {
    std::string clientNick = "*";
    if (_clients.find(client_fd) != _clients.end() && !_clients[client_fd]->getNickname().empty())
        clientNick = _clients[client_fd]->getNickname();

    std::map<int, std::pair<std::string, std::string> > error_messages;
    error_messages[401] = std::make_pair(
        ":server_name 401 " + param + " :No such nick/channel\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 401: No such nick/channel\x03" "\r\n"
    );
    error_messages[402] = std::make_pair(
        ":server_name 402 " + param + " :No such server\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 402: No such server\x03" "\r\n"
    );
    error_messages[403] = std::make_pair(
        ":server_name 403 " + param + " :No such channel\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 403: No such channel\x03" "\r\n"
    );
    error_messages[404] = std::make_pair(
        ":server_name 404 " + param + " :Cannot send to channel\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 404: Cannot send to channel\x03" "\r\n"
    );
    error_messages[405] = std::make_pair(
        ":server_name 405 " + param + " :You have joined too many channels\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 405: You have joined too many channels\x03" "\r\n"
    );
    error_messages[411] = std::make_pair(
        ":server_name 411 * :No recipient given\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 411: No recipient given\x03" "\r\n"
    );
    error_messages[412] = std::make_pair(
        ":server_name 412 * :No text to send\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 412: No text to send\x03" "\r\n"
    );
    error_messages[421] = std::make_pair(
        ":server_name 421 " + param + " :Unknown command\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 421: Unknown command\x03" "\r\n"
    );
    error_messages[431] = std::make_pair(
        ":server_name 431 * :No nickname given\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 431: No nickname given\x03" "\r\n"
    );
    error_messages[432] = std::make_pair(
        ":server_name 432 " + param + " :Erroneous nickname\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 432: Erroneous nickname\x03" "\r\n"
    );
    error_messages[433] = std::make_pair(
        ":server_name 433 " + param + " :Nickname is already in use\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 433: Nickname is already in use\x03" "\r\n"
    );
    error_messages[451] = std::make_pair(
        ":server_name 451 * :You are not registered\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 451: You are not registered\x03" "\r\n"
    );
    error_messages[461] = std::make_pair(
        ":server_name 461 " + param + " :Not enough parameters\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 461: Not enough parameters\x03" "\r\n"
    );
    error_messages[462] = std::make_pair(
        ":server_name 462 * :You may not reregister\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 462: You may not reregister\x03" "\r\n"
    );
    error_messages[464] = std::make_pair(
        ":server_name 464 * :Password incorrect\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 464: Password incorrect\x03" "\r\n"
    );
    error_messages[471] = std::make_pair(
        ":server_name 471 " + param + " :Channel is full\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 471: Channel is full\x03" "\r\n"
    );
    error_messages[473] = std::make_pair(
        ":server_name 473 " + param + " :Invite-only channel\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 473: Invite-only channel\x03" "\r\n"
    );
    error_messages[475] = std::make_pair(
        ":server_name 475 " + param + " :Bad channel key (incorrect password)\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 475: Bad channel key (incorrect password)\x03" "\r\n"
    );
    error_messages[482] = std::make_pair(
        ":server_name 482 " + param + " :You're not a channel operator\r\n",
        ":server_name NOTICE " + clientNick + " :\x03" "4Error 482: You're not a channel operator\x03" "\r\n"
    );

    if (error_messages.find(error_code) != error_messages.end()) {
        send(client_fd, error_messages[error_code].first.c_str(), error_messages[error_code].first.length(), MSG_NOSIGNAL);
        send(client_fd, error_messages[error_code].second.c_str(), error_messages[error_code].second.length(), MSG_NOSIGNAL);
    }
}
