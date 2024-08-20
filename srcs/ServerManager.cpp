
#include "../includes/ServerManager.hpp"

ServerManager::ServerManager(std::vector<ServerConfig> servers) {
	for (size_t i = 0; i < servers.size(); i++) {
		std::vector<int> ports = servers[i].getPorts();
		for (size_t j = 0; j < ports.size(); j++)
			this->_servers.push_back(Server(servers[i], ports[j]));
	}
}

ServerManager::~ServerManager() {
	for (size_t i = 0; i < this->_servers.size(); i++) {
		std::vector<int> clients = this->_servers[i].getClientSockets();
		for (size_t j = 0; j < clients.size(); j++)
			close(clients[j]);
        close(this->_servers[i].getServerSocket());
	}
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
	struct epoll_event event = {};
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
	struct epoll_event event = {};
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

bool findServerWithSocket(Server &servParam, std::vector<Server> manager, int serverSocket, std::string serverName) {
	size_t find = serverName.find(":");
	if (find != std::string::npos)
		serverName.erase(find, serverName.size());
	if (serverName == "localhost" || isValidIPAddress(serverName)) {
		for (std::vector<Server>::iterator it = manager.begin(); it != manager.end(); it++) {
			if (it->getServerSocket() == serverSocket) {
				servParam = *it;
				return true;
			}
		}
	}
	else
		for (std::vector<Server>::iterator it = manager.begin(); it != manager.end(); it++) {
			if (it->getServerSocket() == serverSocket && it->getServerName() == serverName) {
				servParam = *it;
				return true;
			}
		}
	return false;
}

void ServerManager::handleClientSocket(epoll_event event) {
	std::vector<unsigned char> buffer(BUF_SIZE);
	int serverSocket = compareClientSocket(event.data.fd, 0);
	if (event.events & EPOLLHUP) {
		close(event.data.fd);
		return;
	}
	else if (event.events & EPOLLIN) {
		std::vector<unsigned char> data(0);
		std::vector<unsigned char>::iterator it;
		int bytesRead;
		while ((bytesRead = recv(event.data.fd, buffer.data(), BUF_SIZE, 0)) > 0) {
			buffer.resize(bytesRead);
			data.insert(data.end(), buffer.begin(), buffer.end());
			buffer.clear();
			buffer.resize(BUF_SIZE);
			it = std::search(data.begin(), data.end(),"\r\n\r\n", "\r\n\r\n" + 4);
			if (it != data.end()) {
				it += 4;
				break;
			}
		}
		if (bytesRead <= 0) {
			bzero(data.data(), data.size());
			compareClientSocket(event.data.fd, 1);
		}
		else {
			std::string request;
			std::vector<unsigned char> body;
			request.insert(request.end(), data.begin(), it);
			if (it != data.end())
				body.insert(body.end(), it, data.end());
			else
				body.insert(body.begin(), '\0');
			bzero(data.data(), data.size());
			std::map<std::string, std::string> headers = getHeaders(request);
			Server servParam;
			if (headers.empty()) {
				std::string response = createBadRequestResponse(this->_servers, serverSocket);
				send(event.data.fd, response.c_str(), response.size(), 0);
				return;
			}
			if (!findServerWithSocket(servParam, this->_servers, serverSocket, headers["Host"])) {
				compareClientSocket(event.data.fd, 1);
				return;
			}
			try {
				BodyType bodyType = getBodyType(headers["Content-Length"], headers["Transfer-Encoding"], headers["Content-Type"]);
				Requests req(servParam, headers, body, headers["Content-Length"], bodyType);
				req.setLocations();
				if (req.getBodyType()) {
					while ((bytesRead = recv(event.data.fd, buffer.data(), BUF_SIZE, 0)) > 0) {
						buffer.resize(bytesRead);
						req.getBody(buffer);
						buffer.clear();
						buffer.resize(BUF_SIZE);
						if (bytesRead < BUF_SIZE)
							break;
					}
					if (bytesRead <= 0)
						compareClientSocket(event.data.fd, 1);
				}
				req.checkData();
				std::string response = req.getResponse();
				send(event.data.fd, response.c_str(), response.size(), 0);
			}
			catch (const std::exception &e) {
				std::string response = createErrorResponse(servParam.getErrorPage(), atoi(e.what()));
				send(event.data.fd, response.c_str(), response.size(), 0);
			}
		}
	}
}

std::vector<Server> ServerManager::getServers() const {return this->_servers;}
const char* ServerManager::SocketFailed::what() const throw() {return "Error: Socket Failed !";}