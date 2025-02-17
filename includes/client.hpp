# ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <cstdlib>

class Client
{
	private:
		int			_fd_socket;
		bool		_registered;
		bool        _passOk;
		std::string _nickName;
		std::string _userName;
		std::string _realName;
	
	public:

	Client();
	~Client();

	
	void	stopClient();

	
		int						getFDSocket() const {return _fd_socket; }
		bool					getRegistered() const {return _registered; }
		bool					getPassOk() const { return _passOk; }
		const std::string		getNickname() const {return _nickName; }
		const std::string		getUsername() const {return _userName; }
		const std::string		getRealName() const {return _realName; }
	
	
		void		setFDSocket(const int new_socket) {_fd_socket = new_socket; }
		void		setRegistered(const bool is_registered) {_registered = is_registered; }
		void		setPassOk(const bool passOk) { _passOk = passOk; }
		void		setNickname(std::string nickName) {_nickName = nickName; }
		void		setUsername(std::string userName) {_userName = userName; }
		void		setRealName(std::string realName) {_realName = realName; }
	

} ;


#endif