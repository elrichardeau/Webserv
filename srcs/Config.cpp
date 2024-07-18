#include "../includes/Config.hpp"

Config::Config(){}

Config::Config(Config const &other) 
{
    (void)other;
}

Config &Config::operator=(Config const &other)
{
    (void)other;
    return (*this);
}

Config::~Config(){}

void Config::addServer(const ServerConfig &server) 
{ 
	servers.push_back(server);
    std::cout << "Server added. Total servers: " << servers.size() << std::endl;
}

std::vector<ServerConfig> Config::getServers() const 
{ 
	return (servers); 
}

bool isValidIPAddress(const std::string &ip)
{
    int dots;
    char prev;

    dots = 0;
    prev = ' ';
    for (size_t i = 0; i < ip.length(); ++i) 
    {
        char c = ip[i];
        if (c == '.')
        {
            if (prev == '.' || prev == ' ')
                return (false);
            if (dots++ > 3)
                return (false);
        }
        else if (!isdigit(c))
            return (false);
        prev = c;
    }
    if (prev == '.')
        return (false);
    std::istringstream iss(ip);
    std::string byte;
    while (std::getline(iss, byte, '.'))
    {
        if (byte.empty() || atoi(byte.c_str()) > 255 || atoi(byte.c_str()) < 0)
            return (false);
        if (byte.size() > 1 && byte[0] == '0')
            return (false);
    }
    if (dots == 3)
        return (true);
    return (false);
}

std::vector<std::string> Config::split(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter))
    {
        token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
        if (!token.empty() && token[token.length() - 1] == ';')
            token.erase(token.length() - 1);
        if (!token.empty())
            tokens.push_back(token);
        //std::cout << "Split token: " << token << std::endl;
    }
    return (tokens);
}

void Config::location(std::vector<std::string> &tokens, std::string line, LocationConfig &current_location)
{
    tokens = Config::split(line, ' ');
    if (tokens[0] == "allow_methods")
    {
        int get, dt, post = 0;
        if (tokens.size() != 4)
            throw InvalidConfig("Error: no specified allow methods.");
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            if (tokens[i] == "GET"){
                if (get == 1)
                    throw InvalidConfig("Error: Duplicate allow methods.");
                get = 1;
            }
            else if (tokens[i] == "POST"){
                if (post == 1)
                    throw InvalidConfig("Error: Duplicate allow methods.");
                post = 1;
            } 
            else if (tokens[i] == "DELETE"){
                if (dt == 1)
                    throw InvalidConfig("Error: Duplicate allow methods.");
                dt = 1;
            }
            else 
                throw InvalidConfig("Error: Invalid allow methods.");
        }
        if (get == 1)
            current_location.addAllowMethod("GET");
        if (post == 1)
            current_location.addAllowMethod("POST");
        if (dt == 1)
            current_location.addAllowMethod("DELETE");
    } 
    else if (tokens[0] == "index")
        current_location.setIndex(tokens[1]);
    else if (tokens[0] == "root")
        current_location.setRoot(tokens[1]);
    else if (tokens[0] == "cgi_extension") 
    {
        for (size_t i = 1; i < tokens.size(); ++i)
            current_location.addCgiExtension(tokens[i]);
    }
    else if (tokens[0] == "cgi_path")
    {
        for (size_t i = 1; i < tokens.size(); i += 2)
            current_location.addCgiPath(tokens[i], tokens[i + 1]);
    } 
    else if (tokens[0] == "upload_dir")
        current_location.setUploadDir(tokens[1]);
        
}

void Config::server(std::vector<std::string> &tokens, std::string line, ServerConfig &current_server)
{
    tokens = Config::split(line, ' ');
    if (tokens[0] == "listen")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No port number specified.");
        char *buf;
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            std::string start = tokens[i];
            long port = std::strtol(start.c_str(), &buf, 10);
            std::string end = buf;
            if (end == "\0" && port >= 0 && port <=  65535)
                current_server.addPort(static_cast <int> (port));
            else 
                throw InvalidConfig("Error: Invalid port number.");
        }  
    }
    else if (tokens[0] == "host")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No host specified.");
        if (!isValidIPAddress(tokens[1]))
            throw InvalidConfig("Error: Invalid host.");
        current_server.setHost(tokens[1]);  
    }
    else if (tokens[0] == "server_name")
    {
        if (tokens.size() != 2)
            throw InvalidConfig("Error: Invalid number of server_name."); 
        current_server.setServerName(tokens[1]);
    }
    else if (tokens[0] == "client_max_body_size")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No client max body specified.");
        char *end;
        std::string body = tokens[1];
        if (body.find("-") != std::string::npos || body.find("+") != std::string::npos)
            throw InvalidConfig("Error: Invalid client max body size.");
        long bodyInt = std::strtol(body.c_str(), &end, 10);
        if (*end != '\0' || bodyInt < 0 || bodyInt >= 1000000)
            throw InvalidConfig("Error: Invalid client max body size.");
        current_server.setClientMaxBodySize(static_cast<int> (bodyInt));
    }
    else if (tokens[0] == "root")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No root specified.");
        current_server.setRoot(tokens[1]); 
    }
}

