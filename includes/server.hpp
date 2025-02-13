#ifndef SERVER_HPP
# define SERVER_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include "client.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <cstdio>
#include <sstream>
#include <string>

# define SIZE_MSG    1024
# define MAX_CLIENT    100

class Server {
	public:
		Server();
		~Server();

		void start(const char* portStr, const char* password);
		void run();

		// void closeClient(int i);
		void	sendError(int client_fd, int error_code, const std::string &param);
		void	sendServ(int fd, const std::string &msg);
		bool	checkIsRegistered(int i);
		void	processClientCommand(std::string* clientBuffer, int clientIndex);

		// Commands
		void	passCommand(std::string content, int index);
		void	nickCommand(std::string content, int index);
		void	userCommand(std::string content, int index);


	private:
		int			_serverSocket;
		int			_port;
		std::string	_password;
		
		std::map<int, Client*> _clients;
};

#endif
