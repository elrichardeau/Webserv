/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niromano <niromano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 11:56:05 by niromano          #+#    #+#             */
/*   Updated: 2024/07/19 17:18:56 by niromano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include "../includes/Requests.hpp"

std::vector<std::string> split(std::string buf, std::string find) {
	std::vector<std::string> vector;
	for (int i = 0; !buf.empty(); i++) {
		vector.push_back(buf.substr(0, buf.find(find)));
		buf.erase(0, vector[i].size() + 1);
	}
	return vector;
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
	std::vector<std::string> methodPathProtocol, extensionAcceptedList, tmp;
	for (unsigned int i = 0; i < request.size(); i++) {
		if (request[i].find("\r", 0) != std::string::npos)
			request[i].erase(request[i].find("\r", 0));
		if (isMethodAccepted(request[i])) {
			methodPathProtocol = split(request[i], " ");
			if (methodPathProtocol.size() != 3)
				return Requests(400);
			if (!isMethodAccepted(methodPathProtocol[0]))
				return Requests(405);
			if (methodPathProtocol[2].compare("HTTP/1.1"))
				return Requests(505);
		}
		if (!request[i].compare(0, 7, "Accept:")) {
			tmp = getAccept(request[i]);
			extensionAcceptedList.insert(extensionAcceptedList.end(), tmp.begin(), tmp.end());
		}
	}
	if (methodPathProtocol.empty())
		return Requests(400);
	if (extensionAcceptedList.empty())
		return Requests(406);
	return Requests(methodPathProtocol[0], methodPathProtocol[1], extensionAcceptedList);
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

std::string Requests::getResponse() {
	chdir("./pages");
	switch (this->_statusCode) {
		case OK:
			if (this->_path == "./")
				return getPage("./default.html", "HTTP/1.1 200 OK\n\n");
			return getPage(this->_path, "HTTP/1.1 200 OK\n\n");
		case BAD_REQUEST:
			return getPage("error/400.html", "HTTP/1.1 400 Not Found\n\n");
		case FORBIDDEN:
			return getPage("error/403.html", "HTTP/1.1 403 Not Found\n\n");
		case NOT_FOUND:
			return getPage("error/404.html", "HTTP/1.1 404 Not Found\n\n");
		case METHOD_NOT_ALLOWED:
			return getPage("error/405.html", "HTTP/1.1 405 Not Found\n\n");
		case NOT_ACCEPTABLE:
			return getPage("error/406.html", "HTTP/1.1 406 Not Found\n\n");
		case INTERNAL_SERVER_ERROR:
			return getPage("error/500.html", "HTTP/1.1 500 Not Found\n\n");
		case HTTP_VERSION_NOT_SUPPORTED:
			return getPage("error/505.html", "HTTP/1.1 505 Not Found\n\n");
		default:
			return getPage("error/500.html", "HTTP/1.1 500 Not Found\n\n");
	}
}