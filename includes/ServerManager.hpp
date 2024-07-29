#pragma once

#include <vector>
#include <stdexcept>
#include "ServerConfig.hpp"
#include "Server.hpp"

class ServerManager {

	public :

		ServerManager(std::vector<ServerConfig> servers);
		~ServerManager();

		void createSockets();
		void controlSockets(int epollFd);
		int compareServerSocket(int eventFd);
		void handleServerSocket(int epollFd, int index);
		void handleClientSocket(epoll_event event);
		std::vector<Server> getServers() const;

	private :

		std::vector<Server> _servers;
	

		class SocketFailed : public std::exception {
			const char* what() const throw();
		};
};