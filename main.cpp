/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niromano <niromano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 11:56:28 by niromano          #+#    #+#             */
/*   Updated: 2024/07/15 17:45:24 by niromano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fstream>
#include "requests.hpp"

#define LEN_BUF 30000
#define PORT 8080

void printFailed(const std::string &s) {
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
        printFailed("Socket Failed !");
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
        printFailed("Set socket opt Failed !");
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        printFailed("Bind Failed !");
    if (listen(server_fd, 1) < 0)
        printFailed("Listen Failed !");

    client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (client_fd == -1)
        printFailed("Accept new client Failed !");
    if (recv(client_fd, buf, LEN_BUF, 0) == -1)
        printFailed("Receive Failed !");
    Requests req = readRequest(buf);
    std::string response = req.getResponse();
    send(client_fd, response.c_str(), strlen(response.c_str()), 0);
    close(client_fd);
    close(server_fd);
    return 0;
}