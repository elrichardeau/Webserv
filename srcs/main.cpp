#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sstream>
#include "../includes/Config.hpp"
#include "../includes/Requests.hpp"

#define MAX_EVENTS 10
#define PORT 8080

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "2 args required." << std::endl;
        exit(1);
    }
    //std::string filename = argv[1];
    Config config;
    // std::cout << "Config object address: " << &config << std::endl;
    try
    {
        config = Config::readConfig(argv[1]);
        std::cout << "Servers loaded: " << config.getServers().size() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE; 
    }
        int server_fd, client_socket, epoll_fd; // Déclaration des descripteurs de fichiers
        struct sockaddr_in address; // Structure pour stocker les adresses IP et les numéros de port pour IPv4
        int opt = 1; // Option pour permettre le redémarrage rapide du serveur
        int addrlen = sizeof(address); // Taille de la structure adresse
        char buffer[1024] = {0}; // Buffer pour stocker les données reçues

        // Création de la socket
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // configuration des options de la socket
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET; // Famille d'adresses IPv4
        address.sin_addr.s_addr = INADDR_ANY; // Adresse IP sur laquelle écouter
        address.sin_port = htons(8080); // Numéro de port sur lequel écouter

        // Démarrage de l'écoute sur le port spécifié
        ////associe la socket au port 8080 sur toutes les interfaces reseau de la machine
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        // Écoute du port
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        // Création de l'instance epoll
        epoll_fd = epoll_create1(0);
        if (epoll_fd == -1)
        {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }
        // Ajouter le descripteur de fichier du serveur à la liste des événements surveillés
        struct epoll_event event;
        struct epoll_event events[MAX_EVENTS];
        event.events = EPOLLIN;
        event.data.fd = server_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
        {
            perror("epoll_ctl: server_fd");
            exit(EXIT_FAILURE);
        }

        while (true)
        {
            int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
            if (num_fds == -1)
            {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < num_fds; ++i)
            {
                if (events[i].data.fd == server_fd)
                {
                    // Accepter une nouvelle connexion
                    client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    if (client_socket < 0)
                    {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    // Ajouter le nouveau socket client à la liste des événements surveillés
                    event.events = EPOLLIN;
                    event.data.fd = client_socket;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &event) == -1)
                    {
                        perror("epoll_ctl: client_socket");
                        exit(EXIT_FAILURE);
                    }
                }
                else
                {
                    // Lire la requête du client
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
        close(server_fd);
    return (0);
}
