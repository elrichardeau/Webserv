#include "../includes/Config.hpp"

Config::Config(const std::string &filename) 
: inServerBlock(false), inLocationBlock(false), inErrorPageBlock(false), hasReadData(false)
{
    readConfig(filename);
}
Config::~Config(){}


bool commonPorts(const std::vector<int> &ports1, const std::vector<int> &ports2)
{
    std::vector<int> modifiablePorts1 = ports1;
    std::vector<int> modifiablePorts2 = ports2;

    std::sort(modifiablePorts1.begin(), modifiablePorts1.end());
    std::sort(modifiablePorts2.begin(), modifiablePorts2.end());

    std::vector<int> commonPorts(modifiablePorts1.size());
    std::vector<int>::iterator it = std::set_intersection(modifiablePorts1.begin(), \
    modifiablePorts1.end(), modifiablePorts2.begin(), modifiablePorts2.end(), \
    commonPorts.begin());
    commonPorts.resize(it - commonPorts.begin()); 
    return (!commonPorts.empty());
}

bool Config::isUniqueServer(const ServerConfig &newServer)
{
    for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it) 
    {
        bool sameHost = (it->getHost() == newServer.getHost() || newServer.getHost() == "0.0.0.0" || it->getHost() == "0.0.0.0");
        bool samePorts = commonPorts(it->getPorts(), newServer.getPorts());
        if (samePorts && sameHost)
        {
            if (newServer.getServerName() == it->getServerName())
                return (false); 
        }
    }
    return (true);
}

void Config::addServer(const ServerConfig &server) 
{ 
    if (!isUniqueServer(server))
        throw InvalidConfig("Duplicate server configuration detected.");
    servers.push_back(server);
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

void Config::allowMethods(std::vector<std::string> &tokens, LocationConfig &currentLocation)
{
    std::vector<std::string> validMethods;
    validMethods.push_back("GET");
    validMethods.push_back("POST");
    validMethods.push_back("DELETE");
    validMethods.push_back("PUT");
    validMethods.push_back("HEAD");
    validMethods.push_back("OPTIONS");
    validMethods.push_back("PATCH");
    validMethods.push_back("CONNECT");
    validMethods.push_back("TRACE");
    if (tokens[0] == "allow_methods")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: no specified allow methods.");
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            if (std::find(validMethods.begin(), validMethods.end(), tokens[i]) != validMethods.end())
                currentLocation.addAllowMethod(tokens[i]);
            else
                throw InvalidConfig("Error: Invalid HTTP method specified.");
        }
    }
}
        

void Config::index(std::vector<std::string> &tokens, LocationConfig &currentLocation)
{
    if (tokens[0] == "index")
    {
        std::vector<std::string> idx;
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No index specified.");
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            if (tokens[i][0] != '/')
                throw InvalidConfig("Error: Index path must start with '/'.");
            idx.push_back(tokens[i]);
        }
        currentLocation.setIndex(idx);
    }
}


void Config::cgiExtensions(std::vector<std::string> &tokens, LocationConfig &currentLocation)
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
            currentLocation.addCgiExtension(extension);
        } 
    }
}

void Config::cgiPaths(std::vector<std::string> &tokens, LocationConfig &currentLocation)
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
            if (path[0] != '/')
                throw InvalidConfig("Error: CGI path must start with '/'.");
            if (std::find(validExtensions.begin(), validExtensions.end(), extension) == validExtensions.end())
                throw InvalidConfig("Error: Invalid CGI extension. Only '.php' and '.py' are allowed.");
            if (std::find(seenExtensions.begin(), seenExtensions.end(), extension) != seenExtensions.end())
                throw InvalidConfig("Error: Each CGI extension can only have one corresponding path.");
            seenExtensions.push_back(extension);
            currentLocation.addCgiPath(extension, path);
        }
    } 
}

void Config::uploadDir(std::vector<std::string> &tokens, LocationConfig &currentLocation)
{
    if (tokens[0] == "upload_dir")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No upload_dir specified.");
        currentLocation.setUploadDir(tokens[1]);
    }
}

void Config::autoIndex(std::vector<std::string> &tokens, LocationConfig &currentLocation)
{
    if (tokens[0] == "autoindex")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No autoindex specified.");
        if (tokens[1] == "on")
            currentLocation.setAutoIndex(true);
        else if (tokens[1] == "off")
            currentLocation.setAutoIndex(false);
        else 
            throw InvalidConfig("Error: Invalid autoindex.");
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
    }
    return (tokens);
}
void Config::root(std::vector<std::string> &tokens, LocationConfig &currentLocation)
{
    if (tokens[0] == "root")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No root specified.");
        const std::string &path = tokens[1];
        if (path[0] != '/')
            throw InvalidConfig("Error: Root path must start with '/'.");
        currentLocation.setRoot(tokens[1]);
    }
}

