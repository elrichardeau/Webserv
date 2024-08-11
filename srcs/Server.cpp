
#include "../includes/Server.hpp"

Server::Server() {}

Server::Server(ServerConfig serverConfig, int port) :   _serverSocket(-1),
                                                        _isDup(false),
                                                        _port(port),
                                                        _host(serverConfig.getHost()),
                                                        _serverName(serverConfig.getServerName()),
                                                        _clientSockets(0),
                                                        _clientMaxBodySize(serverConfig.getClientMaxBodySize()),
                                                        _root(serverConfig.getRoot()),
                                                        _locations(serverConfig.getLocations()),
                                                        _errorPages(ErrorPage(serverConfig.getErrorPages())) {
    this->_address.sin_family = AF_INET;
    struct addrinfo hints, *res, *p;
    struct sockaddr_in *ipv4;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo(this->_host.c_str(), NULL, &hints, &res);
    if (status != 0) {}
     for (p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            ipv4 = (struct sockaddr_in *)p->ai_addr;
            this->_address.sin_addr.s_addr = ipv4->sin_addr.s_addr;
            break;
        }
    }
     freeaddrinfo(res);
    this->_address.sin_port = htons(port);
    this->_addrLen = sizeof(this->_address);
}

Server& Server::operator=(const Server &copy) {
    if (this == &copy)
        return *this;
    this->_serverSocket = copy._serverSocket;
    this->_isDup = copy._isDup;
    this->_port = copy._port;
    this->_host = copy._host;
    this->_serverName = copy._serverName;
    this->_clientSockets = copy._clientSockets;
    this->_clientMaxBodySize = copy._clientMaxBodySize;
    this->_root = copy._root;
    this->_locations = copy._locations;
    this->_errorPages = copy._errorPages;
    this->_address.sin_family = copy._address.sin_family;
    this->_address.sin_addr.s_addr = copy._address.sin_addr.s_addr;
    this->_address.sin_port = copy._address.sin_port;
    this->_addrLen = copy._addrLen;

    return *this;
}

Server::~Server() {}
struct sockaddr_in& Server::getAddress() {return this->_address;}
int& Server::getAddrLen() {return this->_addrLen;}
std::string Server::getHost() const {return this->_host;}
int Server::getPort() const {return this->_port;}
std::string Server::getServerName() const {return this->_serverName;}
void Server::setServerSocket(int serverSocket) {this->_serverSocket = serverSocket;}
void Server::setIsDup(bool status) {this->_isDup = status;}
int Server::getServerSocket() const {return this->_serverSocket;}
bool Server::getIsDup() const {return this->_isDup;}
void Server::addClientSocket(int clientSocket) {this->_clientSockets.push_back(clientSocket);}
std::vector<int> Server::getClientSockets() const {return this->_clientSockets;}
void Server::rmClientSocket(int index) {this->_clientSockets.erase(this->_clientSockets.begin() + index);}
std::vector<LocationConfig> Server::getLocations() const {return this->_locations;}
ErrorPage Server::getErrorPage() const {return this->_errorPages;}
size_t Server::getClientMaxBodySize() const {return this->_clientMaxBodySize;}
std::string Server::getRoot() const {return this->_root;}