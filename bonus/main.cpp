#include "./bot.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <csignal>
#include <sys/select.h>
#include <errno.h>
#include <ctime>

#include "../includes/server.hpp"

volatile sig_atomic_t g_running = 1;

void handle_sigint(int signum)
{
	(void)signum;
	g_running = 0;
	std::cout << "\nSIGINT Signal" << std::endl;
}

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <IP_ADDRESS> <PORT> <PASSWORD>" << std::endl;
		return EXIT_FAILURE;
	}

	const char* ip = argv[1];
	int port = std::atoi(argv[2]);
	std::string password = argv[3];
	
	struct sigaction sa;
	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_sigint;

	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		std::cerr << "Erreur lors de l'installation du handler SIGINT." << std::endl;
		return EXIT_FAILURE;
	}

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		std::cerr << "Error: Could not create socket." << std::endl;
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_addr;
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
	{
		std::cerr << "Error: Invalid IP address." << std::endl;
		close(sock);
		return EXIT_FAILURE;
	}

	if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr << "Error: Connection to " << ip << ":" << port << " failed." << std::endl;
		close(sock);
		return EXIT_FAILURE;
	}

	std::cout << "Connected to server at " << ip << ":" << port << std::endl;

	Server server;

	Bot bot(&server, "Bot42");
	bot.setFDSocket(sock);
	bot.setConnected(true);

	bot.registerBot(password, sock);

	char buffer[1024];

	while (g_running)
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);

		struct timespec timeout;
		timeout.tv_sec = 1;
		timeout.tv_nsec = 0;

		sigset_t empty_mask;
		sigemptyset(&empty_mask);

		int retval = pselect(sock + 1, &readfds, NULL, NULL, &timeout, &empty_mask);
		if (retval == -1)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "Error: pselect failed." << std::endl;
			break;
		}
		else if (retval > 0 && FD_ISSET(sock, &readfds))
		{
			std::memset(buffer, 0, sizeof(buffer));
			ssize_t bytes_read = read(sock, buffer, sizeof(buffer) - 1);
			if (bytes_read <= 0)
			{
				std::cout << "Disconnected from server or read error." << std::endl;
				break;
			}
			
			std::string message(buffer);
			std::cout << "Received: " << message;

			size_t pos = message.find("PRIVMSG");
			if (pos != std::string::npos)
			{
				size_t colon = message.find(":", pos);
				if (colon != std::string::npos)
				{
					std::string command = message.substr(colon + 1);
					size_t cr_pos = command.find("\r");
					if (cr_pos != std::string::npos)
						command.erase(cr_pos);
					size_t nl_pos = command.find("\n");
					if (nl_pos != std::string::npos)
						command.erase(nl_pos);

					std::cout << "Command extracted: " << command << std::endl;

					bot.respondToMessage(message, sock);
				}
			}
		}
	}

	close(sock);
	std::cout << "Fermeture de la connexion et du programme." << std::endl;
	return EXIT_SUCCESS;
}