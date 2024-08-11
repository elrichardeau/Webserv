
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
	if (request.empty() || request[0].empty() || split(request[0], " ").size() != 3)
		return false;
	for (size_t i = 0; i < request.size(); i++) {
		if (request[i] == "\r")
			break;
		find = request[i].find("\r");
		if (find == std::string::npos)
			return false;
		request[i].erase(find);
		find = request[i].find(": ");
		if (i != 0 && find == std::string::npos)
			return false;
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

void Requests::getRootPath(const std::string &path) {
	this->_urlPath = path;
	if (this->_urlPath.find_last_of("/") != this->_urlPath.size() - 1)
		this->_urlPath.append("/");
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
			this->_uploadDir = "." + this->_root + firstPath + tmp[i].getUploadDir();
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
			this->_autoIndexFile = "";
			return;
		}
	}
	this->_allowMethod.push_back("GET");
	this->_allowMethod.push_back("HEAD");
	this->_autoIndex = false;
	this->_root = this->_servParam.getRoot();
	this->_uploadDir = "";
	this->_redirection = "";
	this->_cgiPathPy = "";
	this->_cgiPathPhp = "";
	this->_autoIndexFile = "";
}

void Requests::createAutoIndexFile() {
	std::vector<std::string> response;
	DIR *dir = opendir(this->_path.c_str());
	if (dir == NULL) {
		this->_statusCode = INTERNAL_SERVER_ERROR;
		return;
	}
	for (dirent *dirData = readdir(dir); dirData != NULL; dirData = readdir(dir)) {
		response.push_back(dirData->d_name);
	}
	closedir(dir);
	std::sort(response.begin(), response.end());
	this->_autoIndexFile.append("<!DOCTYPE html>");
	this->_autoIndexFile.append("<html>");
	this->_autoIndexFile.append("<head>");
	this->_autoIndexFile.append("<meta charset=\"utf-8\" />");
	this->_autoIndexFile.append("<html lang=\"en\"></html>");
	this->_autoIndexFile.append("<link rel=\"stylesheet\" type=\"text/css\" href=\"/stylesAutoIndex.css\" />");
	this->_autoIndexFile.append("<title>" + this->_path + "</title>");
	this->_autoIndexFile.append("</head>");
	this->_autoIndexFile.append("<body>");
	this->_autoIndexFile.append("<h1>" + this->_path + "</h1>");
	this->_autoIndexFile.append("<ul>");
	for (size_t i = 0; i < response.size(); i++) {
		this->_autoIndexFile.append("<li><a href=" + this->_urlPath + response[i] + ">" + response[i] + "</a></li>");
	}
	this->_autoIndexFile.append("</ul>");
	this->_autoIndexFile.append("</body>");
	this->_autoIndexFile.append("</html>");
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
		for (size_t i = 0; i < this->_index.size(); i++) {
			if (!access((this->_path + this->_index[i]).c_str(), F_OK | R_OK)) {
				this->_path.append(this->_index[i]);
				this->_statusCode = OK;
				return;
			}
		}
		if (this->_autoIndex) {
			this->_statusCode = OK;
			createAutoIndexFile();
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
	if (this->_autoIndexFile != "") {
		this->_contentType = "text/html";
		return;
	}
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
		else if (all && extension == "css") {
			this->_contentType = "text/css";
			return;
		}
		else if (all && extension == "js") {
			this->_contentType = "application/json";
			return;
		}
	}
	this->_statusCode = NOT_ACCEPTABLE;
}

void Requests::setBodyType(const std::string &length, const std::string &encoding, const std::string &type) {
	this->_lenOfBody = -1;
	if (encoding == "chunked") {
		this->_hasBody = CHUNKED;
		this->_lenOfBody = 0;
		return;
	}
	if (type == "application/x-www-form-urlencoded") {
		this->_hasBody = CLASSIC;
		this->_lenOfBody = std::atoi(length.c_str());
		return;
	}
	if (type.find("multipart/form-data; boundary=") != std::string::npos) {
		this->_hasBody = MULTIPART;
		this->_lenOfBody = std::atoi(length.c_str());
		this->_boundary = type.substr(type.find("=-"), type.size());
		return;
	}
	if (this->_hasBody == UNDEFINED)
		this->_statusCode = BAD_REQUEST;
	this->_hasBody = NO_ONE;
}

