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
    std::map<int, std::string> error_messages;
    error_messages[401] = ":server_name 401 " + param + " :No such nick/channel\r\n";
    error_messages[402] = ":server_name 402 " + param + " :No such server\r\n";
    error_messages[403] = ":server_name 403 " + param + " :No such channel\r\n";
    error_messages[404] = ":server_name 404 " + param + " :Cannot send to channel\r\n";
    error_messages[405] = ":server_name 405 " + param + " :You have joined too many channels\r\n";
    error_messages[411] = ":server_name 411 * :No recipient given\r\n";
    error_messages[412] = ":server_name 412 * :No text to send\r\n";
    error_messages[421] = ":server_name 421 " + param + " :Unknown command\r\n";
    error_messages[431] = ":server_name 431 * :No nickname given\r\n";
    error_messages[432] = ":server_name 432 " + param + " :Erroneous nickname\r\n";
    error_messages[433] = ":server_name 433 " + param + " :Nickname is already in use\r\n";
    error_messages[451] = ":server_name 451 * :You are not registered\r\n";
    error_messages[461] = ":server_name 461 " + param + " :Not enough parameters\r\n";
    error_messages[462] = ":server_name 462 * :You may not reregister\r\n";
    error_messages[464] = ":server_name 464 * :Password incorrect\r\n";
    error_messages[471] = ":server_name 471 " + param + " :Channel is full\r\n";
    error_messages[473] = ":server_name 473 " + param + " :Invite-only channel\r\n";
    error_messages[475] = ":server_name 475 " + param + " :Bad channel key (incorrect password)\r\n";
    error_messages[482] = ":server_name 482 " + param + " :You're not a channel operator\r\n";
    if (error_messages.find(error_code) != error_messages.end())
        send(client_fd, error_messages[error_code].c_str(), error_messages[error_code].length(), 0);
}