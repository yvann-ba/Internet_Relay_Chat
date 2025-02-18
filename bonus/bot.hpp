#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include "../includes/client.hpp"

class Server;

class Bot : public Client {
private:
	Server*     _server;
	std::string _botName;
	bool        _connected;

public:
	Bot(Server* server, const std::string& botName);
	virtual ~Bot();

	void registerBot();

	void connectToServer();

	void sendBotMessage(int clientFd, const std::string& msg);
	void respondToMessage(const std::string& message, int clientFd);

	bool isConnected() const { return _connected; }
	void setConnected(bool connected) { _connected = connected; }
};

#endif