Requests::Requests(const std::string &buf, std::vector<Server> manager, int serverSocket) {
	std::vector<std::string> bufSplitted = split(buf, "\n");
	if (!isSyntaxGood(bufSplitted)) {
		this->_paramValid = 1;
		this->_servParam = findServerWithSocket(manager, serverSocket, "localhost");
		this->_statusCode = BAD_REQUEST;
	}
	else {
		this->_hasBody = NO_ONE;
		std::map<std::string, std::string> request;
		std::vector<std::string> methodPathProtocol = split(bufSplitted[0], " ");
		request.insert(std::make_pair("Method", methodPathProtocol[0]));
		request.insert(std::make_pair("Path", methodPathProtocol[1]));
		request.insert(std::make_pair("Protocol", methodPathProtocol[2]));
		for (size_t i = 1; i < bufSplitted.size(); i++) {
			if (this->_hasBody || (bufSplitted[i] == "\r" && i + 1 != bufSplitted.size())) {
				this->_hasBody = UNDEFINED;
				this->_body = buf.substr(buf.find("\r\n\r\n") + 4, buf.size());
				break;
			}
			else
				request.insert(std::make_pair(bufSplitted[i].substr(0, bufSplitted[i].find(": ")), bufSplitted[i].substr(bufSplitted[i].find(": ") + 2, bufSplitted[i].size())));
		}
		this->_paramValid = 1;
		this->_servParam = findServerWithSocket(manager, serverSocket, request["Host"]);
		setBodyType(request["Content-Length"], request["Transfer-Encoding"], request["Content-Type"]);
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
					}
				}
			}
		}
	}
}

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
		exportVar(env, "CONTENT_TYPE", "application/x-www-form-urlencoded"); //todo _contentTypeRequest
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
		return this->_statusCode = INTERNAL_SERVER_ERROR, setErrorPage();
	childPid = fork();
	if (childPid == -1)
		return this->_statusCode = INTERNAL_SERVER_ERROR, setErrorPage();
	if (!this->_method.compare("POST")) {
		if (pipe(fdBody) == -1)
			return this->_statusCode = INTERNAL_SERVER_ERROR, setErrorPage();
		if (write(fdBody[1], this->_body.c_str(), static_cast<int>(this->_body.size())) == -1)
			return this->_statusCode = INTERNAL_SERVER_ERROR, setErrorPage();
	}
	if (!childPid) {
		if (!this->_method.compare("POST")) {
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
			return close(fd[0]), close(fd[1]), this->_statusCode = INTERNAL_SERVER_ERROR, setErrorPage();
	if (WEXITSTATUS(childValue) == 1)
		return close(fd[0]), close(fd[1]), this->_statusCode = INTERNAL_SERVER_ERROR, setErrorPage();
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
	return setResponseScript(scriptContent, "OK") + scriptContent;
}

std::string Requests::doUpload() {
	DIR *dirFd = opendir(this->_uploadDir.c_str());
	if (!dirFd) {
		this->_statusCode = BAD_REQUEST;
		return setErrorPage();
	}
	closedir(dirFd);
	size_t posFilename = this->_body.find("filename=");
	if (posFilename == std::string::npos) {
		this->_statusCode = BAD_REQUEST;
		return setErrorPage();
	}
	posFilename += 10;
	size_t endFilename = this->_body.find("\"", posFilename);
	if (endFilename == std::string::npos) {
		this->_statusCode = BAD_REQUEST;
		return setErrorPage();
	}
	std::string filename = this->_body.substr(posFilename, endFilename - posFilename);
	std::string filePath = this->_uploadDir + "/" + filename;

    int count = 1;
    std::string newFilePath = filePath;
    size_t dotPosition = filename.find_last_of(".");
    std::string baseName = filename.substr(0, dotPosition);
    std::string extension = (dotPosition != std::string::npos) ? filename.substr(dotPosition) : "";
    while (std::ifstream(newFilePath.c_str()).is_open()) {
        std::stringstream ss;
        ss << this->_uploadDir << "/" << baseName << "_" << count++ << extension;
        newFilePath = ss.str();
    }
    std::ofstream outFile(newFilePath.c_str(), std::ios::binary);
    if (!outFile.is_open()) {
        this->_statusCode = INTERNAL_SERVER_ERROR;
        return setErrorPage();
    }
	std::string bodyToUpload = this->_body;
	int lineCount = 0, startPos = 0, endPos = 0;
	while (lineCount < 4) {
		endPos = bodyToUpload.find('\n', startPos);
		endPos++;
		bodyToUpload.erase(startPos, endPos);
		lineCount++;
	}
	size_t lastLinePos = bodyToUpload.find_last_of('\n');
	if (lastLinePos != std::string::npos)
		bodyToUpload.erase(lastLinePos);
	lastLinePos = bodyToUpload.find_last_of('\n');
	if (lastLinePos != std::string::npos)
		bodyToUpload.erase(lastLinePos);
	outFile << bodyToUpload;
	outFile.close();
	this->_path.erase(this->_path.find_last_of("/"), this->_path.size());
	this->_path = this->_path + "/uploadSuccessful.html";
	this->_statusCode = OK;
	return getPage(this->_path, setResponse("OK"));
}

std::string Requests::doDelete() {
	this->_statusCode = OK;
	std::vector<std::string> response;
	std::string page;
	DIR *dir = opendir(this->_uploadDir.c_str());
	if (dir == NULL) {
		this->_statusCode = INTERNAL_SERVER_ERROR;
		return setErrorPage();
	}
	for (dirent *dirData = readdir(dir); dirData != NULL; dirData = readdir(dir))
		response.push_back(dirData->d_name);
	closedir(dir);
	std::sort(response.begin(), response.end());
	page.append("<!DOCTYPE html>");
	page.append("<html>");
	page.append("<head>");
	page.append("<meta charset=\"utf-8\" />");
	page.append("<html lang=\"en\"></html>");
	page.append("<link rel=\"stylesheet\" type=\"text/css\" href=\"/stylesAutoIndex.css\" />");
	page.append("<title>Delete</title>");
	page.append("</head>");
	page.append("<body>");
	page.append("<h1>Choose files :</h1>");
	page.append("<ul>");
	for (size_t i = 0; i < response.size(); i++) {
		if (response[i] != "." && response[i] != "..") {
			std::cout << "gggggggggggggg : " << this->_uploadDir + "/" + response[i] << std::endl << std::endl;
			page.append("<li>");
			page.append("<a href=" + this->_uploadDir + "/" + response[i] + ">" + response[i] + "</a>");
			page.append("<button class=\"delete-button\" data-url=" + this->_uploadDir + "/" + response[i] + ">&#10005;</button>");
			page.append("</li>");
		}
	}
	page.append("</ul>");
	page.append("<script src=\"delete.js\"></script>");
	page.append("</body>");
	page.append("</html>");
	std::cout << page << std::endl;
	return setResponseScript(page, "OK") + page;
}

bool Requests::getBody(const std::string &add) {
	if (!this->_hasBody)
		return false;
	if (this->_body.size() == static_cast<size_t>(this->_lenOfBody))
		return false;
	this->_body.append(add);
	return true;
}

std::string Requests::getResponse() {
	std::cout << this->_path << std::endl;
	std::cout << this->_statusCode << std::endl;
	if (this->_body.size() != 0 && this->_body.size() != static_cast<size_t>(this->_lenOfBody))
		this->_statusCode = BAD_REQUEST;
	if (!this->_paramValid)
		return "";
	if (this->_path.find(this->_uploadDir) != std::string::npos && this->_body != "")
		return doUpload();
	if (this->_path == "./pages/listFiles/delete.html")
		return doDelete();
	if (this->_statusCode == OK || this->_statusCode == FOUND) {
		if (this->_statusCode == OK && this->_method == "POST" && this->_hasBody == MULTIPART)
    		return doUpload();
		if (this->_autoIndexFile != "")
			return setResponseScript(this->_autoIndexFile, "OK") + this->_autoIndexFile;
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