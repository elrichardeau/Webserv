#pragma once

#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Server.hpp"
#include "Config.hpp"

enum StatusCode {
	OK = 200,
	FOUND = 302,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

enum BodyStatus {
	NO_ONE,
	UNDEFINED,
	CLASSIC,
	CHUNKED,
	MULTIPART,
};

class Requests {

	public :

		Requests(const std::string &buf, std::vector<Server> manager, int serverSocket);
		~Requests();
		std::string getResponse();
		bool getBody(const std::string &add);

	private :

		int _statusCode;
		std::string _method;
		std::string _urlPath;
		std::string _path;
		std::string _query;
		std::string _protocol;
		std::vector<std::string> _accept;
		std::vector<std::string> _allowMethod;
		std::vector<std::string> _index;
		bool _autoIndex;
		std::string _root;
		std::string _uploadDir;
		std::string _redirection;
		std::string _contentType;
		Server _servParam;
		bool _paramValid;
		std::string _cgiPathPy;
		std::string _cgiPathPhp;
		std::string _body;
		int _hasBody;
		int _lenOfBody;
		std::string _boundary;
		std::string _autoIndexFile;

		void getQuery();
		std::string setErrorPage();
		std::string setResponse(const std::string &codeName);
		Server findServerWithSocket(std::vector<Server> manager, int serverSocket, std::string serverName);
		void getRootPath(const std::string &path);
		void setPath();
		bool isMethodAllowed();
		void setContentType();
		void createAutoIndexFile();
		void setBodyType(const std::string &length, const std::string &encoding, const std::string &type);

		std::string execCgi(const std::string& scriptType);
		std::string readFromPipe(int pipeFd);
		char** vectorToCharArray(const std::vector<std::string> &vector);
	    std::vector<std::string> createCgiEnv();
		std::string setResponseScript(const std::string &scriptResult, const std::string &codeName);
		std::string doUpload();
};

std::string itostr(int nb);