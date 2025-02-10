#include "includes/server.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    try {
        Server server;
        server.start(argv[1], argv[2]);
        std::cout << "Server is ready. Keep this window open to test the connection." << std::endl;
        server.run();
    } catch (std::exception &e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
