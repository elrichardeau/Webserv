#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig(){}

ServerConfig::ServerConfig(ServerConfig const &other) 
{
	(void)other;
}

ServerConfig &ServerConfig::operator=(ServerConfig const &other)
{
	(void)other;
	return (*this);
}

ServerConfig::~ServerConfig(){}

void ServerConfig::setListen(int l)
{ 
	listen = l;
}

void ServerConfig::setHost(const std::string &h)
{ 
	host = h;
}

void ServerConfig::setServerName(const std::string &name)
{ 
	server_name = name; 
}

void ServerConfig::setClientMaxBodySize(int size)
{ 
	client_max_body_size = size;
}

void ServerConfig::setRoot(const std::string &rt)
{ 
	root = rt;
}

void ServerConfig::addLocation(const LocationConfig &loc) 
{ 
	locations.push_back(loc);
}

void ServerConfig::addErrorPage(const ErrorPageConfig &error)
{ 
	error_pages.push_back(error);
}

int ServerConfig::getListen() const {return this->listen;}
std::string ServerConfig::getHost() const {return this->host;}

void ServerConfig::addHost(const std::string &portNumber)
{ 
	ports.push_back(portNumber);
}
std::string ServerConfig::getServerName() const
{ 
	return server_name;
}

int ServerConfig::getClientMaxBodySize() const
{ 
	return client_max_body_size;
}

std::string ServerConfig::getRoot() const 
{ 
	return root;
}

std::vector<LocationConfig> ServerConfig::getLocations() const 
{ 
	return locations;
}

std::vector<ErrorPageConfig> ServerConfig::getErrorPages() const
{
	return error_pages; 
}