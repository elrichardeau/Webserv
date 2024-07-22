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

void Config::allowMethods(std::vector<std::string> &tokens, LocationConfig &current_location)
{
    if (tokens[0] == "allow_methods")
    {
        int get = 0, dt = 0, post = 0;
        if (tokens.size() != 4)
            throw InvalidConfig("Error: no specified allow methods.");
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            if (tokens[i] == "GET")
            {
                if (get == 1)
                    throw InvalidConfig("Error: Duplicate allow methods.");
                get = 1;
            }
            else if (tokens[i] == "POST")
            {
                if (post == 1)
                    throw InvalidConfig("Error: Duplicate allow methods.");
                post = 1;
            } 
            else if (tokens[i] == "DELETE")
            {
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
}

void Config::index(std::vector<std::string> &tokens, LocationConfig &current_location)
{
    if (tokens[0] == "index")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No index specified.");
        current_location.setIndex(tokens[1]);
    }
}


void Config::cgiExtensions(std::vector<std::string> &tokens, LocationConfig &current_location)
{
    if (tokens[0] == "cgi_extension") 
    {
        std::vector<std::string> validExtensions;
        validExtensions.push_back("php");
        validExtensions.push_back("py");
        std::vector<std::string> seenExtensions;
        if (tokens.size() != 3)
            throw InvalidConfig("Error: Exactly two CGI extensions are required.");

        for (size_t i = 1; i < tokens.size(); ++i)
        {
            const std::string &extension = tokens[i];
            if (std::find(validExtensions.begin(), validExtensions.end(), extension) == validExtensions.end())
                throw InvalidConfig("Error: Invalid CGI extension. Only 'php' and 'py' are allowed.");
            if (std::find(seenExtensions.begin(), seenExtensions.end(), extension) != seenExtensions.end())
                throw InvalidConfig("Error: Duplicate CGI extension specified.");
            seenExtensions.push_back(extension);
            current_location.addCgiExtension(extension);
        } 
    }
}

void Config::cgiPaths(std::vector<std::string> &tokens, LocationConfig &current_location)
{
    if (tokens[0] == "cgi_path")
    {
        std::vector<std::string> validExtensions;
        validExtensions.push_back(".php");
        validExtensions.push_back(".py");
        std::vector<std::string> seenExtensions;

        if (tokens.size() != 5)
            throw InvalidConfig("Error: Two CGI path pairs are required.");
        for (size_t i = 1; i < tokens.size(); i += 2)
        {
            const std::string &extension = tokens[i];
            const std::string &path = tokens[i + 1];
            if (std::find(validExtensions.begin(), validExtensions.end(), extension) == validExtensions.end())
                throw InvalidConfig("Error: Invalid CGI extension. Only '.php' and '.py' are allowed.");
            if (std::find(seenExtensions.begin(), seenExtensions.end(), extension) != seenExtensions.end())
                throw InvalidConfig("Error: Each CGI extension can only have one corresponding path.");
            seenExtensions.push_back(extension);
            current_location.addCgiPath(extension, path);
        }
    } 
}

void Config::uploadDir(std::vector<std::string> &tokens, LocationConfig &current_location)
{
    if (tokens[0] == "upload_dir")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No upload_dir specified.");
        current_location.setUploadDir(tokens[1]);
    }
}

void Config::autoIndex(std::vector<std::string> &tokens, LocationConfig &current_location)
{
    if (tokens[0] == "autoindex")
    {
        if (tokens.size() < 2 || (tokens[1] != "on" && tokens[1] != "off"))
            throw InvalidConfig("Error: No autoindex specified.");
        current_location.setAutoIndex(tokens[1]);
    }
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
void Config::root(std::vector<std::string> &tokens, LocationConfig &current_location)
{
    if (tokens[0] == "root")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No root specified.");
        current_location.setRoot(tokens[1]);
    }
}

void Config::location(std::vector<std::string> &tokens, LocationConfig &current_location)
{
    allowMethods(tokens, current_location);
    index(tokens, current_location);
    root(tokens, current_location);
    cgiExtensions(tokens, current_location);
    cgiPaths(tokens, current_location);
    uploadDir(tokens, current_location);
}

void Config::listen(std::vector<std::string> &tokens, ServerConfig &current_server)
{
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
}

void Config::host(std::vector<std::string> &tokens, ServerConfig &current_server)
{
    if (tokens[0] == "host")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No host specified.");
        if (!isValidIPAddress(tokens[1]))
            throw InvalidConfig("Error: Invalid host.");
        current_server.setHost(tokens[1]);  
    }
}

void Config::serverName(std::vector<std::string> &tokens, ServerConfig &current_server)
{
    if (tokens[0] == "server_name")
    {
        if (tokens.size() != 2)
            throw InvalidConfig("Error: Invalid number of server_name."); 
        current_server.setServerName(tokens[1]);
    }
}

