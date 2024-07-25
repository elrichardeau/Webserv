#pragma once

#include <string>
#include <vector>


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

		Requests(int statusCode);
		Requests(const std::string &method, const std::string &path, const std::string &protocol, std::vector<std::string> &accept);
		~Requests();
		std::string getResponse();

		bool isSyntaxError();
		
		//cgi elodie
		std::string 			 execCgi(const std::string& scriptType);
		char**					 createEnv();
		char **					 vectorToCharArray(const std::vector<std::string> &vector);
	std::vector<std::string> 	 createCgiEnv();
		


	private :

		int _statusCode;
		const std::string _method;

		std::string _path;
		std::string _protocol;
		const std::vector<std::string> _accept;
		std::string _contentType;
		void checkPage();
		bool checkExtension();
		std::string setResponse();
		std::string getErrorPage();

};

Requests readRequest(std::string buf);