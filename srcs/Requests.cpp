
#include "../includes/Requests.hpp"

bool isSyntaxGood(std::vector<std::string> &request) {
	size_t find;
	if (request.empty() || request[0].empty() || split(request[0], " ").size() != 3)
		return false;
	for (size_t i = 0; i < request.size(); i++) {
		if (request[i] == "\r" && i == request.size() - 1) {
			request.resize(i);
			break;
		}
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

std::map<std::string, std::string> getHeaders(const std::string &request) {
	std::vector<std::string> headersData = split(request, "\n");
	if (!isSyntaxGood(headersData))
		return std::map<std::string, std::string>();
	std::map<std::string, std::string> headers;
	std::vector<std::string> methodPathProtocol = split(headersData[0], " ");
	headers.insert(std::make_pair("Method", methodPathProtocol[0]));
	headers.insert(std::make_pair("Path", methodPathProtocol[1]));
	headers.insert(std::make_pair("Protocol", methodPathProtocol[2]));
	for (size_t i = 1; i < headersData.size(); i++)
		headers.insert(std::make_pair(headersData[i].substr(0, headersData[i].find(": ")), headersData[i].substr(headersData[i].find(": ") + 2, headersData[i].size())));
	return headers;
}

std::string createBadRequestResponse(std::vector<Server> manager, int serverSocket) {
	Server servParam;
	for (std::vector<Server>::iterator it = manager.begin(); it != manager.end(); it++) {
		if (it->getServerSocket() == serverSocket) {
			servParam = *it;
			break;
		}
	}
	ErrorPage err = servParam.getErrorPage();
	std::string response;
	struct stat buf;
	int st = stat(err.getPath(BAD_REQUEST).c_str(), &buf);
	response.append(err.getProtocol() + " " + itostr(BAD_REQUEST) + " " + err.getErrorMessage(BAD_REQUEST) + "\n");
	response.append("Connection: Keep-Alive\n");
	if (st != -1) {
		size_t bytes = buf.st_size;
		response.append("Content-Length: ");
		response.append(itostr(bytes));
		response.append("\n");
	}
	response.append("Content-Location: " + err.getPath(BAD_REQUEST).substr(1, err.getPath(BAD_REQUEST).size()) + "\n");
	response.append("Content-Type: " + err.getContentType() + "\n");
	if (st != -1) {
		response.append("Date: ");
		response.append(ctime(&buf.st_atime));
		response.append("Last-Modified: ");
		response.append(ctime(&buf.st_mtime));
	}
	response.append("Server: Webserv\n");
	response.append("\n");
	response.append(getPage(err.getPath(BAD_REQUEST)));
	return response;
}

std::string createErrorResponse(ErrorPage err, int code) {
	std::string response;
	struct stat buf;
	int st = stat(err.getPath(code).c_str(), &buf);
	response.append(err.getProtocol() + " " + itostr(code) + " " + err.getErrorMessage(code) + "\n");
	response.append("Connection: Keep-Alive\n");
	if (st != -1) {
		size_t bytes = buf.st_size;
		response.append("Content-Length: ");
		response.append(itostr(bytes));
		response.append("\n");
	}
	response.append("Content-Location: " + err.getPath(code).substr(1, err.getPath(code).size()) + "\n");
	response.append("Content-Type: " + err.getContentType() + "\n");
	if (st != -1) {
		response.append("Date: ");
		response.append(ctime(&buf.st_atime));
		response.append("Last-Modified: ");
		response.append(ctime(&buf.st_mtime));
	}
	response.append("Server: Webserv\n");
	response.append("\n");
	response.append(getPage(err.getPath(code)));
	return response;
}

BodyType getBodyType(const std::string &length, const std::string &encoding, const std::string &type) {
	if (encoding == "chunked")
		return CHUNKED;
	if (type == "application/x-www-form-urlencoded")
		return SIMPLE;
	if (type.find("multipart/form-data; boundary=") != std::string::npos)
		return MULTIPART;
	if (length != "")
		return SIMPLE;
	return NOTHING;
}

void Requests::getBody(std::vector<unsigned char> buf) {
	this->_body.insert(this->_body.end(), buf.begin(), buf.end());
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

std::string Requests::setUrlPath() {
	std::string urlPath = this->_path;
	if (urlPath.find_last_of("/") != urlPath.size() - 1)
		urlPath.append("/");
	return urlPath;
}

std::string Requests::setQuery() {
	std::string query;
	size_t find = this->_path.find("?");
	if (find != std::string::npos) {
		query = this->_path.substr(find + 1, this->_path.size());
		this->_path.erase(find, this->_path.size());
	}
	return query;
}

Requests::Requests(Server &servParam, std::map<std::string, std::string> &headers, std::vector<unsigned char> &body, const std::string &lenOfBody, BodyType bodyType) :
	_servParam(servParam), _protocol(headers["Protocol"]), _method(headers["Method"]), _path(headers["Path"]), _urlPath(setUrlPath()),
	_query(setQuery()), _accept(getAccept(headers["Accept"])), _body(body), _lenOfBody(atoi(lenOfBody.c_str())), _bodyType(bodyType) {}

Requests::~Requests() {}

BodyType Requests::getBodyType() {
	if (this->_body.size() >= this->_lenOfBody)
		return NOTHING;
	return this->_bodyType;
}

bool Requests::isMethodAllowed() {
	for (size_t i = 0; i < this->_allowMethod.size(); i++) {
		if (this->_allowMethod[i] == this->_method)
			return true;
	}
	return false;
}

void Requests::setFileName(const std::string &headersOfBody) {
	size_t start = headersOfBody.find("filename=\"");
	if (start == std::string::npos)
		throw ErrorCode(itostr(BAD_REQUEST));
	start += 10;
	size_t end = headersOfBody.find("\"", start);
	if (end == std::string::npos)
		throw ErrorCode(itostr(BAD_REQUEST));
	std::string fileName = headersOfBody.substr(start, end - start);
	std::string filePath = this->_uploadDir + "/" + fileName;
	size_t dotPosition = fileName.find_last_of(".");
	std::string baseName = fileName.substr(0, dotPosition);
	std::string extension = (dotPosition != std::string::npos) ? fileName.substr(dotPosition) : "";
	for (size_t i = 1;; i++) {
		std::ifstream fd(filePath.c_str());
		if (!fd.is_open())
			break;
		fd.close();
		std::stringstream ss;
		ss << this->_uploadDir << "/" << baseName << "_" << i << extension;
		filePath = ss.str();
	}
	this->_uploadDir = filePath;
}

void Requests::removeBoundary() {
	std::vector<unsigned char>::iterator it;
	it = std::search(this->_body.begin(), this->_body.end(),"\r\n\r\n", "\r\n\r\n" + 4);
	if (it == this->_body.end())
		throw ErrorCode(itostr(BAD_REQUEST));
	it += 4;
	std::string headersOfBody;
	headersOfBody.insert(headersOfBody.end(), this->_body.begin(), it);
	std::vector<unsigned char> tmp;
	tmp.insert(tmp.end(), it, this->_body.end());
	bzero(this->_body.data(), this->_body.size());
	this->_body = tmp;
	tmp.clear();
	it = std::find_end(this->_body.begin(), this->_body.end(),"\r\n", "\r\n" + 2);
	if (it == this->_body.end())
		throw ErrorCode(itostr(BAD_REQUEST));
	it = std::find_end(this->_body.begin(), it,"\r\n", "\r\n" + 2);
	if (it == this->_body.end())
		throw ErrorCode(itostr(BAD_REQUEST));
	tmp.insert(tmp.end(), this->_body.begin(), it);
	bzero(this->_body.data(), this->_body.size());
	this->_body = tmp;
	tmp.clear();
	setFileName(headersOfBody);
}

void Requests::parseMultipartBody() {
	DIR *dirFd = opendir(this->_uploadDir.c_str());
	if (!dirFd)
		throw ErrorCode(itostr(BAD_REQUEST));
	closedir(dirFd);
	removeBoundary();
}

void Requests::checkData() {
	if (this->_protocol != "HTTP/1.1")
		throw ErrorCode(itostr(HTTP_VERSION_NOT_SUPPORTED));
	if (!isMethodAllowed())
		throw ErrorCode(itostr(METHOD_NOT_ALLOWED));
	if (this->_body.size() > this->_servParam.getClientMaxBodySize())
		throw ErrorCode(itostr(BAD_REQUEST));
	if (this->_bodyType == MULTIPART)
		parseMultipartBody();
}

void Requests::setLocations() {
	std::vector<LocationConfig> tmp = this->_servParam.getLocations();
	std::string firstPath = this->_path;
	size_t find = firstPath.find("/", 1);
	if (find != std::string::npos)
		firstPath.erase(find, firstPath.size());
	for (size_t i = 0; i < tmp.size(); i++) {
		if (tmp[i].getPath() == firstPath) {
			this->_allowMethod = tmp[i].getAllowMethods();
			this->_index = tmp[i].getIndex();
			this->_autoIndex = tmp[i].getautoIndex();
			this->_root = tmp[i].getRoot();
			if (tmp[i].getUploadDir().empty())
				this->_uploadDir = "";
			else
				this->_uploadDir = "." + this->_root + firstPath + tmp[i].getUploadDir();
			this->_redirection = tmp[i].getReturnDirective();
			this->_cgiPathPy = "";
			this->_cgiPathPhp = "";
			this->_cgiExtensions = tmp[i].getCgiExtensions();
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
	this->_autoIndex = false;
	this->_root = this->_servParam.getRoot();
	this->_uploadDir = "";
	this->_redirection = "";
	this->_cgiPathPy = "";
	this->_cgiPathPhp = "";
}

void Requests::getContentType() {
	bool all = false;
	size_t find = this->_path.find_last_of(".");
	if (find != std::string::npos) {
		std::string extension = this->_path.substr(find + 1, this->_path.size());
		for (size_t i = 0; i < this->_accept.size(); i++) {
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
		if (all && extension == "html") {
			this->_contentType = "text/html";
			return;
		}
		else if (all && extension == "ico") {
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
	throw ErrorCode(itostr(NOT_ACCEPTABLE));
}

void Requests::checkPermissionPath() {
	if (access(this->_path.c_str(), F_OK))
		throw ErrorCode(itostr(NOT_FOUND));
	else if (access(this->_path.c_str(), R_OK))
		throw ErrorCode(itostr(FORBIDDEN));
}

std::string Requests::setResponse(StatusCode statusCode) {
	checkPermissionPath();
	this->_statusCode = statusCode;
	getContentType();
	std::string statusCodeName;
	if (statusCode == CREATED)
		statusCodeName = "Created";
	else if (statusCode == FOUND)
		statusCodeName = "Found";
	else
		statusCodeName = "OK";
	return setHeaders(statusCodeName) + getPage(this->_path);
}

std::string Requests::setFaviconResponse() {
	this->_path = "./pages/favicon.ico";
	return setResponse(OK);
}

std::string Requests::setRedirectionResponse() {
	std::vector<std::string> redirection = split(this->_redirection, " ");
	this->_statusCode = static_cast<StatusCode>(std::atoi(redirection[0].c_str()));
	if (this->_statusCode != FOUND)
		throw ErrorCode(itostr(NOT_IMPLEMENTED));
	size_t find = this->_path.find("/", 1);
	if (find == std::string::npos) {
		this->_path = "." + redirection[1];
		return setResponse(FOUND);
	}
	std::string path = this->_path.substr(find, this->_path.size() - find);
	this->_path = "." + redirection[1] + path;
	return setResponse(FOUND);
}

std::string Requests::doDelete() {
	size_t find = this->_path.find_last_of("/");
	if (find == std::string::npos)
		throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
	this->_path = this->_uploadDir + this->_path.substr(find, this->_path.size() - find);
	if (std::remove(this->_path.c_str()) == -1)
		throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
	this->_path = "./pages/listFiles/deleteSuccessful.html";
	return setResponse(OK);
}

bool Requests::isFolder() {
	DIR *dir = opendir(this->_path.c_str());
	if (dir != NULL) {
		closedir(dir);
		return true;
	}
	return false;
}

std::string Requests::createAutoIndexFile() {
	std::vector<std::string> files;
	DIR *dir = opendir(this->_path.c_str());
	if (dir == NULL)
		throw ErrorCode(itostr(INTERNAL_SERVER_ERROR));
	for (dirent *dirData = readdir(dir); dirData != NULL; dirData = readdir(dir)) {
		files.push_back(dirData->d_name);
	}
	closedir(dir);
	std::sort(files.begin(), files.end());
	std::string autoIndexFile;
	autoIndexFile.append("<!DOCTYPE html>");
	autoIndexFile.append("<html>");
	autoIndexFile.append("<head>");
	autoIndexFile.append("<meta charset=\"utf-8\" />");
	autoIndexFile.append("<html lang=\"en\"></html>");
	autoIndexFile.append("<link rel=\"stylesheet\" type=\"text/css\" href=\"/stylesAutoIndex.css\" />");
	autoIndexFile.append("<title>" + this->_path + "</title>");
	autoIndexFile.append("</head>");
	autoIndexFile.append("<body>");
	autoIndexFile.append("<h1>" + this->_path + "</h1>");
	autoIndexFile.append("<ul>");
	for (size_t i = 0; i < files.size(); i++)
		autoIndexFile.append("<li><a href=" + this->_urlPath + files[i] + ">" + files[i] + "</a></li>");
	autoIndexFile.append("</ul>");
	autoIndexFile.append("</body>");
	autoIndexFile.append("</html>");
	return autoIndexFile;
}

std::string Requests::setAutoIndexFile() {
	std::string autoIndexFile = createAutoIndexFile();
	this->_statusCode = OK;
	this->_contentType = "text/html";
	return setHeadersForScript(autoIndexFile, "OK") + autoIndexFile;
}

std::string Requests::setFolderResponse() {
	for (size_t i = 0; i < this->_index.size(); i++) {
		if (!access((this->_path + this->_index[i]).c_str(), F_OK | R_OK)) {
			this->_path.append(this->_index[i]);
			return setResponse(OK);
		}
	}
	if (this->_autoIndex)
		return setAutoIndexFile();
	throw ErrorCode(itostr(FORBIDDEN));
}

std::string Requests::getScriptExtension() {
	std::vector<std::string> words = split(this->_path, ".");
	if (words.size() == 1)
		throw ErrorCode(itostr(BAD_REQUEST));
	if (words[words.size() - 1] == "py" || words[words.size() - 1] == "php") {
		std::vector<std::string> ext = this->_cgiExtensions;
		for (size_t i = 0; i < ext.size(); i++) {
			if (words[words.size() - 1] == ext[i])
				return ext[i];
		}
	}
	return "";
}

std::string Requests::setScriptResponse(const std::string &extension) {
	std::string scriptInterpreter;
	if (extension == "py")
		scriptInterpreter = this->_cgiPathPy;
	else
		scriptInterpreter = this->_cgiPathPhp;
	Cgi script(this->_method, this->_query, std::string(this->_body.begin(), this->_body.end()), scriptInterpreter, this->_path, this->_uploadDir);
	std::string scriptResult = script.execCgi();
	this->_statusCode = OK;
	this->_contentType = "text/html";
	return setHeadersForScript(scriptResult, "OK") + scriptResult;
}

std::string Requests::getResponse() {
	if (this->_path == "/favicon.ico")
		return setFaviconResponse();
	if (this->_redirection != "")
		return setRedirectionResponse();
	if (this->_method == "DELETE")
		return doDelete();
	this->_path = "." + this->_root + this->_path;
	if (isFolder())
		return setFolderResponse();
	checkPermissionPath();
	std::string extension = getScriptExtension();
	if (!extension.empty())
		return setScriptResponse(extension);
	return setResponse(OK);
}

std::string Requests::setHeaders(const std::string &codeName) {
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

std::string Requests::setHeadersForScript(const std::string &scriptResult, const std::string &codeName) {
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