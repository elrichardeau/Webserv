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

bool isValidIPAddress(const std::string& ip)
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
        std::cout << "Split token: " << token << std::endl;
    }
    return (tokens);
}

void Config::location(std::vector<std::string> &tokens, std::string line, LocationConfig &current_location)
{
    tokens = Config::split(line, ' ');
    if (tokens[0] == "allow_methods")
    {
        for (size_t i = 1; i < tokens.size(); ++i)
            current_location.addAllowMethod(tokens[i]);    
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
        char *buf;
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            std::string start = tokens[i];
            long port = std::strtol(start.c_str(), &buf, 10);
            std::string end = buf;
            if (end == "\0" && port >= 0 && port <=  65535)
                current_server.addPort(static_cast <int> (port));
            else 
	        {
                std::cerr << "Invalid port number" << std::endl;
                exit(1);
	        }
        }  
    }
    else if (tokens[0] == "host")
    {
        if (!isValidIPAddress(tokens[1]))
        {
            std::cerr << "Host invalid" << std::endl;
            exit(1);
        }
        current_server.setHost(tokens[1]);  
    }
    else if (tokens[0] == "server_name")
        current_server.setServerName(tokens[1]);
    else if (tokens[0] == "client_max_body_size")
    {
        char *end;
        std::string body = tokens[1];
        long bodyInt = std::strtol(body.c_str(), &end, 10);
        if (*end != '\0' || bodyInt < 0)
        {
            std::cerr << "Invalid client max body" << std::endl;
            exit(1);
        }
        current_server.setClientMaxBodySize(std::atoi(tokens[1].c_str()));
    }
    //else if (tokens[0] == "root")
    //     current_server.setRoot(tokens[1]); 
}

void Config::errorPage(std::vector<std::string> &tokens, std::string line, ServerConfig &current_server)
{
    tokens = Config::split(line, ' ');
    ErrorPageConfig error_page;
    error_page.setCode(std::atoi(tokens[0].c_str()));
    error_page.setPath(tokens[1]);
    current_server.addErrorPage(error_page);
}

Config Config::readConfig(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
        throw Openfile();

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
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue;
        if (line == "server {") 
        {
            in_server_block = true;
            current_server = ServerConfig();
        } 
        else if (line == "}") 
        {
            if (in_location_block) 
            {
                current_server.addLocation(current_location);
                in_location_block = false;
            } 
            else if (in_server_block) 
            {
                Config.addServer(current_server);
                std::cout << "Server added" << std::endl;
                in_server_block = false;
            }
        } 
        else if (line.substr(0, 8) == "location") 
        {
            in_location_block = true;
            current_location = LocationConfig();
            current_location.setPath(line.substr(9, line.length() - 9));
        }
		else if (line == "error_page {")
            in_error_page_block = true;
		else if (in_server_block && !in_location_block && !in_error_page_block)
            server(tokens, line, current_server);
        else if (in_location_block)
            location(tokens, line, current_location);
		else if (in_error_page_block)
            errorPage(tokens, line, current_server);
    }
    return (Config);
}


