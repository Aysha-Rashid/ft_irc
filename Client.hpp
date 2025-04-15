# ifndef CLIENT_HPP
# define CLIENT_HPP

# include <sys/socket.h>
# include <string>
# include <time.h>

enum    clientState
{   
	UNAUTHENTICATED,
	AUTHENTICATED,
	REGISTERED,
};  

class Client    
{
	private:
		int			_fd;
		std::string _ipAddress;
		std::string _username;
		std::string _nickname;
		std::string _realname;
		clientState _state;
		int			_channelCount;
		time_t		_lastActivity;
		
	public:
		Client();  
		Client(const Client &other);
		Client &operator = (const Client &other);
		Client(int fd, std::string &ip);
		~Client();

		std::string _receiveBuffer;
		//setters
		void		setSocketFd(int socketFd);
		void		setNickName(std::string &nickname);
		void		setUserName(std::string &username);
		void		setRealName(std::string &realname);
		void		setState(clientState state);
		void		updateLastActivity();
		bool		isInactive(time_t timeout) const;

		//getters
		int			getSocketFd(void) const;
		std::string	getUserName(void) const;
		std::string	getRealName(void) const;
		std::string	getNickName(void) const;
		std::string	getIpAddress(void) const;
		std::string	getPrefix(void) const;
		clientState	getState(void) const;
		int			getChannelCount(void) const;
		time_t		getLastActivity(void) const;

		//member    functions
		void		write(const std::string msg) const;
		void		incrementChannelCount(void);
		void		decrementChannelCount(void);
};

# endif