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
		Requests(const std::string &method, const std::string &path, std::vector<std::string> &accept);
		~Requests();
		std::string getResponse();

	private :

		int _statusCode;
		const std::string _method;
		std::string _path;
		const std::vector<std::string> _accept;
		void checkPage();
		bool checkExtension() const;
};

Requests readRequest(std::string buf);