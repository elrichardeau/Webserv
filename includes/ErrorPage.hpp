#pragma once

#include <vector>
#include <map>
#include <string>

class ErrorPage {

	public :

		ErrorPage(std::map<int, std::string> errorPages);
		~ErrorPage();

	private :

		const std::string _protocol = "HTTP/1.1";
		const std::string _contentType = "text/html";
		std::map<int, std::string> _errorPage;
};