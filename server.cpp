#include <sys/epoll.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <stdio.h>

#define MAX_EVENTS 10
#define PORT 8080

int main() {
    int server_fd, client_fd, epoll_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];

    // Créer les sockets serveur : tableau avec le nb de fd serveur
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Réutilisation de l'adresse et du port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Assigner l'adresse et le port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    // Créer l'instance epoll
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1 failed");
        exit(EXIT_FAILURE);
    }

    // Ajouter le descripteur de fichier du serveur à epoll
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl failed");
        exit(EXIT_FAILURE);
    }

    // Boucle principale
    while (true) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait failed");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server_fd) {
                // Nouvelle connexion
                client_fd = accept(events[i].data.fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                if (client_fd == -1) {
                    perror("accept failed");
                    exit(EXIT_FAILURE);
                }
                // Ajouter le descripteur de fichier du client à epoll
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    perror("epoll_ctl failed");
                    exit(EXIT_FAILURE);
                }
            } else {
                // Lire les données du client
                char buffer[1024] = {0};
                if (recv(client_fd, buffer, 1024, 0) == -1){
                    perror("recv failed");
                    close(events[i].data.fd);
                }
                else {
					//insérer la fonction de parsing de la requête ici
					//insérer la fonction de réponse aux requêtes ici
                    std::cout << "Message received: " << buffer << std::endl;
                    const char *response =
                        "HTTP/1.1 200 OK\n"
                        "Content-Type: text/html\n"
                        "Content-Length: 46\n"
                        "\n"
                        "<html><body><h1>Hello from server</h1></body></html>";
                    send(events[i].data.fd, response, strlen(response), 0);
					close(events[i].data.fd);
                }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    return 0;
}