#include "server.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }
    
    try {
        Server server;
        server.start(argv[1]);
        std::cout << "Server is ready. Keep this window open to test the connection." << std::endl;
        // Infinite loop to keep the server running (to be replaced by an event loop later)
        while (true) { }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
