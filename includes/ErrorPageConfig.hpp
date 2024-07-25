#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

class ErrorPageConfig
{	
	public:
		ErrorPageConfig();
		ErrorPageConfig(ErrorPageConfig const &other);
		ErrorPageConfig &operator=(ErrorPageConfig const &other);
		~ErrorPageConfig();
	
		void setErrorPage(int code, const std::string &path);
		std::map<int, std::string> getErrorPages() const;

	private:
		std::map<int, std::string> errorPages;
};