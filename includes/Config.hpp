#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <set>
#include "LocationConfig.hpp"
#include "ErrorPageConfig.hpp"
#include "ServerConfig.hpp"

class Config 
{
	public:
		Config();
		Config(Config const &other);
		Config &operator=(Config const &other);
		~Config();

		void addServer(const ServerConfig &server);
		std::vector<ServerConfig> getServers() const;
		static std::vector<std::string> split(const std::string &str, char delimiter);
		static Config readConfig(const std::string &filename);
		static void location(std::vector<std::string> &tokens, std::string line, LocationConfig &current_location);
		static void server(std::vector<std::string> &tokens, std::string line, ServerConfig &current_server);
		static void errorPage(std::vector<std::string> &tokens, std::string line, ServerConfig &current_server);
	private:
		std::vector<ServerConfig> servers;
	
	class InvalidConfig : public std::exception
	{
		public:
		InvalidConfig(const std::string &message) : _message(message) {}
		virtual const char *what(void) const throw()
		{
			return (_message.c_str());
		}
		virtual ~InvalidConfig() throw(){}
		private:
			std::string _message;
	};
};