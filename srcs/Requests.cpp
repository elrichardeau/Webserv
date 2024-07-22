#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include "../includes/Requests.hpp"

std::string itostr(int nb) {
	std::stringstream ss;
	std::string str;
	ss << nb;
	ss >> str;
	return str;
}

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
	line.erase(0, 8);
	std::vector<std::string> accept = split(line, ",");
	for (unsigned int i = 0; i < accept.size(); i++) {
		size_t find = accept[i].find(";");
		if (find != std::string::npos)
			accept[i].erase(find, accept[i].size());
	}
	for (unsigned int i = 0; i < accept.size(); i++) {
		size_t find = accept[i].find("+");
		size_t find2 = accept[i].find("/");
		if (find != std::string::npos && find2 != std::string::npos) {
			std::string tmp = accept[i].substr(0, find2 + 1);
			accept.push_back(tmp + accept[i].substr(find + 1, accept[i].size()));
			accept[i].erase(find, accept[i].size());
		}
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
	return Requests(methodPathProtocol[0], methodPathProtocol[1], methodPathProtocol[2], acceptedList);
}

// std::string getErrorPage(int error) {
// 	switch (error) {
// 		case BAD_REQUEST:
// 			return "error/400.html";
// 		case FORBIDDEN:
// 			return "error/403.html";
// 		case NOT_FOUND:
// 			return "error/404.html";
// 		case METHOD_NOT_ALLOWED:
// 			return "error/405.html";
// 		case NOT_ACCEPTABLE:
// 			return "error/406.html";
// 		case INTERNAL_SERVER_ERROR:
// 			return "error/500.html";
// 		case HTTP_VERSION_NOT_SUPPORTED:
// 			return "error/505.html";
// 		default:
// 			return "error/500.html";
// 	}
// }

Requests::Requests(int statusCode) : _statusCode(statusCode), _method(""), _path(""), _accept(0) {}
Requests::Requests(const std::string &method, const std::string &path, const std::string &protocol, std::vector<std::string> &accept) :  _statusCode(200), _method(method), _path("." + path), _protocol(protocol), _accept(accept) {}
Requests::~Requests() {}

bool Requests::checkExtension() {
	if (this->_path == "./favicon.ico") {
		this->_contentType = "image/x-icon";
		return true;
	}
	size_t find = this->_path.find_last_of(".");
	if (find != std::string::npos) {
		std::string extension = this->_path.substr(find + 1, this->_path.size());
		for (unsigned int i = 0; i < this->_accept.size(); i++) {
			size_t find = this->_accept[i].find("/");
			if (find != std::string::npos) {
				std::string type = this->_accept[i].substr(find + 1, this->_accept[i].size());
				if (extension == type) {
					this->_contentType = this->_accept[i];
					return true;
				}
			}
		}
	}
	return false;
}

void Requests::checkPage() {
	if (this->_path == "./")
		this->_path = "./index.html";
	if (access(this->_path.c_str(), F_OK))
		this->_statusCode = NOT_FOUND;
	else if (access(this->_path.c_str(), R_OK))
		this->_statusCode = FORBIDDEN;
	else if (!checkExtension())
		this->_statusCode = NOT_ACCEPTABLE;
}

std::string getPage(std::string filename, std::string responseStatus) {
    std::ifstream fd(filename.c_str());
    std::string line;
	std::string response = responseStatus;
    while (getline(fd, line))
        response.append(line + "\n");
    fd.close();
	return response;
}

std::string Requests::getResponse() {
	chdir("./pages");
	checkPage();
	switch (this->_statusCode) {
		case OK:
			return getPage(this->_path, setResponse());
		case BAD_REQUEST:
			return getPage("error/400.html", setResponse());
		case FORBIDDEN:
			return getPage("error/403.html", setResponse());
		case NOT_FOUND:
			return getPage("error/404.html", setResponse());
		case METHOD_NOT_ALLOWED:
			return getPage("error/405.html", setResponse());
		case NOT_ACCEPTABLE:
			return getPage("error/406.html", setResponse());
		case INTERNAL_SERVER_ERROR:
			return getPage("error/500.html", setResponse());
		case HTTP_VERSION_NOT_SUPPORTED:
			return getPage("error/505.html", setResponse());
		default:
			return getPage("error/500.html", setResponse());
	}
	if (this->_statusCode == OK)
		return getPage(this->_path, setResponse());
}

std::string getErrorName(int error) {
	switch (error) {
		case OK:
			return "OK";
		case BAD_REQUEST:
			return "Bad Request";
		case FORBIDDEN:
			return "Forbidden";
		case NOT_FOUND:
			return "Not Found";
		case METHOD_NOT_ALLOWED:
			return "Method Not Allowed";
		case NOT_ACCEPTABLE:
			return "Not Acceptable";
		case INTERNAL_SERVER_ERROR:
			return "Internal Server Error";
		case HTTP_VERSION_NOT_SUPPORTED:
			return "Version not supported";
		default:
			return "Internal Server Error"; 
	}
}

std::string Requests::setResponse() {
	std::string response;
	struct stat buf;
	response.append(this->_protocol + " " + itostr(this->_statusCode) + " " + getErrorName(this->_statusCode) + "\n");
	response.append("Content-Type: " + this->_contentType + "\n");
	stat(this->_path.c_str(), &buf);
	response.append("\n");
	return response;
}