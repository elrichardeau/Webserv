#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <fstream>

enum StatusCode {
	OK = 200,
	CREATED = 201,
	FOUND = 302,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	REQUEST_ENTITY_TOO_LARGE = 413,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

class ErrorCode : public std::exception {

	public:

		ErrorCode(const std::string &errorCode) : _errorCode(errorCode) {}
		virtual const char* what() const throw() {return this->_errorCode.c_str();}
		virtual ~ErrorCode() throw(){}

	private:

		std::string _errorCode;
};

std::string itostr(int nb);
std::vector<std::string> split(std::string buf, const std::string &find);
std::string getPage(std::string filename);