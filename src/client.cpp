#include "../includes/client.hpp"

Client::Client() : _fd_socket(-1), _registered(false), _passOk(false), _nickName(""), _userName(""), _realName("") {
    
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
	_passOk = false;
	_nickName = "";
	_userName = "";
	_realName = "";
	return ;
}