void Config::body(std::vector<std::string> &tokens, ServerConfig &current_server)
{
    if (tokens[0] == "client_max_body_size")
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
}

void Config::root(std::vector<std::string> &tokens, ServerConfig &current_server)
{
    if (tokens[0] == "root")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No root specified.");
        current_server.setRoot(tokens[1]);
    }
}

void Config::server(std::vector<std::string> &tokens, ServerConfig &current_server)
{
    listen(tokens, current_server);
    host(tokens, current_server);
    serverName(tokens, current_server);
    body(tokens, current_server);
    root(tokens, current_server);
}

void Config::errorPage(std::vector<std::string> &tokens, ServerConfig &current_server)
{
    ErrorPageConfig error_page;
    error_page.setCode(std::atoi(tokens[0].c_str()));
    error_page.setPath(tokens[1]);
    current_server.addErrorPage(error_page);
}

void Config::inBlocks(bool &in_location_block, bool &in_server_block, bool &in_error_page_block, ServerConfig &current_server, LocationConfig &current_location)
{
    if (in_location_block)
    {
        current_server.addLocation(current_location);
        in_location_block = false;
        //std::cout << "Location block closed." << std::endl;
    } 
    else if (in_error_page_block)
    {
        in_error_page_block = false;
        //std::cout << "Error page block closed." << std::endl;
    } 
    else if (in_server_block)
    {
        this->addServer(current_server);
        in_server_block = false;
        //std::cout << "Server block closed." << std::endl;
    }
}


void Config::configLocation(bool &in_location_block, LocationConfig &current_location, std::string line)
{
    in_location_block = true;
    current_location = LocationConfig();
    size_t path_start = line.find_first_not_of(" ", 8);
    size_t path_end = line.find("{", path_start);
    if (path_end != std::string::npos)
        path_end--;
    current_location.setPath(line.substr(path_start, path_end - path_start + 1));
    //std::cout << "Location block started for path: " << current_location.getPath() << std::endl;
}

bool endsWith(const std::string &line, char character)
{
    if (line.empty())
        return (false);
    return (line[line.size() - 1] == character);
}

bool isValidineEnding(const std::string &line)
{
    size_t lastNonSpace = line.find_last_not_of(" \t\n\r");
    if (lastNonSpace == std::string::npos)
        return (true);
    char lastChar = line[lastNonSpace];
    if (lastChar == ';')
    {
        if (lastNonSpace > 0 && (line[lastNonSpace - 1] == '{' || line[lastNonSpace - 1] == '}'))
            return (false);
        return (true);
    }
    else if (lastChar == '{' || lastChar == '}')
    {
        return (lastNonSpace == line.size() - 1);
    }
    return (false);
}

void Config::checkLine(std::vector <std::string> validDirectives, std::vector<std::string> tokens, std::string line)
{
    validDirectives.push_back("server");
    validDirectives.push_back("listen");
    validDirectives.push_back("host");
    validDirectives.push_back("server_name");
    validDirectives.push_back("client_max_body_size");
    validDirectives.push_back("location");
    validDirectives.push_back("index");
    validDirectives.push_back("allow_methods");
    validDirectives.push_back("autoindex");
    validDirectives.push_back("upload_dir");
    validDirectives.push_back("root");
    validDirectives.push_back("cgi_extension"); 
    validDirectives.push_back("cgi_path");
    validDirectives.push_back("error_page");

    bool isValidDirective = false;
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (std::find(validDirectives.begin(), validDirectives.end(), tokens[i]) != validDirectives.end())
        {
            isValidDirective = true;
            if (!isValidineEnding(line))
                throw InvalidConfig("Error: Invalid line.");
            break;
        }
        if (!isValidDirective)
            continue;
    }
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
    std::vector <std::string> validDirectives;
	std::vector<std::string> tokens;
    while (std::getline(file, line))
    {
        tokens = Config::split(line, ' ');
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        if (line.empty())
            continue;
        checkLine(validDirectives, tokens, line);
        if (line == "server {")
        {
            in_server_block = true;
            current_server = ServerConfig();
        }
        else if (line == "}")
            Config.inBlocks(in_location_block, in_server_block, in_error_page_block, current_server, current_location);
        else if (line.find("location") == 0 && line[line.length() - 1] == '{')
            configLocation(in_location_block, current_location, line);
        else if (line == "error_page {")
            in_error_page_block = true;
        else if (in_server_block && !in_location_block && !in_error_page_block)
            server(tokens, current_server);
        else if (in_location_block)
            location(tokens, current_location);
        else if (in_error_page_block)
            errorPage(tokens, current_server);
        else
            continue;    
    }
    return (Config);
}


