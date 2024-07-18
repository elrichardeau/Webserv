/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: niromano <niromano@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 11:56:05 by niromano          #+#    #+#             */
/*   Updated: 2024/07/18 17:52:44 by niromano         ###   ########.fr       */
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

// bool isMethodAccepted(std::string line) {
// 	std::vector<std::string> method;
// 	method.push_back("GET");
// 	method.push_back("POST");
// 	method.push_back("DELETE");
// 	for (unsigned int i = 0; i < method.size(); i++)
// 		if (!line.compare(0, method[i].size(), method[i]))
// 			return true;
// 	return false;
// }

std::vector<std::string> getAccept(std::string line) {
	line.erase(0, 8);
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
	std::vector<std::string> methodPathProtocol;
	int mPPGet = 0;
	std::vector<std::string> extensionAcceptedList;
	int eALGet = 0;
	for (unsigned int i = 0; i < request.size(); i++) {
		if (isMethodExisted(request[i])) {
			methodPathProtocol = split(request[i], " ");
			mPPGet++;
		}
		if (!request[i].compare(0, 7, "Accept:")) {
			extensionAcceptedList = getAccept(request[i]);
			eALGet++;
		}
	}
	if (mPPGet != 1)
		methodPathProtocol.clear();
	if (eALGet != 1)
		extensionAcceptedList.clear();
	Requests requests(methodPathProtocol[0], methodPathProtocol[1], methodPathProtocol[2], extensionAcceptedList);
	return requests;
}

std::string Requests::getStatusCode() {return this->_statusCode;}

Requests::Requests(std::string &statusCode) :
	_statusCode(statusCode), _method(""), _path(""), _protocol(""), _extensionAcceptedList(0) {}

Requests::Requests(const std::string &method, const std::string &path, const std::string &protocol, std::vector<std::string> &accept) : 
	_statusCode("200"), _method(method), _path("." + path), _protocol(protocol), _extensionAcceptedList(accept) {}

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
	if (this->_statusCode == "200") {
		if (this->_path == "./")
			return getPage("./default.html", this->_protocol + " " + this->_statusCode + " OK" + "\n\n");
		return getPage(this->_path, this->_protocol + " " + this->_statusCode + " OK" + "\n\n");
	}
	return getPage("error/404.html", "HTTP/1.1 404 Not Found\n\n");
}