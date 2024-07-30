
#include "../includes/ServerManager.hpp"

#define MAX_EVENTS 10
#define BUF_SIZE 1024

ServerManager::ServerManager(std::vector<ServerConfig> servers) {
	for (size_t i = 0; i < servers.size(); i++) {
		std::vector<int> ports = servers[i].getPorts();
		for (size_t j = 0; j < ports.size(); j++)
			this->_servers.push_back(Server(servers[i], ports[j]));
	}
}

ServerManager::~ServerManager() {
	for (size_t i = 0; i < this->_servers.size(); i++)
        close(this->_servers[i].getServerSocket());
}

void ServerManager::createSockets() {
	for (size_t i = 0; i < this->_servers.size(); i++) {
		int opt = 1;
		this->_servers[i].setServerSocket(socket(AF_INET, SOCK_STREAM, 0));
		if (this->_servers[i].getServerSocket() == -1)
			continue;
		if (setsockopt(this->_servers[i].getServerSocket(), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
			throw SocketFailed();
		if (bind(this->_servers[i].getServerSocket(), (struct sockaddr *)&this->_servers[i].getAddress(), this->_servers[i].getAddrLen()) < 0)
			throw SocketFailed();
		if (listen(this->_servers[i].getServerSocket(), 10) < 0)
			throw SocketFailed();
	}
}

void ServerManager::controlSockets(int epollFd) {
	struct epoll_event event;
	event.events = EPOLLIN;
	for (size_t i = 0; i < this->_servers.size(); i++) {
		event.data.fd = this->_servers[i].getServerSocket();
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, this->_servers[i].getServerSocket(), &event) == -1)
			throw SocketFailed();
	}
}

int ServerManager::compareServerSocket(int eventFd) {
	for (size_t i = 0; i < this->_servers.size(); i++)
        if (eventFd == this->_servers[i].getServerSocket())
            return i;
    return -1;
}

void ServerManager::handleServerSocket(int epollFd, int index) {
	struct epoll_event event;
	int clientSocket = accept(this->_servers[index].getServerSocket(), (struct sockaddr *)&this->_servers[index].getAddress(), (socklen_t *)&this->_servers[index].getAddrLen());
	if (clientSocket < 0)
		throw SocketFailed();
	event.events = EPOLLIN;
	event.data.fd = clientSocket;
	this->_servers[index].addClientSocket(clientSocket);
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientSocket, &event) == -1)
		throw SocketFailed();
}

int ServerManager::compareClientSocket(int eventFd) {
	for (size_t i = 0; i < this->_servers.size(); i++) {
		std::vector<int> clientSockets = this->_servers[i].getClientSockets();
		for (size_t j = 0; j < clientSockets.size(); j++) {
			if (eventFd == clientSockets[j]) {
				this->_servers[i].rmClientSocket(j);
				return i;
			}
		}
	}
    return -1;
}

void ServerManager::handleClientSocket(epoll_event event) {
	char buffer[BUF_SIZE] = {0};
	int index = compareClientSocket(event.data.fd);
	std::cout << "index : " << index << std::endl;
	if(recv(event.data.fd, buffer, BUF_SIZE, 0) <= 0)
		close(event.data.fd);
	else {
		Requests req(buffer, this->_servers[index]);
		// std::cout  << "PATH FOR PY = " << "." << req.getCgiPathPy() << "." << std::endl;
		std::string response = req.getResponse();
		send(event.data.fd, response.c_str(), response.size(), 0);
		close(event.data.fd);
	}
}

std::vector<Server> ServerManager::getServers() const {return this->_servers;}
const char* ServerManager::SocketFailed::what() const throw() {return "Error: Socket Failed !";}

