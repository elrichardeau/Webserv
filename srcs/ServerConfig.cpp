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


void ServerConfig::setHost(const std::string &h)
{ 
	this->host = h;
}

void ServerConfig::setServerName(const std::string &name)
{ 
	this->server_name = name; 
}

void ServerConfig::setClientMaxBodySize(int size)
{ 
	this->client_max_body_size = size;
}

void ServerConfig::setRoot(const std::string &rt)
{ 
	this->root = rt;
}

void ServerConfig::addLocation(const LocationConfig &loc) 
{ 
	locations.push_back(loc);
}

void ServerConfig::addErrorPage(const ErrorPageConfig &error)
{ 
	error_pages.push_back(error);
}

std::string ServerConfig::getHost() const 
{ 
	return (this->host);
}

std::vector<int> ServerConfig::getPorts() const
{
	return (this->ports);
}

void ServerConfig::addPort(int portNumber)
{ 
	ports.push_back(portNumber);
}
std::string ServerConfig::getServerName() const
{ 
	return (this->server_name);
}

int ServerConfig::getClientMaxBodySize() const
{ 
	return (this->client_max_body_size);
}

std::string ServerConfig::getRoot() const 
{ 
	return (this->root);
}

std::vector<LocationConfig> ServerConfig::getLocations() const 
{ 
	return (this->locations);
}

std::vector<ErrorPageConfig> ServerConfig::getErrorPages() const
{
	return (this->error_pages); 
}