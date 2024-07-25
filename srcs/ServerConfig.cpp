#include "../includes/ServerConfig.hpp"

ServerConfig::ServerConfig() : hasRoot(false), hasHost(false), hasListen(false)
{
}
// ServerConfig::ServerConfig(const std::string &host, const std::string &serverName, int port)
// : host(host), server_name(serverName), client_max_body_size(0), root("")
// {
//     ports.push_back(port);
// }

ServerConfig::ServerConfig(ServerConfig const &other) 
{
	host = other.host;
    server_name = other.server_name;
    client_max_body_size = other.client_max_body_size;
    root = other.root;
    locations = other.locations;
    error_pages = other.error_pages;
    ports = other.ports;
    hasRoot = other.hasRoot;
    hasHost = other.hasHost;
    hasListen = other.hasListen;
}

ServerConfig &ServerConfig::operator=(ServerConfig const &other)
{
	if (this != &other) 
	{
        host = other.host;
        server_name = other.server_name;
        client_max_body_size = other.client_max_body_size;
        root = other.root;
        locations = other.locations;
        error_pages = other.error_pages;
        ports = other.ports;
        hasRoot = other.hasRoot;
        hasHost = other.hasHost;
        hasListen = other.hasListen;
    }
    return (*this);
}

ServerConfig::~ServerConfig(){}


void ServerConfig::setHost(const std::string &h)
{ 
	this->host = h;
	hasHost = true;
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
	hasRoot = true;
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
	hasListen = true;
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

bool ServerConfig::isValid() const
{
	return (hasRoot && hasHost && hasListen);
}