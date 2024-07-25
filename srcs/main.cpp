#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sstream>
#include <arpa/inet.h>
#include "../includes/Config.hpp"
#include "../includes/Requests.hpp"
#include "../includes/Server.hpp"

#define MAX_EVENTS 10
#define PORT 8080

int indexServerSocket(int eventFd, std::vector<Server> serverList) {
    for (size_t i = 0; i < serverList.size(); i++)
        if (eventFd == serverList[i].getServerSocket())
            return i;
    return -1;
}

int main(int argc, char **argv)
{
    std::string filename;
    
    if (argc <= 2)
    {
        if (argc == 2)
            filename = argv[1];
        else if (argc == 1)
            filename = "server.conf";
    }
    
    try
    {
        Config config(filename);
        std::cout << "Servers loaded: " << config.getServers().size() << std::endl;
        std::vector<ServerConfig> servers;
    servers = config.getServers();
    std::vector<Server> serverList;
    for (size_t i = 0; i < servers.size(); i++) {
        for (size_t j = 0; j < servers[i].getPorts().size(); j++) {
            Server server(servers[i].getHost(), servers[i].getServerName(), servers[i].getPorts()[j]);
            serverList.push_back(server);
        }
    }
    std::cout << "True Servers loaded: " << serverList.size() << std::endl;
    for (size_t i = 0; i < serverList.size(); i++) 
    {
        int opt = 1; // Option pour permettre le redémarrage rapide du serveur

        // Création de la socket
        serverList[i].setServerSocket(socket(AF_INET, SOCK_STREAM, 0));
        if (serverList[i].getServerSocket() == -1)
            continue;
        // configuration des options de la socket
        if (setsockopt(serverList[i].getServerSocket(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        // Démarrage de l'écoute sur le port spécifié
        ////associe la socket au port 8080 sur toutes les interfaces reseau de la machine
        if (bind(serverList[i].getServerSocket(), (struct sockaddr *)&serverList[i]._address, serverList[i]._addrLen) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        // Écoute du port
        if (listen(serverList[i].getServerSocket(), 10) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    }
    // Création de l'instance epoll
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    // Ajouter le descripteur de fichier du serveur à la liste des événements surveillés
    struct epoll_event event;
    event.events = EPOLLIN;
    for (size_t i = 0; i < serverList.size(); i++)
    {
        event.data.fd = serverList[i].getServerSocket();
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, serverList[i].getServerSocket(), &event) == -1)
        {
            perror("epoll_ctl: server_fd");
            exit(EXIT_FAILURE);
        }
    }
    while (true)
    {
        struct epoll_event events[MAX_EVENTS];
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1)
        {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < num_fds; ++i)
        {
            int index = indexServerSocket(events[i].data.fd, serverList);
            if (index != -1)
            {
                // Accepter une nouvelle connexion
                int clientSocket = accept(serverList[index].getServerSocket(), (struct sockaddr *)&serverList[index]._address, (socklen_t *)&serverList[index]._addrLen);
                if (clientSocket < 0)
                {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                // Ajouter le nouveau socket client à la liste des événements surveillés
                event.events = EPOLLIN;
                event.data.fd = clientSocket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, clientSocket, &event) == -1)
                {
                    perror("epoll_ctl: client_socket");
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                // Lire la requête du client
                char buffer[1024] = {0};
                if(recv(events[i].data.fd, buffer, 1024, 0) <= 0)
                    close(events[i].data.fd);
                else {
                    Requests req = readRequest(buffer);
                    std::string response = req.getResponse();
                    // Envoyer une réponse au client
                    send(events[i].data.fd, response.c_str(), response.size(), 0);
                    // Fermer la connexion
                    close(events[i].data.fd);
                }
            }
        }
    }
    // Fermeture de la socket du serveur
    for (size_t i = 0; i < serverList.size(); i++)
        close(serverList[i].getServerSocket());
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE; 
    }
    return (0);
}