void Config::location(std::vector<std::string> &tokens, LocationConfig &currentLocation)
{
    if (tokens[0].empty())
        return;
    if (tokens[0] == "return")
        handleReturn(tokens, currentLocation);
    else
    {
        allowMethods(tokens, currentLocation);
        autoIndex(tokens, currentLocation);
        index(tokens, currentLocation);
        root(tokens, currentLocation);
        cgiExtensions(tokens, currentLocation);
        cgiPaths(tokens, currentLocation);
        uploadDir(tokens, currentLocation);
    }
}

void Config::listen(std::vector<std::string> &tokens, ServerConfig &currentServer)
{
    if (tokens[0] == "listen")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No port number specified.");
        std::vector<int> tempPort;
        for (size_t i = 1; i < tokens.size(); ++i)
        {
            char *buf;
            std::string start = tokens[i];
            long port = std::strtol(start.c_str(), &buf, 10);
            std::string end = buf;
            if (end != "\0" && port < 0 && port >  65535)
                throw InvalidConfig("Error: Invalid port number.");
            if (std::find(tempPort.begin(), tempPort.end(), port) != tempPort.end())
                throw InvalidConfig("Error: Duplicate port number.");
            tempPort.push_back(port);
        
        }  
        for (size_t i = 0; i < tempPort.size(); ++i)
            currentServer.addPort(static_cast <int> (tempPort[i]));
    }
}

void Config::host(std::vector<std::string> &tokens, ServerConfig &currentServer)
{
    if (tokens[0] == "host")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No host specified.");
        if (!isValidIPAddress(tokens[1]))
            throw InvalidConfig("Error: Invalid host.");
        currentServer.setHost(tokens[1]);  
    }
}

void Config::serverName(std::vector<std::string> &tokens, ServerConfig &currentServer)
{
    if (tokens[0] == "server_name")
    {
        if (tokens.size() != 2)
            throw InvalidConfig("Error: Invalid number of server_name."); 
        currentServer.setServerName(tokens[1]);
    }
}

void Config::body(std::vector<std::string> &tokens, ServerConfig &currentServer)
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
        currentServer.setClientMaxBodySize(static_cast<int> (bodyInt));
    }
}

void Config::root(std::vector<std::string> &tokens, ServerConfig &currentServer)
{
    if (tokens[0] == "root")
    {
        if (tokens.size() < 2)
            throw InvalidConfig("Error: No root specified.");
        const std::string &path = tokens[1];
        if (path[0] != '/')
            throw InvalidConfig("Error: Root path must start with '/'.");
        currentServer.setRoot(tokens[1]);
    }
}

void Config::server(std::vector<std::string> &tokens, ServerConfig &currentServer)
{
    listen(tokens, currentServer);
    host(tokens, currentServer);
    serverName(tokens, currentServer);
    body(tokens, currentServer);
    root(tokens, currentServer);
}

void Config::errorPage(std::vector<std::string> &tokens, ServerConfig &currentServer)
{
    if (tokens.size() < 2)
        throw InvalidConfig("Error: Insufficient parameters from error page configuration.");
    int errorCode = std::atoi(tokens[0].c_str());
    const std::string &errorPath = tokens[1];
    if (errorPath[0] != '/')
        throw InvalidConfig("Error: Error page path must start with '/'.");
    currentServer.addErrorPage(errorCode, errorPath);
}

void Config::inBlocks(bool &inLocationBlock, bool &inServerBlock, bool &inErrorPageBlock, ServerConfig &currentServer, LocationConfig &currentLocation)
{
    if (inLocationBlock)
    {
        finalizeLocation(currentLocation);
        currentServer.addLocation(currentLocation);
        inLocationBlock = false;
    } 
    else if (inErrorPageBlock)
        inErrorPageBlock = false;
    else if (inServerBlock)
    {
        this->addServer(currentServer);
        inServerBlock = false;
    }
}


