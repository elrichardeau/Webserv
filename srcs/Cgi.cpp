
#include "../includes/Cgi.hpp"

Cgi::Cgi(const std::string &method, const std::string &query, const std::string &body, const std::string &scriptInterpreter, const std::string &path, const std::string &uploadDir) :
	_method(method), _query(query), _body(body), _scriptInterpreter(scriptInterpreter), _path(path), _uploadDir(uploadDir) {}

Cgi::~Cgi() {}

std::string Cgi::readFromPipe(int pipeFd) {
	char buffer[4096];
	ssize_t bytesRead = read(pipeFd, buffer, 4096 - 1);
	if (bytesRead == -1)
		throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
	std::string result;
	time_t t = time(NULL);
	while (bytesRead > 0 && t + 2 > time(NULL)) {
		buffer[bytesRead] = '\0';
		result += buffer;
		bytesRead = read(pipeFd, buffer, 4096 - 1);
		if (bytesRead == -1)
			throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
	}
	return result;
}

char **Cgi::vectorToCharArray(const std::vector<std::string> &vector) {
	char** arr = new char*[vector.size() + 1];
	for (size_t i = 0; i < vector.size(); i++) {
		arr[i] = new char[vector[i].size() + 1];
		std::copy(vector[i].begin(), vector[i].end(), arr[i]);
		arr[i][vector[i].size()] = '\0';
	}
	arr[vector.size()] = NULL;
	return arr;
}

std::vector<std::string> Cgi::createCgiEnv() {
	std::vector<std::string> env;
	env.push_back("REQUEST_METHOD=" + this->_method);
	env.push_back("SCRIPT_NAME=response.php");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=webserv/1.1");
	env.push_back("UPLOAD_DIR=" + this->_uploadDir);
	if (this->_method == "GET")
		env.push_back("QUERY_STRING=" + this->_query);
	else if (this->_method == "POST") {
		env.push_back("CONTENT_LENGTH=" + itostr(this->_body.size()));
		env.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
	}
	return env;
}

std::string  Cgi::execCgi() {
	int childPid;
	int childValue;
	int fd[2];
	int fdBody[2];

	if (this->_scriptInterpreter == "")
		throw ErrorCode(itostr(NOT_IMPLEMENTED));
	if (pipe(fd) == -1)
		throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
	childPid = fork();
	if (childPid == -1)
		throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
	if (this->_method == "POST") {
		if (pipe(fdBody) == -1)
			throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
		if (write(fdBody[1], this->_body.c_str(), static_cast<int>(this->_body.size())) == -1)
			throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
	}
	if (!childPid) {
		if (this->_method == "POST") {
			close(fdBody[1]);
			if (dup2(fdBody[0], STDIN_FILENO))
				exit(EXIT_FAILURE);
			close(fdBody[0]);
		}
		close(fd[0]);
		if (dup2(fd[1], STDOUT_FILENO) == -1) 
			exit(EXIT_FAILURE);
		close(fd[1]);
		char **env = vectorToCharArray(createCgiEnv());
		char *args[] = { const_cast<char*>(this->_scriptInterpreter.c_str()), const_cast<char*>(this->_path.c_str()), NULL };
		execve(this->_scriptInterpreter.c_str(), args, env);
		delete [] env;
		exit(EXIT_FAILURE);
	}
	int i = 0;
	while (true) {
		pid_t status = waitpid(childPid, &childValue, WNOHANG);
		if (status == 0) {
			usleep(100000);
			if (i++ > 2)
				kill(childPid, SIGKILL);
		}
		else if (status == -1) {
			close(fd[0]);
			close(fd[1]);
			if (this->_method == "POST") {
				close(fdBody[0]);
				close(fdBody[1]);
			}
			throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
		}
		else
			break;
	}
	if (WIFEXITED(childValue) == 1) {
		if (WEXITSTATUS(childValue) == 1) {
			close(fd[0]);
			close(fd[1]);
			if (this->_method == "POST") {
				close(fdBody[0]);
				close(fdBody[1]);
			}
			throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
		}
	}
	if (this->_method == "POST") {
		close(fdBody[0]);
		close(fdBody[1]);
	}
	close(fd[1]); 
    std::string scriptContent = readFromPipe(fd[0]);
	close(fd[0]);
	if (scriptContent.size() > 10000)
		throw ErrorCode(itostr(REQUEST_ENTITY_TOO_LARGE));
	return scriptContent;
}