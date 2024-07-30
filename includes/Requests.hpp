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
#include "Server.hpp"

enum StatusCode {
	OK = 200,
	MOVED_PERMANENTLY = 301,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	INTERNAL_SERVER_ERROR = 500,
	HTTP_VERSION_NOT_SUPPORTED = 505
};


class Requests {

	public :

		Requests(const std::string &buf, const Server &servParam);
		~Requests();
		std::string getResponse();

		//cgi elodie
		std::string 			 execCgi(const std::string& scriptType);
		char**					 createEnv();
		char **					 vectorToCharArray(const std::vector<std::string> &vector);
	std::vector<std::string> 	 createCgiEnv();

	private :

		int _statusCode;
		std::string _method;
		std::string _path;
		std::string _query;
		std::string _protocol;
		std::vector<std::string> _accept;
		std::string _contentType;
		const Server _servParam;
		void checkPage();
		bool checkExtension();
		std::string setResponse();
		std::string getErrorPage();
};