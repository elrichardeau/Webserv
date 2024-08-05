#pragma once

#include <string>
#include <vector>
#include <arpa/inet.h>
#include "ServerConfig.hpp"
#include "ErrorPage.hpp"

class Server {

    public :

        Server();
        Server& operator=(const Server &copy);
        Server(ServerConfig serverConfig, int port);
        ~Server();

        struct sockaddr_in &getAddress();
        int &getAddrLen();
        std::string getHost() const;
        int getPort() const;
        std::string getServerName() const;
        void setServerSocket(int serverSocket);
        void setIsDup(bool status);
        int getServerSocket() const;
        bool getIsDup() const;
        void addClientSocket(int clientSocket);
        std::vector<int> getClientSockets() const;
        void rmClientSocket(int index);
        std::vector<LocationConfig> getLocations() const;
        ErrorPage getErrorPage() const;
        size_t getClientMaxBodySize() const;

    private :

        int _serverSocket;
        bool _isDup;
        struct sockaddr_in _address;
        int _addrLen;
        int _port;
        std::string _host;
        std::string _serverName;
        std::vector<int> _clientSockets;
        size_t _clientMaxBodySize;
        std::string _root;
        std::vector<LocationConfig> _locations;
        ErrorPage _errorPages;
};