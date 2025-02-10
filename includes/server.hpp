#ifndef SERVER_HPP
# define SERVER_HPP

# include <stdexcept>
# include "client.hpp"

# define MAX_CLIENT  100

class Server {
public:
	Server();
	~Server();

	// Method to start the server by passing the port as a parameter.
	void start(const char* portStr);
	void run();

	int		acceptAndConfigureClient();
	void	handleClientCommunication(int clientSocket);

private:
	int _serverSocket; // The server socket file descriptor.
	int _port;         // The port number (converted from string to int).
	
	Client	_clientList[MAX_CLIENT];
};

#endif
