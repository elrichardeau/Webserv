#pragma once

#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sys/wait.h>
#include "../includes/utils.hpp"

class Cgi {

	public :

		Cgi(const std::string &method, const std::string &query, const std::string &body, const std::string &scriptInterpreter, const std::string &path);
		~Cgi();
		std::string execCgi();

	private :

		std::string _method;
		std::string _query;
		std::string _body;
		std::string _scriptInterpreter;
		std::string _path;

		std::string readFromPipe(int pipeFd);
		char **vectorToCharArray(const std::vector<std::string> &vector);
		std::vector<std::string> createCgiEnv();
};