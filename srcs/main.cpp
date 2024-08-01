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
#include "../includes/ServerManager.hpp"

#define MAX_EVENTS 10

int main(int argc, char **argv) {
    std::string filename;
    if (argc <= 2) {
        if (argc == 2)
            filename = argv[1];
        else
            filename = "server.conf";
    }
    try {
        Config config(filename);
        std::cout << "Servers loaded: " << config.getServers().size() << std::endl;
        ServerManager manager(config.getServers());
        std::cout << "True Servers loaded: " << manager.getServers().size() << std::endl;
        manager.createSockets();
        for (size_t i = 0; i < manager.getServers().size(); i++) {
        }
        int epollFd = epoll_create1(0);
        if (epollFd == -1) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }
        manager.controlSockets(epollFd);
        while (true) {
            struct epoll_event events[MAX_EVENTS];
            int num_fds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
            if (num_fds == -1) {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }
            for (int i = 0; i < num_fds; ++i) {
                int index = manager.compareServerSocket(events[i].data.fd);
                if (index != -1)
                    manager.handleServerSocket(epollFd, index);
                else
                    manager.handleClientSocket(events[i]);
            }
        }
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE; 
    }
    return (0);
}
