#include "includes/server.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
		return 1;
	}
	
	Server server;
	server.start(argv[1]);
	std::cout << "Server is ready. Keep this window open to test the connection." << std::endl;
	server.run();
	
	return 0;
}