void Config::configLocation(bool &inLocationBlock, LocationConfig &currentLocation, std::string line)
{
    inLocationBlock = true;
    currentLocation = LocationConfig();
    size_t pathStart = line.find_first_not_of(" ", 8);
    size_t pathEnd = line.find_last_not_of(" ", line.find("{") - 1);
    if (pathEnd != std::string::npos && pathStart != std::string::npos && pathEnd >= pathStart)
    {
        std::string path = line.substr(pathStart, pathEnd - pathStart + 1);
        if (path[0] != '/')
            throw InvalidConfig("Error: Location path must start with '/'.");
        currentLocation.setPath(line.substr(pathStart, pathEnd - pathStart + 1));
    }    
    else
        throw InvalidConfig("Error: Wrong path in Location.");
}


bool isValidineEnding(const std::string &line)
{
    size_t lastNonSpace = line.find_last_not_of(" \t\n\r");
    if (lastNonSpace == std::string::npos)
        return (true);
    char lastChar = line[lastNonSpace];
    if (lastChar == ';')
    {
        size_t prevNonSpace = line.find_last_not_of(" \t\n\r", lastNonSpace - 1);
        if (prevNonSpace != std::string::npos)
        {
            char prevChar = line[prevNonSpace];
            if (prevChar == '{' || prevChar == '}')
                return (false);
        }
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
    validDirectives.push_back("}");
    validDirectives.push_back("{");

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

void Config::handleReturn(std::vector<std::string> &tokens, LocationConfig &currentLocation)
{
    if (tokens[0] == "return")
    {
        if (tokens.size() < 3)
            throw InvalidConfig("Error: Incomplete return directive.");
        char *end;
        long statusCode = std::strtol(tokens[1].c_str(), &end, 10);
        if ((statusCode < 300 || statusCode > 310) || *end != '\0')
            throw InvalidConfig("Error: Invalid return code.");
        const std::string &path = tokens[2];
        if (path[0] != '/')
            throw InvalidConfig("Error: Return code path must start with '/'.");
        std::string directive = tokens[1] + " " + tokens[2];
        currentLocation.setReturnDirective(directive);
    }
}

void Config::finalizeLocation(LocationConfig &currentLocation)
{
    if (currentLocation.getAllowMethods().empty())
    {
        std::vector<std::string> defaultMethods;
        defaultMethods.push_back("GET");
        defaultMethods.push_back("HEAD");
        currentLocation.setAllowMethods(defaultMethods);
    }
    if (currentLocation.getRoot().empty())
        currentLocation.setRoot("/");
}

void Config::processLine(ServerConfig &currentServer, LocationConfig &currentLocation)
{
    if (line == "server {")
    {
        if (inServerBlock)
            throw InvalidConfig("Error: 'server' block is incorrectly configured.");
        inServerBlock = true;
        currentServer = ServerConfig();
    }
    else if (line == "}")
    {
        if (inServerBlock && !currentServer.isValid())
            throw InvalidConfig("Error: Missing required directives (root, host, or listen).");
        if (!inServerBlock && !inLocationBlock && !inErrorPageBlock)
            throw InvalidConfig("Error: Wrong configuration.");
        inBlocks(inLocationBlock, inServerBlock, inErrorPageBlock, currentServer, currentLocation);
    }
    else if (line.find("location") == 0 && line[line.length() - 1] == '{')
    {
        if (!inServerBlock || inLocationBlock || inErrorPageBlock)
            throw InvalidConfig("Error: A block is not closed.");
        configLocation(inLocationBlock, currentLocation, line);
    }
    else if (line == "error_page {")
    {
        if (!inServerBlock || inLocationBlock || inErrorPageBlock)
            throw InvalidConfig("Error: A block is not closed.");
        inErrorPageBlock = true;
    }
    else if (inServerBlock && !inLocationBlock && !inErrorPageBlock)
        server(tokens, currentServer);
    else if (inLocationBlock)
        location(tokens, currentLocation);
    else if (inErrorPageBlock)
        errorPage(tokens, currentServer);
    else
        throw InvalidConfig("Error: Invalid line.");   
}

void Config::readConfig(const std::string &filename)
{
    if (access(filename.c_str(), R_OK) == -1)
        throw InvalidConfig("Error: Access denied or file does not exist.");
    std::ifstream file(filename.c_str());
    if (!file.is_open())
        throw InvalidConfig("Error: couldn't open file.");
    ServerConfig currentServer;
    LocationConfig currentLocation;
    while (std::getline(file, line))
    {
        hasReadData = true;
        tokens = Config::split(line, ' ');
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);
        if (line.empty())
            continue;
        checkLine(validDirectives, tokens, line);
        processLine(currentServer, currentLocation);
    }
    file.close();
    if (!hasReadData)
        throw InvalidConfig("Error: Wrong configuration file.");
}



