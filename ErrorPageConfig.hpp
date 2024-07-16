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
	
		void setCode(int c);
		void setPath(const std::string &p);

		int getCode() const;
		std::string getPath() const;

	private:
		int code;
		std::string path;
};