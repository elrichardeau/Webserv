
#include "../includes/Server.hpp"

Server::Server(ServerConfig serverConfig, int port) :   _serverSocket(-1),
                                                        _port(port),
                                                        _host(serverConfig.getHost()),
                                                        _serverName(serverConfig.getServerName()),
                                                        _clientSockets(0),
                                                        _clientMaxBodySize(serverConfig.getClientMaxBodySize()),
                                                        _root(serverConfig.getRoot()),
                                                        _locations(serverConfig.getLocations()) {
    // remove
    std::vector<ErrorPageConfig> errorPageConfig = serverConfig.getErrorPages();
    for (size_t i = 0; i < errorPageConfig.size(); i++) {
        std::map<int, std::string> mapError = errorPageConfig[i].getErrorPages();
        this->_errorPages.insert(mapError.begin(), mapError.end());
    }
    // this->_errorPages = serverConfig.getErrorPages();
    this->_address.sin_family = AF_INET;
    this->_address.sin_addr.s_addr = inet_addr(this->_host.c_str());
    this->_address.sin_port = htons(port);
    this->_addrLen = sizeof(this->_address);
}

Server::~Server() {}
struct sockaddr_in& Server::getAddress() {return this->_address;}
int& Server::getAddrLen() {return this->_addrLen;}
std::string Server::getHost() const {return this->_host;}
int Server::getPort() const {return this->_port;}
std::string Server::getServerName() const {return this->_serverName;}
void Server::setServerSocket(int serverSocket) {this->_serverSocket = serverSocket;}
int Server::getServerSocket() const {return this->_serverSocket;}
void Server::addClientSocket(int clientSocket) {this->_clientSockets.push_back(clientSocket);}
std::vector<int> Server::getClientSockets() const {return this->_clientSockets;}
void Server::rmClientSocket(int index) {this->_clientSockets.erase(this->_clientSockets.begin() + index);}