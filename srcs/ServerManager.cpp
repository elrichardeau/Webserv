
#include "../includes/ServerManager.hpp"

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
	std::map<std::string, int> checkIpPort;
	std::string ipPort;
	for (size_t i = 0; i < this->_servers.size(); i++) {
		int opt = 1;
		ipPort = this->_servers[i].getHost() + ":" + itostr(this->_servers[i].getPort());
		
		for (std::map<std::string, int>::iterator it = checkIpPort.begin(); it != checkIpPort.end(); it++) {
			if (ipPort == it->first) {
				this->_servers[i].setServerSocket(it->second);
				this->_servers[i].setIsDup(true);
				break;
			}
		}
		if (this->_servers[i].getServerSocket() != -1)
			continue;
		this->_servers[i].setServerSocket(socket(AF_INET, SOCK_STREAM, 0));
		if (this->_servers[i].getServerSocket() == -1)
			continue;
		if (setsockopt(this->_servers[i].getServerSocket(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
			throw SocketFailed();
		if (bind(this->_servers[i].getServerSocket(), (struct sockaddr *)&this->_servers[i].getAddress(), this->_servers[i].getAddrLen()) < 0)
			throw SocketFailed();
		if (listen(this->_servers[i].getServerSocket(), 10) < 0)
			throw SocketFailed();
		checkIpPort.insert(std::make_pair(ipPort, this->_servers[i].getServerSocket()));
	}
}

void ServerManager::controlSockets(int epollFd) {
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLHUP;
	for (size_t i = 0; i < this->_servers.size(); i++) {
		if (this->_servers[i].getIsDup() == true)
			continue;
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

int ServerManager::compareClientSocket(int eventFd, bool forClose) {
	for (size_t i = 0; i < this->_servers.size(); i++) {
		std::vector<int> clientSockets = this->_servers[i].getClientSockets();
		for (size_t j = 0; j < clientSockets.size(); j++) {
			if (eventFd == clientSockets[j]) {
				if (forClose) {
					this->_servers[i].rmClientSocket(j);
					close(eventFd);
					return -1;
				}
				return this->_servers[i].getServerSocket();
			}
		}
	}
    return -1;
}

void ServerManager::handleClientSocket(epoll_event event) {
	char buffer[BUF_SIZE] = {0};
	int serverSocket = compareClientSocket(event.data.fd, 0);
	if (event.events & EPOLLHUP) {
		close(event.data.fd);
		return ;
	}
	else if (event.events & EPOLLIN) {
		std::string requestData;
		int bytesRead;
		while ((bytesRead = recv(event.data.fd, buffer, BUF_SIZE, 0)) > 0) {
			buffer[bytesRead] = '\0';
			requestData.append(buffer);
			bzero(buffer, sizeof(buffer));
			if (requestData.find("\r\n\r\n") != std::string::npos)
				break;
		}
		if (bytesRead <= 0)
			compareClientSocket(event.data.fd, 1);
		else {
			Requests req(requestData, this->_servers, serverSocket);
			if (req.getBody("")) {
				while ((bytesRead = recv(event.data.fd, buffer, BUF_SIZE, 0)) > 0) {
					buffer[bytesRead] = '\0';
					req.getBody(buffer);
					bzero(buffer, sizeof(buffer));
					if (bytesRead < BUF_SIZE)
						break;
				}
				if (bytesRead <= 0)
					compareClientSocket(event.data.fd, 1);
			}
			std::string response = req.getResponse();
			if (response == "")
				compareClientSocket(event.data.fd, 1);
			else
				send(event.data.fd, response.c_str(), response.size(), 0);
		}
	}
}






std::vector<Server> ServerManager::getServers() const {return this->_servers;}
const char* ServerManager::SocketFailed::what() const throw() {return "Error: Socket Failed !";}
