#pragma once

#include <unistd.h>
#include <vector>
#include <map>
#include <string>

typedef std::map<int, std::string >::iterator MapIterator;
typedef std::map<int, std::vector<std::string> >::iterator MapVectorIterator;

class ErrorPage {

	public :

		ErrorPage();
		ErrorPage& operator=(const ErrorPage &copy);
		ErrorPage(std::map<int, std::string> errors);
		~ErrorPage();
		void setNewPath(int statusCode, const std::string &newPath);
		std::string getPath(int statusCode);
		std::string getErrorMessage(int statusCode);
		std::string getProtocol() const;
		std::string getContentType() const;

	private :

		std::map<int, std::vector<std::string> > _errorPage;
		std::string _protocol;
		std::string _contentType;
};