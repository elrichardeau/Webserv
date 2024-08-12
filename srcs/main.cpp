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
#include <csignal>
#include "../includes/Config.hpp"
#include "../includes/Requests.hpp"
#include "../includes/Server.hpp"
#include "../includes/ServerManager.hpp"

#define MAX_EVENTS 10

int signalCode = 1;

void signalHandler(int code) {
    if (code == SIGINT)
        signalCode = 0;
}

int main(int argc, char **argv) {
    signal(SIGINT, signalHandler);
    std::string filename;
    if (argc <= 2) {
        if (argc == 2)
            filename = argv[1];
        else
            filename = "server.conf";
    }
    try {
        Config config(filename);
        ServerManager manager(config.getServers());        
        manager.createSockets();
        for (size_t i = 0; i < manager.getServers().size(); i++) {
        }
        int epollFd = epoll_create1(0);
        if (epollFd == -1) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }
        manager.controlSockets(epollFd);
        while (signalCode) {
            struct epoll_event events[MAX_EVENTS];
            int num_fds = epoll_wait(epollFd, events, MAX_EVENTS, -1);
            if (num_fds == -1 && signalCode) {
                perror("epoll_wait");
                exit(EXIT_FAILURE);
            }
            if (num_fds == 0)
                continue;
            for (int i = 0; i < num_fds; ++i) {
                int index = manager.compareServerSocket(events[i].data.fd);
                if (index != -1)
                    manager.handleServerSocket(epollFd, index);
                else
                    manager.handleClientSocket(events[i]);
            }
        }
        close(epollFd);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE; 
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    return 0;
}
