#pragma once

#include <string>

class Server {

    public :

        Server(const std::string &host, const std::string &serverName, int port);
        ~Server();

        std::string getHost() const;
        std::string getServerName() const;
        int getPort() const;
        int getServerSocket() const;
        void setServerSocket(int serverSocket);
        struct sockaddr_in _address;
        int _addrLen;

    private :

        int _serverSocket;
        int _port;
        const std::string _host;
        const std::string _serverName;
};