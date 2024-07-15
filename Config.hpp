#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>


struct ServerConfig
{
    int port;
    std::string host;
    std::string root_directory;
    std::string default_file;
    std::string error_page_404;
};

struct LogConfig
{
    std::string log_file;
    std::string log_level;
};

struct Config
{
    ServerConfig server;
    LogConfig log;
};

Config readConfig(const std::string& filename);

#endif 