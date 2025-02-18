#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>

std::string generateBotResponse(const std::string& input) {
	if (input == "!hello")
		return "PRIVMSG bot :Hello! I am bot, your friendly bot.";
	else if (input == "!help")
		return "PRIVMSG bot :Commands: !hello, !joke, !help.";
	else if (input == "!joke")
		return "PRIVMSG bot :Why don't programmers like nature? It has too many bugs!";
	else
		return "PRIVMSG bot :I don't understand that command. Try !help.";
}

int main(int argc, char* argv[]) {

}
