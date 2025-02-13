#include "../includes/client.hpp"

Client::Client() : _fd_socket(-1), _registered(false), _nickName(""), _userName(""), _realName("") {
    
}

Client::~Client()
{
	if (_fd_socket != -1)
		close(_fd_socket);
	return ;
}

void	Client::stopClient()
{
	if (_fd_socket != -1)
		close(_fd_socket);
	_fd_socket = -1;
	_registered = false;
	_nickName = "";
	_userName = "";
	_realName = "";
	return ;
}

void	Client::startClient(char **argv)
{
	_fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sockClient;

	sockClient.sin_family = AF_INET;
	sockClient.sin_port = htons(atoi(argv[1]));
	sockClient.sin_addr.s_addr = inet_addr(argv[2]);

	connect(_fd_socket, (struct sockaddr *)&sockClient, sizeof(sockClient));

	return ;
}