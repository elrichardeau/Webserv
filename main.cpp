#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LEN_BUF 30000
#define PORT 8080

void print_failed(const std::string &s) {
    std::cout << s << std::endl;
    exit(1);
}

int main() {
    int server_fd;
    int client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buf[LEN_BUF] = {0};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        print_failed("Socket Failed !");
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        print_failed("Set socket opt Failed !");
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        print_failed("Bind Failed !");
    if (listen(server_fd, 1) < 0)
        print_failed("Listen Failed !");
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) == -1)
        print_failed("Accept new client Failed !");
    if (recv(client_fd, buf, LEN_BUF, 0) == -1)
        print_failed("Receive Failed !");
    std::cout << "Message from client: " << buf << std::endl;
    send(client_fd, buf, strlen(buf), 0);
    close(client_fd);
    close(server_fd);
    return 0;
}