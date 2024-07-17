/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niromano <niromano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 11:56:05 by niromano          #+#    #+#             */
/*   Updated: 2024/07/17 17:26:07 by niromano         ###   ########.fr       */
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

bool isMethodValid(std::string line) {
	std::vector<std::string> method;
	method.push_back("GET");
	method.push_back("POST");
	method.push_back("DELETE");
	for (unsigned int i = 0; i < method.size(); i++)
		if (!line.compare(0, method[i].size(), method[i]))
			return true;
	return false;
}

std::vector<std::string> readMethod(std::string line) {
	std::vector<std::string> method = split(line, " ");
	for (unsigned int i = 0; i < method.size(); i++)
		std::cout << method[i] << std::endl;
	return method;
}

void readTest(std::string buf) {
	std::cout << buf << std::endl;
	std::vector<std::string> request = split(buf, "\n");
	for (unsigned int i = 0; i < request.size(); i++)
		if (isMethodValid(request[i]))
			std::vector<std::string> method = readMethod(request[i]);		
}

Requests readRequest(std::string buf) {
	readTest(buf);
	std::string firstLine = buf.substr(0, buf.find("\n"));
	const std::string method = firstLine.substr(0, firstLine.find(" "));
	firstLine.erase(0, method.size() + 1);
	const std::string path = "." + firstLine.substr(0, firstLine.find(" "));
	firstLine.erase(0, path.size() + 1);
	const std::string protocol = firstLine;
	std::vector<std::string> accept;
	Requests req(method, path, protocol, accept);
	return req;
}

Requests::Requests(const std::string &method, const std::string &path, const std::string &protocol, std::vector<std::string> &accept) : 
	_method(method), _path(path), _protocol(protocol), _accept(accept) {}
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
	// for (unsigned int i = 0; i < this->_accept.size(); i++) {
	// 	std::cout << this->_accept[i] << std::endl;
	// }
	if (access((this->_path).c_str(), F_OK))
		return getPage("error/404.html", "HTTP/1.1 404 Not Found\n\n");
	if (access((this->_path).c_str(), R_OK))
		return getPage("error/403.html", "HTTP/1.1 403 Forbidden\n\n");
	if (this->_path == "./")
		return getPage("default.html", "HTTP/1.1 200 OK\n\n");
	return getPage(this->_path, "HTTP/1.1 200 OK\n\n");
}