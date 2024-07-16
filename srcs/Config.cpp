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
}

std::vector<ServerConfig> Config::getServers() const 
{ 
	return (servers); 
}

std::vector<std::string> Config::split(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter))
    {
        if (!token.empty())
            tokens.push_back(token);
    }
    return (tokens);
}

void Config::location(std::vector<std::string> tokens, std::string line, LocationConfig current_location)
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

void Config::server(std::vector<std::string> tokens, std::string line, ServerConfig current_server)
{
    tokens = Config::split(line, ' ');
    if (tokens[0] == "listen")
    {
        char *end;
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            const char *start = tokens[i].c_str();
            long port = std::strtol(start, &end, 10);
            if (start != end && *end == '\0' && port >= 0 && port <=  65535)
            {
                current_server.addPort(static_cast <int> (port));
            }   
            else 
	        {
                std::cerr << "Invalid port number: " << port << std::endl;
                std::exit(1);
	        }
        }
            
    }
    else if (tokens[0] == "host")
        current_server.setHost(tokens[1]);
    else if (tokens[0] == "server_name")
        current_server.setServerName(tokens[1]);
    else if (tokens[0] == "client_max_body_size")
        current_server.setClientMaxBodySize(std::atoi(tokens[1].c_str()));
    else if (tokens[0] == "root")
        current_server.setRoot(tokens[1]);
}

void Config::errorPage(std::vector<std::string> tokens, std::string line, ServerConfig current_server)
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
        //line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
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


