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

#include <map>
#include <string>
#include <utility>

void Server::sendError(int client_fd, int error_code, const std::string &param) {
    std::string clientNick = "*";
    if (_clients.find(client_fd) != _clients.end() && !_clients[client_fd]->getNickname().empty())
        clientNick = _clients[client_fd]->getNickname();

    // Build the numeric error messages.
    std::map<int, std::string> numeric_errors;
    numeric_errors[401] = ":server_name 401 " + param + " :No such nick/channel\r\n";
    numeric_errors[402] = ":server_name 402 " + param + " :No such server\r\n";
    numeric_errors[403] = ":server_name 403 " + param + " :No such channel\r\n";
    numeric_errors[404] = ":server_name 404 " + param + " :Cannot send to channel\r\n";
    numeric_errors[405] = ":server_name 405 " + param + " :You have joined too many channels\r\n";
    numeric_errors[411] = ":server_name 411 * :No recipient given\r\n";
    numeric_errors[412] = ":server_name 412 * :No text to send\r\n";
    numeric_errors[421] = ":server_name 421 " + param + " :Unknown command\r\n";
    numeric_errors[431] = ":server_name 431 * :No nickname given\r\n";
    numeric_errors[432] = ":server_name 432 " + param + " :Erroneous nickname\r\n";
    numeric_errors[433] = ":server_name 433 " + param + " :Nickname is already in use\r\n";
    numeric_errors[451] = ":server_name 451 * :You are not registered\r\n";
    numeric_errors[461] = ":server_name 461 " + param + " :Not enough parameters\r\n";
    numeric_errors[462] = ":server_name 462 * :You may not reregister\r\n";
    numeric_errors[464] = ":server_name 464 * :Password incorrect\r\n";
    numeric_errors[471] = ":server_name 471 " + param + " :Channel is full\r\n";
    numeric_errors[473] = ":server_name 473 " + param + " :Invite-only channel\r\n";
    numeric_errors[475] = ":server_name 475 " + param + " :Bad channel key (incorrect password)\r\n";
    numeric_errors[482] = ":server_name 482 " + param + " :You're not a channel operator\r\n";

    // Build the visual NOTICE messages (with mIRC color codes).
    std::map<int, std::string> notice_errors;
    notice_errors[401] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 401: No such nick/channel\x03" "\r\n";
    notice_errors[402] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 402: No such server\x03" "\r\n";
    notice_errors[403] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 403: No such channel\x03" "\r\n";
    notice_errors[404] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 404: Cannot send to channel\x03" "\r\n";
    notice_errors[405] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 405: You have joined too many channels\x03" "\r\n";
    notice_errors[411] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 411: No recipient given\x03" "\r\n";
    notice_errors[412] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 412: No text to send\x03" "\r\n";
    notice_errors[421] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 421: Unknown command\x03" "\r\n";
    notice_errors[431] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 431: No nickname given\x03" "\r\n";
    notice_errors[432] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 432: Erroneous nickname\x03" "\r\n";
    notice_errors[433] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 433: Nickname is already in use\x03" "\r\n";
    notice_errors[451] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 451: You are not registered\x03" "\r\n";
    notice_errors[461] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 461: Not enough parameters\x03" "\r\n";
    notice_errors[462] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 462: You may not reregister\x03" "\r\n";
    notice_errors[464] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 464: Password incorrect\x03" "\r\n";
    notice_errors[471] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 471: Channel is full\x03" "\r\n";
    notice_errors[473] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 473: Invite-only channel\x03" "\r\n";
    notice_errors[475] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 475: Bad channel key (incorrect password)\x03" "\r\n";
    notice_errors[482] = ":server_name NOTICE " + clientNick + " :\x03" "4Error 482: You're not a channel operator\x03" "\r\n";

    // Send the numeric error reply always.
    if (numeric_errors.find(error_code) != numeric_errors.end()) {
        send(client_fd, numeric_errors[error_code].c_str(), numeric_errors[error_code].length(), MSG_NOSIGNAL);
    }

    // Only send the NOTICE message if the client has a valid nickname (i.e. not "*" which indicates nc).
    if (clientNick != "*" && notice_errors.find(error_code) != notice_errors.end()) {
        send(client_fd, notice_errors[error_code].c_str(), notice_errors[error_code].length(), MSG_NOSIGNAL);
    }
}
