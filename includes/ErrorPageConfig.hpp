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
	
		// void setCode(int c);
		// void setPath(const std::string &p);
		void setErrorPage(int code, const std::string &path);

		// int getCode() const;
		// std::string getPath() const;
		std::map<int, std::string> getErrorPages() const;

	private:
		// int code;
		// std::string path;
		std::map<int, std::string> errorPages;
};