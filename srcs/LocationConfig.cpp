#include "../includes/LocationConfig.hpp"

LocationConfig::LocationConfig() : autoindex(false)
{}


LocationConfig::LocationConfig(LocationConfig const &other) : path(other.path), allowMethods(other.allowMethods), index(other.index),
  autoindex(other.autoindex), root(other.root), cgi_extension(other.cgi_extension),
  cgi_path(other.cgi_path), upload_dir(other.upload_dir), returnDirective(other.returnDirective)
{}

LocationConfig &LocationConfig::operator=(LocationConfig const &other)
{
	if (this != &other)
    {
        path = other.path;
        allowMethods = other.allowMethods;
        index = other.index;
        autoindex = other.autoindex;
        root = other.root;
        cgi_extension = other.cgi_extension;
        cgi_path = other.cgi_path;
        upload_dir = other.upload_dir;
		returnDirective = other.returnDirective;
    }
    return *this;
}

LocationConfig::~LocationConfig()
{}

void LocationConfig::setPath(const std::string &p)
{ 
	this->path = p; 
}

void LocationConfig::setAllowMethods(const std::vector<std::string> &methods)
{
	allowMethods = methods;
}

void LocationConfig::addAllowMethod(const std::string &method)
{ 
	allowMethods.push_back(method);	
}

void LocationConfig::setIndex(std::vector<std::string> &idx) 
{ 
	this->index = idx;
}


void LocationConfig::setAutoIndex(bool autoidx) 
{ 
	this->autoindex = autoidx; 
}

void LocationConfig::setRoot(const std::string &rt)
{ 
	this->root = rt;
}

void LocationConfig::addCgiExtension(const std::string &ext)
{ 
	cgi_extension.push_back(ext);
}

void LocationConfig::addCgiPath(const std::string &ext, const std::string &path)
{ 
	cgi_path[ext] = path;
}

void LocationConfig::setUploadDir(const std::string &dir)
{
	this->upload_dir = dir;
}

void LocationConfig::setReturnDirective(const std::string &directive)
{
	this->returnDirective = directive;
}


std::string LocationConfig::getPath() const
{ 
	return (this->path); 
}

std::vector<std::string> LocationConfig::getAllowMethods() const
{
	return (this->allowMethods);
}

std::vector<std::string> LocationConfig::getIndex() const
{
	return (this->index); 
}

bool LocationConfig::getautoIndex() const
{
	return (this->autoindex);
}

std::string LocationConfig::getRoot() const
{ 
	return (this->root);
}

std::vector<std::string> LocationConfig::getCgiExtensions() const
{ 
	return (this->cgi_extension);
}

std::map<std::string, std::string> LocationConfig::getCgiPaths() const
{ 
	return (this->cgi_path);
}

std::string LocationConfig::getUploadDir() const
{ 
	return (this->upload_dir);
}

std::string LocationConfig::getReturnDirective() const
{
	return (this->returnDirective);
}

