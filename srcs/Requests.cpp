/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elrichar <elrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 11:56:05 by niromano          #+#    #+#             */
/*   Updated: 2024/07/18 17:56:50 by elrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include "../includes/Requests.hpp"

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


std::vector<std::string> Requests::split(std::string buf, const std::string& find) 
{
    std::vector<std::string> vector;
    for (int i = 0; !buf.empty(); i++) {
        vector.push_back(buf.substr(0, buf.find(find)));
        buf.erase(0, vector[i].size() + 1);
    }
    return vector;
}

std::string readFromPipe(int pipeFd)
{
	char buffer[4096];
	std::string result;
	ssize_t bytesRead = read(pipeFd, buffer, 4096 - 1);
	if (bytesRead == -1)
			return ("HTTP/1.1 500 Internal Server Error");

	while (bytesRead  > 0)
	{
		if (bytesRead == -1)
			return ("HTTP/1.1 500 Internal Server Error");
		buffer[bytesRead] = '\0';
		result += buffer;
		bytesRead = read(pipeFd, buffer, 4096 - 1);
	}
	return (result);
}


// char** 		 Requests::createEnv()
// {
// 	parcourir la classe contenant toutes les vars dont j'ai besoin et creer un char **
// 
// }

std::string  Requests::execCgi(const std::string& scriptType)
{
	int childPid;
	int fd[2];

	if (pipe(fd) == -1)
		return (getPage("error/500.html", "HTTP/1.1 500 Internal Server Error"));
	childPid = fork();
	if (childPid == -1)
		return (getPage("error/500.html", "HTTP/1.1 500 Internal Server Error"));
	
	//if (la requete est de type POST)
	//je cree un pipe dans lequel j'ecris avec write le body dont a besoin mon script CGI
	//

	if (!childPid)
	{
		if (scriptType == "py")
			char *scriptInterpreter = (this->_scriptInterpreterPy).c_str();
		else
			char *scriptInterpreter = (this->_scriptInterpreterPhp).c_str();
		
		//char **env = createEnv();
		char **env = NULL;
		char *args[] = { const_cast<char*>(scriptInterpreter), const_cast<char*>(_path.c_str()), NULL };

		
		close(fd[0]);
		if (dup2(fd[1], STDOUT_FILENO))
			return (delete [] env, getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n"));
		close(fd[1]);
		execve(scriptInterpreter, args, env);
		return (delete [] env, getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n"));
	}
	close(fd[1]);
    std::string scriptContent = readFromPipe(fd[0]);
	if (!scriptContent.compare("HTTP/1.1 500 Internal Server Error"))
		return (close(fd[0]), getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n"));
	close(fd[0]);
	return getPage(scriptContent, "HTTP/1.1 200 OK\n\n");
}


std::string Requests::getResponse() {
	chdir("./pages");
	if (isSyntaxError())
		return getPage("error/400.html", "HTTP/1.1 400 Bad Request\n\n");
	if (access((this->_path).c_str(), F_OK))
		return getPage("error/404.html", "HTTP/1.1 404 Not Found\n\n");
	if (access((this->_path).c_str(), R_OK))
		return getPage("error/403.html", "HTTP/1.1 403 Forbidden\n\n");
	if (this->_path == "./")
		return getPage("default.html", "HTTP/1.1 200 OK\n\n");
	
	//partie cgi elodie
	std::vector<std::string> words = Requests::split(this->_path, ".");
	int nb_words = words.size();
	if (nb_words == 1)
		return getPage("error/400.html", "HTTP/1.1 400 Bad Request\n\n");
	if (words[nb_words - 1] == "py" || words[nb_words - 1] == "php")
	{
		if (access((this->_path).c_str(), X_OK))
			return getPage("error/403.html", "HTTP/1.1 403 Forbidden\n\n");
		return (execCgi(words[nb_words - 1]));
	}
	return getPage(this->_path, "HTTP/1.1 200 OK\n\n");
}