# include "Client.hpp"

Client::Client() {}

Client::Client(int fd,std::string &ip, std::string  &hostname) :_fd(fd), _ipAddress(ip),_hostname(hostname),_channelCount(0) {}

Client::Client(const Client &other){}

Client& Client::operator=(const Client &other)
{

}

Client::~Client() {}
