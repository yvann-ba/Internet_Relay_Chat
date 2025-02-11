#ifndef SERVER_HPP
# define SERVER_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "client.hpp"

# define SIZE_MSG    1024
# define MAX_CLIENT    100

class Server {
	public:
		Server();
		~Server();

		void start(const char* portStr, const char* password);
		void run();

		// void closeClient(int i);
		void	sendError(Client& client, std::string errorCode,std::string errorMsg);
		bool	checkIsRegistered(int i);
		void	processClientCommand(std::string* clientBuffer, int clientIndex);

	private:
		int			_serverSocket;
		int			_port;
		std::string	_password;
		
		std::map<int, Client*> _clients;
};

#endif
