#pragma once

#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unistd.h>
#include <set>
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

class Config 
{
	public:
		Config(const std::string &filename);
		~Config();
		std::vector<ServerConfig> getServers() const;
		void readConfig(const std::string &filename);
	
	private:
		std::vector<ServerConfig> servers;
		std::vector <std::string> validDirectives;
		std::vector<std::string> tokens;

		std::string line;

		bool inServerBlock;
    	bool inLocationBlock;
		bool inErrorPageBlock;
		bool hasReadData;

		void addServer(const ServerConfig &server);
		bool isUniqueServer(const ServerConfig &newServer);
		void inBlocks(bool &inLocationBlock, bool &inServerBlock, bool &inErrorPageBlock, ServerConfig &currentServer, LocationConfig &currentLocation);

		static void configLocation(bool &inLocationBlock, LocationConfig &currentLocation, std::string line);
		static void checkLine(std::vector <std::string> validDirectives, std::vector<std::string> tokens, std::string line);
		static void handleReturn(std::vector<std::string> &tokens, LocationConfig &currentLocation);

		static std::vector<std::string> split(const std::string &str, char delimiter);
		static void location(std::vector<std::string> &tokens, LocationConfig &currentLocation);
		static void server(std::vector<std::string> &tokens, ServerConfig &currentServer);
		static void errorPage(std::vector<std::string> &tokens, ServerConfig &currentServer);

		static void allowMethods(std::vector<std::string> &tokens, LocationConfig &currentLocation);
		static void index(std::vector<std::string> &tokens, LocationConfig &currentLocation);
		static void root(std::vector<std::string> &tokens, LocationConfig &currentLocation);
		static void cgiExtensions(std::vector<std::string> &tokens, LocationConfig &currentLocation);
		static void cgiPaths(std::vector<std::string> &tokens, LocationConfig &currentLocation);
		static void uploadDir(std::vector<std::string> &tokens, LocationConfig &currentLocation);
		static bool autoIndex(std::vector<std::string> &tokens, LocationConfig &currentLocation);

		static void listen(std::vector<std::string> &tokens, ServerConfig &currentServer);
		static void host(std::vector<std::string> &tokens, ServerConfig &currentServer);
		static void serverName(std::vector<std::string> &tokens, ServerConfig &currentServer);
		static void body(std::vector<std::string> &tokens, ServerConfig &currentServer);
		static void root(std::vector<std::string> &tokens, ServerConfig &currentServer);
		static void finalizeLocation(LocationConfig &currentLocation);

		void processLine(ServerConfig &currentServer, LocationConfig &currentLocation);
	
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