#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../includes/Requests.hpp"

std::vector<std::string> split(std::string buf, std::string find) {
	std::vector<std::string> vector;
	if (buf.find(find) == std::string::npos) {
		vector.push_back(buf.substr(0, buf.size()));
		return vector;
	}
	for (int i = 0; !buf.empty(); i++) {
		vector.push_back(buf.substr(0, buf.find(find)));
		buf.erase(0, vector[i].size() + 1);
	}
	return vector;
}

bool isMethodExisted(std::string line) {
	std::vector<std::string> method;
	method.push_back("GET");
	method.push_back("HEAD");
	method.push_back("POST");
	method.push_back("PUT");
	method.push_back("DELETE");
	method.push_back("CONNECT");
	method.push_back("OPTIONS");
	method.push_back("TRACE");
	method.push_back("PATCH");
	for (unsigned int i = 0; i < method.size(); i++)
		if (!line.compare(0, method[i].size(), method[i]))
			return true;
	return false;
}

bool isMethodAccepted(std::string line) {
	std::vector<std::string> method;
	method.push_back("GET");
	method.push_back("POST");
	method.push_back("DELETE");
	for (unsigned int i = 0; i < method.size(); i++)
		if (!line.compare(0, method[i].size(), method[i]))
			return true;
	return false;
}

std::vector<std::string> getAccept(std::string line) {
	std::vector<std::string> accept = split(line, ",");
	for (unsigned int i = 0; i < accept.size(); i++) {
		std::vector<std::string> tmp = split(accept[i], "/");
		accept[i] = tmp[1];
	}
	for (unsigned int i = 0; i < accept.size(); i++) {
		int find = accept[i].find("+");
		if (find != -1) {
			std::vector<std::string> tmp = split(accept[i], "+");
			for (unsigned int j = 1; j < tmp.size(); j++) {
				accept.push_back(tmp[j]);
			}
			accept[i].erase(find, accept[i].size());
		}
		find = accept[i].find(";");
		if (find != -1)
			accept[i].erase(find, accept[i].size());
	}
	return accept;
}

Requests readRequest(std::string buf) {
	std::vector<std::string> request = split(buf, "\n");
	std::vector<std::string> methodPathProtocol, acceptedList, tmp;
	for (unsigned int i = 0; i < request.size(); i++) {
		if (request[i].find("\r", 0) != std::string::npos)
			request[i].erase(request[i].find("\r", 0));
		if (isMethodExisted(request[i])) {
			if (!methodPathProtocol.empty())
				return Requests(BAD_REQUEST);
			methodPathProtocol = split(request[i], " ");
			if (methodPathProtocol.size() != 3)
				return Requests(BAD_REQUEST);
			if (!isMethodAccepted(methodPathProtocol[0]))
				return Requests(METHOD_NOT_ALLOWED);
			if (methodPathProtocol[2].compare("HTTP/1.1"))
				return Requests(HTTP_VERSION_NOT_SUPPORTED);
		}
		if (!request[i].compare(0, 7, "Accept:")) {
			tmp = getAccept(request[i]);
			acceptedList.insert(acceptedList.end(), tmp.begin(), tmp.end());
		}
	}
	if (methodPathProtocol.empty())
		return Requests(BAD_REQUEST);
	if (acceptedList.empty())
		return Requests(NOT_ACCEPTABLE);
	return Requests(methodPathProtocol[0], methodPathProtocol[1], acceptedList);
}

Requests::Requests(int statusCode) : _statusCode(statusCode), _method(""), _path(""), _accept(0) {}
Requests::Requests(const std::string &method, const std::string &path, std::vector<std::string> &accept) :  _statusCode(200), _method(method), _path("." + path), _accept(accept) {}
Requests::~Requests() {}

std::string getPage(std::string filename, std::string responseStatus) {
    std::ifstream fd(filename.c_str());
    std::string line;
	std::string response = responseStatus;
    while (getline(fd, line))
        response.append(line + "\n");
    fd.close();
	return response;
}

bool Requests::checkExtension() const {
	std::string extension = this->_path.substr(this->_path.find_last_of(".") + 1, this->_path.size());
	for (unsigned int i = 0; i < this->_accept.size(); i++)
		if (this->_accept[i] == "*" || extension == this->_accept[i])
			return true;
	return false;
}

void Requests::checkPage() {
	if (this->_path == "./")
		this->_path = "./index.html";
	if (!checkExtension())
		this->_statusCode = 406;
	else if (access(this->_path.c_str(), F_OK))
		this->_statusCode = 404;
	else if (access(this->_path.c_str(), R_OK))
		this->_statusCode = 403;
}

std::string Requests::getResponse() {
	chdir("./pages");
	checkPage();
	switch (this->_statusCode) {
		case OK:
			return getPage(this->_path, "HTTP/1.1 200 OK\n\n");
		case BAD_REQUEST:
			return getPage("error/400.html", "HTTP/1.1 400 Bad Request\n\n");
		case FORBIDDEN:
			return getPage("error/403.html", "HTTP/1.1 403 Forbidden\n\n");
		case NOT_FOUND:
			return getPage("error/404.html", "HTTP/1.1 404 Not Found\n\n");
		case METHOD_NOT_ALLOWED:
			return getPage("error/405.html", "HTTP/1.1 405 Method Not Allowed\n\n");
		case NOT_ACCEPTABLE:
			return getPage("error/406.html", "HTTP/1.1 406 Not Acceptable\n\n");
		case INTERNAL_SERVER_ERROR:
			return getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n");
		case HTTP_VERSION_NOT_SUPPORTED:
			return getPage("error/505.html", "HTTP/1.1 505 HTTP Version not supported\n\n");
		default:
			return getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n");
	}
}