void Config::errorPage(std::vector<std::string> &tokens, std::string line, ServerConfig &current_server)
{
    tokens = Config::split(line, ' ');
    ErrorPageConfig error_page;
    error_page.setCode(std::atoi(tokens[0].c_str())); // Enlever le atoi et remettre en string le code 
    error_page.setPath(tokens[1]);
    current_server.addErrorPage(error_page);
}

Config Config::readConfig(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
        throw InvalidConfig("Error: couldn't open file.");

    std::string line;
    Config Config;
    ServerConfig current_server;
    LocationConfig current_location;
    bool in_server_block = false;
    bool in_location_block = false;
	bool in_error_page_block = false;
	std::vector<std::string> tokens;
    while (std::getline(file, line))
    {
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;

        if (line == "server {")
        {
            in_server_block = true;
            current_server = ServerConfig();
            std::cout << "Server block started." << std::endl;
        }
        else if (line == "}")
        {
            if (in_location_block)
            {
                current_server.addLocation(current_location);
                in_location_block = false;
                std::cout << "Location block closed." << std::endl;
            }
            else if (in_server_block)
            {
                Config.addServer(current_server);
                in_server_block = false;
                std::cout << "Server block closed." << std::endl;
            }
        }
        else if (line.find("location") == 0 && line[line.length() - 1] == '{')
        {
            in_location_block = true;
            current_location = LocationConfig();
            size_t path_start = line.find_first_not_of(" ", 8);
            size_t path_end = line.find("{", path_start);
            if (path_end != std::string::npos) path_end--;
            current_location.setPath(line.substr(path_start, path_end - path_start + 1));
            std::cout << "Location block started for path: " << current_location.getPath() << std::endl;
        }
        else if (line == "error_page {")
        {
            in_error_page_block = true;
            std::cout << "Error page block started." << std::endl;
        }
            
        else if (in_server_block && !in_location_block && !in_error_page_block)
            server(tokens, line, current_server);
        else if (in_location_block)
            location(tokens, line, current_location);
        else if (in_error_page_block)
            errorPage(tokens, line, current_server);
    }

    // while (std::getline(file, line))
    // {
    //     if (line.empty() || line[0] == '#' || line[0] == ';')
    //         continue;
    //     if (line == "server {") 
    //     {
    //         in_server_block = true;
    //         current_server = ServerConfig();
    //         std::cout << "!! NON ICI !!!!!!! !!" << std::endl;
    //     } 
    //     else if (line == "}") 
    //     {
    //         if (in_location_block) 
    //         {
    //             current_server.addLocation(current_location);
    //             in_location_block = false;
    //         } 
    //         else if (in_server_block) 
    //         {
    //             Config.addServer(current_server);
    //             std::cout << "Server added" << std::endl;
    //             in_server_block = false;
    //         }
    //         std::cout << "!! HERE !!!!!!! !!" << std::endl;
    //     }
    //     std::cout << "Avant location: " << line << std::endl;
    //     if (line == "location" && line[line.length() - 1] == '{') 
    //     {
    //         in_location_block = true;
    //         std::cout << "!! TRUEEEEEE !!!!!!! !!" << std::endl;
    //         current_location = LocationConfig();
    //         current_location.setPath(line.substr(9, line.length() - 9));
    //     }
    //     std::cout << "Avant error_page: " << line << std::endl;
	// 	if (line == "error_page {")
    //         in_error_page_block = true;
    //     std::cout << "Avant server_block: " << line << std::endl;
	// 	if (in_server_block && !in_location_block && !in_error_page_block)
    //         server(tokens, line, current_server);
    //     std::cout << "Avant location_block: " << line << std::endl;
    //     if (in_location_block)
    //     {
    //          std::cout << "!! ICI !!!!!!!!!" << std::endl;
    //         location(tokens, line, current_location);
    //     }
    //     std::cout << "Avant error_page_block: " << line << std::endl;
	// 	if (in_error_page_block)
    //         errorPage(tokens, line, current_server);
    // }
    return (Config);
}


