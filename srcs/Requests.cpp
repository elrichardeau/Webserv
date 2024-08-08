
#include "../includes/Requests.hpp"

std::string itostr(int nb) {
	std::stringstream ss;
	std::string str;
	ss << nb;
	ss >> str;
	return str;
}

std::vector<std::string> split(std::string buf, const std::string &find) {
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

bool isSyntaxGood(std::vector<std::string> &request) {
	size_t find;
	bool body = 0;
	bool length = 0;
	bool chunked = 0;
	bool line = 0;
	if (request.empty() || request[0].empty() || split(request[0], " ").size() != 3)
		return false;
	for (size_t i = 0; i < request.size(); i++) {
		if (body == 0) {
			if (!request[i].compare(0, 15, "Content-Length:")) {
				if (length == 1 || chunked == 1)
					return false;
				length = 1;
			}
			else if (!request[i].compare(0, 26, "Transfer-Encoding: chunked")) {
				if (chunked == 1 || length == 1)
					return false;
				chunked = 1;
			}
			else if (!request[i].compare("\r")) {
				body = 1;
				continue;
			}
			find = request[i].find("\r");
			if (find == std::string::npos)
				return false;
			request[i].erase(find);
			find = request[i].find(": ");
			if (i != 0 && find == std::string::npos)
				return false;
		}
		else {
			if (length == 1) {
				find = request[i].find("\r");
				if (line == 1 || find != std::string::npos)
					return false;
				line = !line;
			}
			else if (chunked == 1) {
				find = request[i].find("\r");
				if (find == std::string::npos)
					return false;
			}
			else
				return false;
		}
	}
	return true;
}

std::vector<std::string> getAccept(std::string line) {
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

void Requests::getQuery() {
	size_t find = this->_path.find("?");
	if (find != std::string::npos) {
		this->_query = this->_path.substr(find + 1, this->_path.size());
		this->_path.erase(find, this->_path.size());

	}
}

Server Requests::findServerWithSocket(std::vector<Server> manager, int serverSocket, std::string serverName) {
	size_t find = serverName.find(":");
	if (find != std::string::npos)
		serverName.erase(find, serverName.size());
	if (serverName == "localhost" || isValidIPAddress(serverName)) {
		for (std::vector<Server>::iterator it = manager.begin(); it != manager.end(); it++)
			if (it->getServerSocket() == serverSocket)
				return *it;
	}
	else
		for (std::vector<Server>::iterator it = manager.begin(); it != manager.end(); it++)
			if (it->getServerSocket() == serverSocket && it->getServerName() == serverName)
				return *it;
	this->_paramValid = 0;
	return manager[0];
}

std::string Requests::getBody(std::vector<std::string> bufSplitted, size_t index, std::map<std::string, std::string> request) {
	size_t length = atoi(request["Content-Length"].c_str());
	if (length > this->_servParam.getClientMaxBodySize()) {
		this->_statusCode = NOT_IMPLEMENTED;
		return "";
	}
	if (length != 0) {
		if (index + 1 >= bufSplitted.size() || length != strlen(bufSplitted[index + 1].c_str())) {
			this->_statusCode = BAD_REQUEST;
			return "";
		}
		return bufSplitted[index + 1];
	}
	bool chunked = !request["Transfer-Encoding"].compare("chunked");
	if (chunked == 1) {
		std::string body;
		size_t find;
		bool line = 0;
		char *endPtr;
		size_t nbOfReturn = 0;
		for (size_t i = index + 1; i < bufSplitted.size(); i++) {
			find = bufSplitted[i].find("\r");
			if (find != std::string::npos)
				bufSplitted[i].erase(find);
			if (!bufSplitted[i].empty()) {
				nbOfReturn++;
				body.append("\n");
			}
			else if (line == 0) {
				size_t lenTmp = std::strtol(bufSplitted[i].c_str(), &endPtr, 16);
				if (lenTmp == 0)
					break;
				length += lenTmp;
				if (*endPtr != '\0') {
					this->_statusCode = BAD_REQUEST;
					return "";
				}
				line = !line;
			}
			else {
				body.append(bufSplitted[i]);
				line = !line;
			}
		}
		if (length > this->_servParam.getClientMaxBodySize()) {
			this->_statusCode = NOT_IMPLEMENTED;
			return "";
		}
		if (length - nbOfReturn != strlen(body.c_str())) {
			this->_statusCode = BAD_REQUEST;
			return "";
		}
		return body;
	}
	return "";
}

void Requests::getRootPath(const std::string &path) {
	this->_path = path;
	getQuery();
	std::vector<LocationConfig> tmp = this->_servParam.getLocations();
	std::string firstPath;
	if (path == "/")
		firstPath = path + "/";
	else
		firstPath = "/" + split(path, "/")[1] + "/";
	for (size_t i = 0; i < tmp.size(); i++) {
		if (!(tmp[i].getPath() + "/").compare(0, firstPath.size(), firstPath)) {
			this->_allowMethod = tmp[i].getAllowMethods();
			this->_index = tmp[i].getIndex();
			this->_autoIndex = tmp[i].getautoIndex();
			this->_root = tmp[i].getRoot();
			this->_uploadDir = tmp[i].getUploadDir();
			this->_redirection = tmp[i].getReturnDirective();
			this->_cgiPathPy = "";
			this->_cgiPathPhp = "";
			std::map<std::string, std::string> temp = tmp[i].getCgiPaths();
			for (std::map<std::string, std::string>::iterator it = temp.begin(); it != temp.end(); it++) {
				if (it->first == ".py")
					this->_cgiPathPy = it->second;
				else if (it->first == ".php")
					this->_cgiPathPhp = it->second;
			}
			return;
		}
	}
	this->_allowMethod.push_back("GET");
	this->_allowMethod.push_back("HEAD");
	this->_index = "";
	this->_autoIndex = false;
	this->_root = this->_servParam.getRoot();
	this->_uploadDir = "";
	this->_redirection = "";
	this->_cgiPathPy = "";
	this->_cgiPathPhp = "";
}

void Requests::setPath() {
	if (this->_path == "/favicon.ico") {
		this->_statusCode = OK;
		this->_path = "./pages/favicon.ico";
		return;
	}
	if (this->_redirection != "") {
		std::vector<std::string> redirection = split(this->_redirection, " ");
		this->_statusCode = std::atoi(redirection[0].c_str());
		if (this->_statusCode != FOUND) {
			this->_statusCode = NOT_IMPLEMENTED;
			return;
		}
		this->_path = "." + redirection[1];
		if (access(this->_path.c_str(), F_OK))
			this->_statusCode = NOT_FOUND;
		else if (access(this->_path.c_str(), R_OK))
			this->_statusCode = FORBIDDEN;
		return;
	}
	this->_path = "." + this->_root + this->_path;
	DIR *dir = opendir(this->_path.c_str());
	if (dir != NULL) {
		closedir(dir);
		if (this->_index != "") {
			if (!access((this->_path + this->_index).c_str(), F_OK | R_OK)) {
				this->_path.append(this->_index);
				this->_statusCode = OK;
				return;
			}
		}
		if (this->_autoIndex && 0) {
			// this->_statusCode = OK;
			return;
		}
		else {
			this->_statusCode = FORBIDDEN;
			return;
		}
	}
	else if (access(this->_path.c_str(), F_OK)) {
		this->_statusCode = NOT_FOUND;
		return;
	}
	else if (access(this->_path.c_str(), R_OK)) {
		this->_statusCode = FORBIDDEN;
		return;
	}
	this->_statusCode = OK;
}

bool Requests::isMethodAllowed() {
	for (size_t i = 0; i < this->_allowMethod.size(); i++) {
		if (this->_allowMethod[i] == this->_method)
			return true;
	}
	return false;
}

void Requests::setContentType() {
	bool all = false;
	size_t find = this->_path.find_last_of(".");
	if (find != std::string::npos) {
		std::string extension = this->_path.substr(find + 1, this->_path.size());
		if (extension == "php" || extension == "py") {
			this->_contentType = "text/html";
			return;
		}
		for (unsigned int i = 0; i < this->_accept.size(); i++) {
			size_t find = this->_accept[i].find("/");
			if (find != std::string::npos) {
				std::string type = this->_accept[i].substr(find + 1, this->_accept[i].size());
				if (extension == type) {
					this->_contentType = this->_accept[i];
					return;
				}
				else if (type == "*")
					all = true;
			}
		}
		if (all && extension == "ico") {
			this->_contentType = "image/x-icon";
			return;
		}
	}
	this->_statusCode = NOT_ACCEPTABLE;
}

Requests::Requests(const std::string &buf, std::vector<Server> manager, int serverSocket) {
	std::vector<std::string> bufSplitted = split(buf, "\n");
	if (!isSyntaxGood(bufSplitted)) {
		this->_paramValid = 1;
		this->_servParam = findServerWithSocket(manager, serverSocket, "localhost");
		this->_statusCode = BAD_REQUEST;
	}
	else {
		std::map<std::string, std::string> request;
		std::vector<std::string> methodPathProtocol = split(bufSplitted[0], " ");
		request.insert(std::make_pair("Method", methodPathProtocol[0]));
		request.insert(std::make_pair("Path", methodPathProtocol[1]));
		request.insert(std::make_pair("Protocol", methodPathProtocol[2]));
		for (size_t i = 1; i < bufSplitted.size(); i++) {
			if (!bufSplitted[i].compare("\r")) {
				if (i + 1 != bufSplitted.size())
					request.insert(std::make_pair("Body", getBody(bufSplitted, i, request)));
				break;
			}
			request.insert(std::make_pair(bufSplitted[i].substr(0, bufSplitted[i].find(": ")), bufSplitted[i].substr(bufSplitted[i].find(": ") + 2, bufSplitted[i].size())));
		}
		this->_paramValid = 1;
		this->_servParam = findServerWithSocket(manager, serverSocket, request["Host"]);
		if (this->_paramValid) {
			this->_protocol = request["Protocol"];
			if (this->_protocol != "HTTP/1.1")
				this->_statusCode = HTTP_VERSION_NOT_SUPPORTED;
			else {
				getRootPath(request["Path"]);
				this->_method = request["Method"];
				if (!isMethodAllowed())
					this->_statusCode = METHOD_NOT_ALLOWED;
				else {
					setPath();
					if (this->_statusCode == OK || this->_statusCode == FOUND) {
						this->_accept = getAccept(request["Accept"]);
						setContentType();
						this->_body = request["Body"];
					}
				}
			}
		}
	}
}

Requests::~Requests() {}

std::string getPage(std::string filename, std::string responseStatus) {
	std::ifstream fd(filename.c_str());
	if (!fd.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    }
    std::string line;
	std::string response = responseStatus;
    while (getline(fd, line))
        response.append(line + "\n");
    fd.close();
	return response;
}

std::string Requests::readFromPipe(int pipeFd) {
	char buffer[4096];
	ssize_t bytesRead = read(pipeFd, buffer, 4096 - 1);
	if (bytesRead == -1) {
		this->_statusCode = INTERNAL_SERVER_ERROR;
		return "";
	}
	std::string result;
	while (bytesRead > 0) {
		buffer[bytesRead] = '\0';
		result += buffer;
		bytesRead = read(pipeFd, buffer, 4096 - 1);
		if (bytesRead == -1) {
			this->_statusCode = INTERNAL_SERVER_ERROR;
			return "";
		}
	}
	return result;
}

char **Requests::vectorToCharArray(const std::vector<std::string> &vector) {
	char** arr = new char*[vector.size() + 1];
	for (size_t i = 0; i < vector.size(); i++) {
		arr[i] = new char[vector[i].size() + 1];
		std::copy(vector[i].begin(), vector[i].end(), arr[i]);
		arr[i][vector[i].size()] = '\0';
	}
	arr[vector.size()] = NULL;
	return arr;
}

static void exportVar(std::vector<std::string> &env, const std::string&key, const std::string& value) {env.push_back(key + '=' + value);}

std::vector<std::string> Requests::createCgiEnv() {
	std::vector<std::string> env;
	exportVar(env, "REQUEST_METHOD", this->_method);
	exportVar(env, "SCRIPT_NAME", "response.php");
	exportVar(env, "SERVER_PROTOCOL", "HTTP/1.1");
	exportVar(env, "SERVER_SOFTWARE", "webserv/1.1");
	// exportVar(env, "DOCUMENT_ROOT", "/var/www/html");
	if (!this->_method.compare("GET"))
		exportVar(env, "QUERY_STRING", this->_query);
	else if (!this->_method.compare("POST")) {
		exportVar(env, "CONTENT_LENGTH", itostr(this->_body.size()));
		exportVar(env, "CONTENT_TYPE", "x-www-form-urlencoded");
	}
	return env;
}

std::string  Requests::execCgi(const std::string& scriptType) {
	int childPid;
	int childValue;
	int fd[2];
	int fdBody[2];
	const char *scriptInterpreter;
	if (pipe(fd) == -1)
		return this->_statusCode = 500, setErrorPage();
	childPid = fork();
	if (childPid == -1)
		return this->_statusCode = 500, setErrorPage();
	//si POST, on crée un | pour permettre l'écriture et la lecture du body
	if (!this->_method.compare("POST")) {
		if (pipe(fdBody) == -1)
			return this->_statusCode = 500, setErrorPage();
		if (write(fdBody[1], this->_body.c_str(), static_cast<int>(this->_body.size())) == -1)
			return this->_statusCode = 500, setErrorPage();
	}
	if (!childPid) {
		if (!this->_method.compare("POST")) {
			std::cout << "CONTENT TYPE  " << this->_contentType << std::endl;
			close(fdBody[1]);
			if (dup2(fdBody[0], STDIN_FILENO))
				exit(EXIT_FAILURE);
			close(fdBody[0]);
		}
		close(fd[0]);
		if (dup2(fd[1], STDOUT_FILENO) == -1) 
			exit(EXIT_FAILURE);
		close(fd[1]);
		if (!scriptType.compare("py"))
			scriptInterpreter = this->_cgiPathPy.c_str();
		else
			scriptInterpreter = this->_cgiPathPhp.c_str();
		char **env = vectorToCharArray(createCgiEnv());
		char *args[] = { const_cast<char*>(scriptInterpreter), const_cast<char*>(_path.c_str()), NULL };
		execve(scriptInterpreter, args, env);
		delete [] env;
		exit(EXIT_FAILURE);
	}
	if (waitpid(childPid, &childValue, WUNTRACED) == -1)
			return close(fd[0]), close(fd[1]), this->_statusCode = 500, setErrorPage();
	if (WEXITSTATUS(childValue) == 1)
		return close(fd[0]), close(fd[1]), this->_statusCode = 500, setErrorPage();
	if (!this->_method.compare("POST")) {
		close(fdBody[0]);
		close(fdBody[1]);
	}
	close(fd[1]); 
    std::string scriptContent = readFromPipe(fd[0]);
	close(fd[0]);
	std::string line;
	if (this->_statusCode != OK)
		return setErrorPage();
	std::string response = "HTTP/1.1 200 OK\n\n";
        response.append(scriptContent + "\n");
	return setResponseScript(scriptContent, "OK") + scriptContent;
	return response;
}

std::string Requests::getResponse() {
	if (!this->_paramValid)
		return "";
	if (this->_statusCode == OK || this->_statusCode == FOUND) {
		std::vector<std::string> words = split(this->_path, ".");
		if (words.size() == 1) {
			this->_statusCode = BAD_REQUEST;
			return setErrorPage();
		}
		if (words[words.size() - 1] == "py" || words[words.size() - 1] == "php") {
			if (access((this->_path).c_str(), X_OK)) {
				this->_statusCode = FORBIDDEN;
				return setErrorPage();
			}
			return execCgi(words[words.size() - 1]);
		}
		if (this->_statusCode == FOUND)
			return getPage(this->_path, setResponse("Found"));
		return getPage(this->_path, setResponse("OK"));
	}
	return setErrorPage();
}

std::string Requests::setErrorPage() {
	ErrorPage err = this->_servParam.getErrorPage();
	this->_protocol = err.getProtocol();
	this->_contentType = err.getContentType();
	this->_path = err.getPath(this->_statusCode);
	return getPage(this->_path, setResponse(err.getErrorMessage(this->_statusCode)));
}

std::string Requests::setResponse(const std::string &codeName) {
	std::string response;
	struct stat buf;
	int st = stat(this->_path.c_str(), &buf);
	response.append(this->_protocol + " " + itostr(this->_statusCode) + " " + codeName + "\n");
	response.append("Connection: Keep-Alive\n");
	if (st != -1) {
		size_t bytes = buf.st_size;
		response.append("Content-Length: ");
		response.append(itostr(bytes));
		response.append("\n");
	}
	response.append("Content-Location: " + this->_path.substr(1, this->_path.size()) + "\n");
	response.append("Content-Type: " + this->_contentType + "\n");
	if (st != -1) {
		response.append("Date: ");
		response.append(ctime(&buf.st_atime));
		response.append("Last-Modified: ");
		response.append(ctime(&buf.st_mtime));
	}
	response.append("Server: Webserv\n");
	response.append("\n");
	return response;
}

std::string Requests::setResponseScript(const std::string &scriptResult, const std::string &codeName) {
	std::string response;
	response.append(this->_protocol + " " + itostr(this->_statusCode) + " " + codeName + "\n");
	response.append("Connection: Keep-Alive\n");
	response.append("Content-Length: ");
	response.append(itostr(strlen(scriptResult.c_str())));
	response.append("\n");
	response.append("Content-Location: " + this->_path.substr(1, this->_path.size()) + "\n");
	response.append("Content-Type: " + this->_contentType + "\n");
	time_t actualTime = time(NULL);
	response.append("Date: ");
	response.append(ctime(&actualTime));
	response.append("Last-Modified: ");
	response.append(ctime(&actualTime));
	response.append("Server: Webserv\n");
	response.append("\n");
	return response;
}