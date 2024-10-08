#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include "LocationConfig.hpp"

class ServerConfig
{
public:

    bool hasRoot;
    bool hasHost;
    bool hasListen;
    
    ServerConfig();
    ServerConfig(ServerConfig const &other);
    ServerConfig &operator=(ServerConfig const &other);
    ~ServerConfig();

    void setHost(const std::string &h);
    void addPort(int portNumber);
    void setServerName(const std::string &name);
    void setClientMaxBodySize(int size);
    void setRoot(const std::string &rt);
    void addLocation(const LocationConfig &loc);
    void addErrorPage(int code, const std::string &path);

    std::string getHost() const;
    std::string getServerName() const;
    int getClientMaxBodySize() const;
    std::string getRoot() const;
    std::vector<LocationConfig> getLocations() const;
    std::map<int, std::string> getErrorPages() const;
    std::vector<int> getPorts() const;
    bool isValid() const;

private:
    std::string host;
    std::string server_name;
    int client_max_body_size;
    std::string root;
    std::vector<int> ports;
    std::vector<LocationConfig> locations;
    std::map<int, std::string> error_pages;
};