#include "../includes/LocationConfig.hpp"

LocationConfig::LocationConfig(){}

LocationConfig::LocationConfig(LocationConfig const &other)
{
	(void)other;
}
LocationConfig &LocationConfig::operator=(LocationConfig const &other)
{
	(void)other;
	return (*this);
}

LocationConfig::~LocationConfig()
{}

void LocationConfig::setPath(const std::string &p)
{ 
	this->path = p; 
}

void LocationConfig::addAllowMethod(const std::string &method)
{ 
	allow_methods.push_back(method); 
}

void LocationConfig::setIndex(const std::string &idx) 
{ 
	this->index = idx; 
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

std::string LocationConfig::getPath() const
{ 
	return (this->path); 
}

std::vector<std::string> LocationConfig::getAllowMethods() const
{
	return (this->allow_methods); 
}

std::string LocationConfig::getIndex() const
{ 
	return (this->index); 
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