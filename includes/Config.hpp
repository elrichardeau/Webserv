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
		Config(const std::string &filename);
		~Config();


		bool isUniqueServer(const ServerConfig &newServer);
		void addServer(const ServerConfig &server);
		std::vector<ServerConfig> getServers() const;
		static std::vector<std::string> split(const std::string &str, char delimiter);
		void readConfig(const std::string &filename);
		static void location(std::vector<std::string> &tokens, LocationConfig &current_location);
		static void server(std::vector<std::string> &tokens, ServerConfig &current_server);
		static void errorPage(std::vector<std::string> &tokens, ServerConfig &current_server);

		static void allowMethods(std::vector<std::string> &tokens, LocationConfig &current_location);
		static void index(std::vector<std::string> &tokens, LocationConfig &current_location);
		static void root(std::vector<std::string> &tokens, LocationConfig &current_location);
		static void cgiExtensions(std::vector<std::string> &tokens, LocationConfig &current_location);
		static void cgiPaths(std::vector<std::string> &tokens, LocationConfig &current_location);
		static void uploadDir(std::vector<std::string> &tokens, LocationConfig &current_location);
		static void autoIndex(std::vector<std::string> &tokens, LocationConfig &current_location);

		static void listen(std::vector<std::string> &tokens, ServerConfig &current_server);
		static void host(std::vector<std::string> &tokens, ServerConfig &current_server);
		static void serverName(std::vector<std::string> &tokens, ServerConfig &current_server);
		static void body(std::vector<std::string> &tokens, ServerConfig &current_server);
		static void root(std::vector<std::string> &tokens, ServerConfig &current_server);

		void inBlocks(bool &in_location_block, bool &in_server_block, bool &in_error_page_block, ServerConfig &current_server, LocationConfig &current_location);
		static void configLocation(bool &in_location_block, LocationConfig &current_location, std::string line);
		static void checkLine(std::vector <std::string> validDirectives, std::vector<std::string> tokens, std::string line);
		static void handleReturn(std::vector<std::string> &tokens, LocationConfig &current_location);
	
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