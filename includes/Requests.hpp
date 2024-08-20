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
#include "Cgi.hpp"
#include "utils.hpp"

enum BodyType {
	NOTHING,
	SIMPLE,
	CHUNKED,
	MULTIPART,
};

class Requests {

	public :

		Requests(Server &servParam, std::map<std::string, std::string> &headers, std::vector<unsigned char> &body, const std::string &lenOfBody, BodyType bodyType);
		~Requests();
		BodyType getBodyType();
		void getBody(std::vector<unsigned char> buf);
		void checkData();
		void setLocations();
		std::string getResponse();

	private :

		Server _servParam;
		std::string _protocol;
		std::string _method;
		std::string _path;
		std::string _urlPath;
		std::string _query;
		std::vector<std::string> _accept;
		std::vector<unsigned char> _body;
		size_t _lenOfBody;
		BodyType _bodyType;

		StatusCode _statusCode;
		std::string _contentType;

		std::vector<std::string> _allowMethod;
		std::vector<std::string> _index;
		bool _autoIndex;
		std::string _root;
		std::string _uploadDir;
		std::string _redirection;
		std::vector<std::string> _cgiExtensions;
		std::string _cgiPathPy;
		std::string _cgiPathPhp;

		std::string setUrlPath();
		std::string setQuery();
		bool isMethodAllowed();
		void parseMultipartBody();
		void setFileName(const std::string &headersOfBody);
		void removeBoundary();
		void getContentType();
		void checkPermissionPath();
		std::string setFaviconResponse();
		std::string setRedirectionResponse();
		std::string doDelete();
		bool isFolder();
		std::string createAutoIndexFile();
		std::string setAutoIndexFile();
		std::string setFolderResponse();
		std::string getScriptExtension();
		std::string setScriptResponse(const std::string &extension);

		std::string setHeaders(const std::string &codeName);
		std::string setHeadersForScript(const std::string &scriptResult, const std::string &codeName);
		std::string setResponse(StatusCode statusCode);
};

std::map<std::string, std::string> getHeaders(const std::string &request);
std::string createBadRequestResponse(std::vector<Server> manager, int serverSocket);
std::string createErrorResponse(ErrorPage err, int code);
BodyType getBodyType(const std::string &length, const std::string &encoding, const std::string &type);