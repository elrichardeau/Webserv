#pragma once

#include <unistd.h>
#include <vector>
#include <map>
#include <stdexcept>
//#include <sys/epoll.h>
#include <arpa/inet.h>
#include "Server.hpp"
#include "Requests.hpp"

class ServerManager {

	public :

		ServerManager(std::vector<ServerConfig> servers);
		~ServerManager();

		void createSockets();
		void controlSockets(int epollFd);
		int compareServerSocket(int eventFd);
		void handleServerSocket(int epollFd, int index);
		int compareClientSocket(int eventFd);
		//void handleClientSocket(epoll_event event);

		std::vector<Server> getServers() const;

	private :

		std::vector<Server> _servers;
	

		class SocketFailed : public std::exception {
			const char* what() const throw();
		};
};