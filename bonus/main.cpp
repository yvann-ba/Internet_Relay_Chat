#include "server_bonus.hpp"
#include <iostream>

volatile sig_atomic_t g_running = 1;

void handleSigint(int signum)
{
	(void)signum;
	std::cout << "\nSIGINT received. Shutting down server gracefully..." << std::endl;
	g_running = 0;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
		return 1;
	}
	try {
		Server server;
		std::signal(SIGINT, handleSigint);
		server.start(argv[1], argv[2]);
		std::cout << "Server is ready. Keep this window open to test the connection." << std::endl;
		server.run();
	} catch (std::exception &e) {
		std::cerr << "Server error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
