#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include "LocationConfig.hpp"
#include "ErrorPageConfig.hpp"

class ServerConfig
{
public:
    ServerConfig();
    ServerConfig(ServerConfig const &other);
    ServerConfig &operator=(ServerConfig const &other);
    ~ServerConfig();

    void setListen(int l);
    void setHost(const std::string &h);
    void addHost(const std::string &portNumber);
    void setServerName(const std::string &name);
    void setClientMaxBodySize(int size);
    void setRoot(const std::string &rt);
    void addLocation(const LocationConfig &loc);
    void addErrorPage(const ErrorPageConfig &error);

    int getListen() const;
    std::string getHost() const;
    std::string getServerName() const;
    int getClientMaxBodySize() const;
    std::string getRoot() const;
    std::vector<LocationConfig> getLocations() const;
    std::vector<ErrorPageConfig> getErrorPages() const;

private:

    int listen;
    std::string host;
    std::string server_name;
    int client_max_body_size;
    std::string root;
    std::vector<std::string> ports;
    std::vector<LocationConfig> locations;
    std::vector<ErrorPageConfig> error_pages;
};