#include "./bot.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Include the bot and server headers
#include "../includes/server.hpp"

volatile sig_atomic_t g_running = 1;


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
    
    // Create the socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Error: Could not create socket." << std::endl;
        return EXIT_FAILURE;
    }

    // Set up the server address structure
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

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Error: Connection to " << ip << ":" << port << " failed." << std::endl;
        close(sock);
        return EXIT_FAILURE;
    }

    std::cout << "Connected to server at " << ip << ":" << port << std::endl;

    // Create a Server instance (assuming a default constructor)
    Server server;

    Bot bot(&server, "Bot42");
    bot.setFDSocket(sock);
    bot.setConnected(true);

	bot.registerBot(password, sock);

    char buffer[1024];

    while (true)
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

    close(sock);
    return EXIT_SUCCESS;
}
