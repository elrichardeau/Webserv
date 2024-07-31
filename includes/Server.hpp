#pragma once

#include <string>
#include <vector>
#include <arpa/inet.h>
#include "ServerConfig.hpp"
#include "ErrorPage.hpp"

class Server {

    public :

        Server(ServerConfig serverConfig, int port);
        ~Server();

        struct sockaddr_in &getAddress();
        int &getAddrLen();
        std::string getHost() const;
        int getPort() const;
        std::string getServerName() const;
        void setServerSocket(int serverSocket);
        int getServerSocket() const;
        void addClientSocket(int clientSocket);
        std::vector<int> getClientSockets() const;
        void rmClientSocket(int index);
        std::vector<LocationConfig> getLocations() const;
        ErrorPage getErrorPage() const;

    private :

        int _serverSocket;
        struct sockaddr_in _address;
        int _addrLen;
        int _port;
        const std::string _host;
        const std::string _serverName;
        std::vector<int> _clientSockets;
        int _clientMaxBodySize;
        std::string _root;
        std::vector<LocationConfig> _locations;
        ErrorPage _errorPages;
};