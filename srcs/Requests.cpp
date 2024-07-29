#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
 #include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
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
	std::string method[9] = {"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH"};
	for (unsigned int i = 0; i < 9; i++)
		if (!line.compare(0, strlen(method[i].c_str()), method[i].c_str()))
			return true;
	return false;
}

bool isMethodAccepted(std::string line) {
	std::string method[3] = {"GET", "POST", "DELETE"};
	for (unsigned int i = 0; i < 3; i++)
		if (!line.compare(0, strlen(method[i].c_str()), method[i].c_str()))
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

bool checkRequestSyntax(std::vector<std::string> request) {
	size_t find;
	for (unsigned int i = 0; i < request.size(); i++) {
		find = request[i].find("\r");
		if (find == std::string::npos)
			return false;
	}
	std::vector<std::string> firstLine = split(request[0], " ");
	if (firstLine.size() != 3)
		return false;
	for (unsigned int i = 1; i < request.size() - 1; i++) {
		std::vector<std::string> otherLine = split(request[i], ":");
		if (otherLine.size() < 2)
			return false;
	}
	return true;
}

Requests readRequest(std::string buf) {
	std::vector<std::string> request = split(buf, "\n");
	if (!checkRequestSyntax(request))
		return Requests(BAD_REQUEST);
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

Requests::Requests(int statusCode) : _statusCode(statusCode), _method(""), _path(""), _accept(0) {}

Requests::Requests(const std::string &method, const std::string &path, const std::string &protocol, std::vector<std::string> &accept) :  _statusCode(200), _method(method), _path("." + path), _protocol(protocol), _accept(accept) {
	this->_query = "";
	size_t find = this->_path.find("?");
	if (find != std::string::npos) {
		this->_query = this->_path.substr(find + 1, this->_path.size());
		this->_path.erase(find, this->_path.size());
	}
}

Requests::~Requests() {}

bool Requests::checkExtension() {
	if (this->_path == "./favicon.ico") {
		this->_contentType = "image/x-icon";
		return true;
	}
	size_t find = this->_path.find_last_of(".");
	if (find != std::string::npos) {
		std::string extension = this->_path.substr(find + 1, this->_path.size());
		if (extension == "php" || extension == "py") {
			this->_contentType = "text/html";
			return true;
		}
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
	DIR *dir = opendir(this->_path.c_str());
	if (dir != NULL) {
		closedir(dir);
		std::string slash = "";
		if (this->_path[this->_path.size() - 1] != '/')
			slash = "/";
		this->_path = this->_path + slash + "index.html";
	}
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

std::string readFromPipe(int pipeFd)
{
	char buffer[4096];
	std::string result;
	ssize_t bytesRead = read(pipeFd, buffer, 4096 - 1);
	if (bytesRead == -1)
			return ("HTTP/1.1 500 Internal Server Error");

	while (bytesRead  > 0)
	{
		buffer[bytesRead] = '\0';
		result += buffer;
		bytesRead = read(pipeFd, buffer, 4096 - 1);
		if (bytesRead == -1)
			return ("HTTP/1.1 500 Internal Server Error");
	}
	return (result);
}


char **Requests::vectorToCharArray(const std::vector<std::string> &vector)
{
	char** arr = new char*[vector.size() + 1];
	
	for (size_t i = 0; i < vector.size(); i++)
	{
		arr[i] = new char[vector[i].size() + 1];
		std::copy(vector[i].begin(), vector[i].end(), arr[i]);
		arr[i][vector[i].size()] = '\0';
	}
	arr[vector.size()] = NULL;
	return (arr);
}

static void exportVar(std::vector<std::string> &env, const std::string&key, const std::string& value)
{
	env.push_back(key + '=' + value);
}


std::vector<std::string> Requests::createCgiEnv()
{
	std::vector<std::string> env;

	exportVar(env, "REQUEST_METHOD", this->_method);
	exportVar(env, "SCRIPT_NAME", "response.php");
	exportVar(env, "SERVER_PROTOCOL", "HTTP/1.1");
	exportVar(env, "SERVER_SOFTWARE", "webserv/1.1");
	// exportVar(env, "DOCUMENT_ROOT", "/var/www/html");

	if (!this->_method.compare("GET"))
		exportVar(env, "QUERY_STRING", this->_query);

	else if (!this->_method.compare("POST"))
	{
		exportVar(env, "CONTENT_LENGTH", "");
		exportVar(env, "CONTENT_TYPE", "");
	}
	return (env);
}

std::string  Requests::execCgi(const std::string& scriptType)
{
	int childPid;
	int childValue;
	int fd[2];
	int fdBody[2];
	const char *scriptInterpreter;

	if (pipe(fd) == -1)
		return (getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n"));
	childPid = fork();
	if (childPid == -1)
		return (getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n"));
	
	//si POST, on crée un | pour permettre l'écriture et la lecture du body
	if (!this->_method.compare("POST"))
	{
		std::string _body = "name=John+Doe&email=john.doe%40example.com&age=30";

		if (pipe(fdBody) == -1)
			return (getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n"));
		if (write(fdBody[1], _body.c_str(), _body.size()))
			return (getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n"));
	}
	
	if (!childPid)
	{
		if (!this->_method.compare("POST"))
		{
			close(fdBody[1]);
			if (dup2(fd[0], STDIN_FILENO))
				exit(EXIT_FAILURE);
		}

		close(fd[0]);
		if (dup2(fd[1], STDOUT_FILENO) == -1) 
		{
			perror("PIPE ERROR ");
			exit(EXIT_FAILURE);
		}
		close(fd[1]);
		
		if (!scriptType.compare("py"))
			scriptInterpreter = "/usr/bin/python3";
		else
			scriptInterpreter = "/usr/bin/php";
			
		char **env = vectorToCharArray(createCgiEnv());
		for (int i = 0; i < 5; i++)
			std::cerr << env[i] << std::endl;
		char *args[] = { const_cast<char*>(scriptInterpreter), const_cast<char*>(_path.c_str()), NULL };

		execve(scriptInterpreter, args, env);
		delete [] env;
		exit(EXIT_FAILURE);
	}

	if (waitpid(childPid, &childValue, WUNTRACED) == -1)
			return (close(fd[0]), close(fd[1]), getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n"));
	if (WEXITSTATUS(childValue) == 1)
		return (close(fd[0]), close(fd[1]), getPage("error/500.html", "HTTP/1.1 500 Internal Server Error\n\n"));

	if (!this->_method.compare("POST"))
	{
		close(fdBody[0]);
		close(fdBody[1]);
	}

	close(fd[1]); 
    std::string scriptContent = readFromPipe(fd[0]);
	close(fd[0]);
	std::string line;
	std::string response = "HTTP/1.1 200 OK\n\n";
        response.append(scriptContent + "\n");
	return response;
}


// std::string Requests::getResponse() {
// 	chdir("./pages");
// 	if (isSyntaxError())
// 		return getPage("error/400.html", "HTTP/1.1 400 Bad Request\n\n");
// 	if (access((this->_path).c_str(), F_OK))
// 		return getPage("error/404.html", "HTTP/1.1 404 Not Found\n\n");
// 	if (access((this->_path).c_str(), R_OK))
// 		return getPage("error/403.html", "HTTP/1.1 403 Forbidden\n\n");
// 	if (this->_path == "./")
// 		return getPage("default.html", "HTTP/1.1 200 OK\n\n");
	
// 	//partie cgi elodie
// 	std::vector<std::string> words = Requests::split(this->_path, ".");
// 	int nb_words = words.size();
// 	if (nb_words == 1)
// 		return getPage("error/400.html", "HTTP/1.1 400 Bad Request");
// 	if (words[nb_words - 1] == "py" || words[nb_words - 1] == "php")
// 	{
// 		if (access((this->_path).c_str(), X_OK))
// 			return getPage("error/403.html", "HTTP/1.1 403 Forbidden");
// 		return (execCgi(words[nb_words - 1]));
// 	}
// 	return getPage(this->_path, "HTTP/1.1 200 OK");
// }

std::string Requests::getResponse() {
	chdir("./pages");
	if (this->_statusCode == OK)
		checkPage();
	if (this->_statusCode == OK) {
		std::vector<std::string> words = split(this->_path, ".");
		int nb_words = words.size();
		if (nb_words == 1)
			return getPage("error/400.html", "HTTP/1.1 400 Bad Request");
		if (words[nb_words - 1] == "py" || words[nb_words - 1] == "php")
		{
			if (access((this->_path).c_str(), X_OK))
				return getPage("error/403.html", "HTTP/1.1 403 Forbidden");
			return (execCgi(words[nb_words - 1]));
		}
		return getPage(this->_path, setResponse());
	}
	return getErrorPage();
}

std::string Requests::getErrorPage() {
	this->_protocol = "HTTP/1.1";
	this->_contentType = "text/html";
	switch (this->_statusCode) {
		case BAD_REQUEST:
			this->_path = "./error/400.html";
			break;
		case FORBIDDEN:
			this->_path = "./error/403.html";
			break;
		case NOT_FOUND:
			this->_path = "./error/404.html";
			break;
		case METHOD_NOT_ALLOWED:
			this->_path = "./error/405.html";
			break;
		case NOT_ACCEPTABLE:
			this->_path = "./error/406.html";
			break;
		case HTTP_VERSION_NOT_SUPPORTED:
			this->_path = "./error/505.html";
			break;
		case INTERNAL_SERVER_ERROR:
			this->_path = "./error/500.html";
			break;
	}
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
	int st = stat(this->_path.c_str(), &buf);
	response.append(this->_protocol + " " + itostr(this->_statusCode) + " " + getErrorName(this->_statusCode) + "\n");
	response.append("Connection: Keep-Alive\n");
	if (st != 1) {
		size_t bytes = buf.st_size;
		response.append("Content-Length: ");
		response.append(itostr(bytes));
		response.append("\n");
	}
	response.append("Content-Location: " + this->_path.substr(1, this->_path.size()) + "\n");
	response.append("Content-Type: " + this->_contentType + "\n");
	if (st != 1) {
		response.append("Date: ");
		response.append(ctime(&buf.st_atime));
		response.append("Last-Modified: ");
		response.append(ctime(&buf.st_mtime));
	}
	response.append("Server: Webserv\n");
	response.append("\n");
	return response;

}