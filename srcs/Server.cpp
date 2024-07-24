
#include <arpa/inet.h>
#include "../includes/Server.hpp"

Server::Server(const std::string &host, const std::string &serverName, int port) :_serverSocket(-1), _port(port), _host(host), _serverName(serverName) {
    this->_address.sin_family = AF_INET;
    this->_address.sin_addr.s_addr = inet_addr(host.c_str());
    this->_address.sin_port = htons(port);
    this->_addrLen = sizeof(this->_address);
}
Server::~Server() {}

std::string Server::getHost() const {return this->_host;}
std::string Server::getServerName() const {return this->_serverName;}
int Server::getPort() const {return this->_port;}
int Server::getServerSocket() const {return this->_serverSocket;}

void Server::setServerSocket(int serverSocket) {this->_serverSocket = serverSocket;}