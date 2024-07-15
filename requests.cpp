/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niromano <niromano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 11:56:05 by niromano          #+#    #+#             */
/*   Updated: 2024/07/15 18:04:06 by niromano         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include "requests.hpp"

Requests readRequest(std::string buf) {
	std::string firstLine = buf.substr(0, buf.find("\n"));
	const std::string type = firstLine.substr(0, firstLine.find(" "));
	firstLine.erase(0, type.size() + 1);
	const std::string path = "." + firstLine.substr(0, firstLine.find(" "));
	firstLine.erase(0, path.size() + 1);
	const std::string method = firstLine;
	Requests req(type, path, method);
	return req;
}

Requests::Requests(const std::string &type, const std::string &path, const std::string &method) : _type(type), _path(path), _method(method) {}
Requests::~Requests() {}
std::string Requests::getType() {return this->_type;}
std::string Requests::getPath() {return this->_path;}
std::string Requests::getMethod() {return this->_method;}

std::string getPage(std::string filename, std::string responseStatus) {
    std::ifstream fd(filename.c_str());
    std::string line;
	std::string response = responseStatus;
    while (getline(fd, line))
        response.append(line + "\n");
    fd.close();
	return response;
}

bool Requests::isSyntaxError() {
	if (0)
		return true;
	return false;
}

std::string Requests::getResponse() {
	chdir("./pages");
	if (isSyntaxError())
		return getPage("error/400.html", "HTTP/1.1 400 Bad Request\n\n");
	if (!access((this->_path).c_str(), F_OK))
		return getPage("error/404.html", "HTTP/1.1 404 Not Found\n\n");
	if (!access((this->_path).c_str(), R_OK))
		return getPage("error/403.html", "HTTP/1.1 403 Forbidden\n\n");
	if (this->_path == "./")
		return getPage("default.html", "HTTP/1.1 200 OK\n\n");
	return getPage(this->_path, "HTTP/1.1 200 OK\n\n");
}