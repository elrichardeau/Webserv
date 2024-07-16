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
	path = p; 
}

void LocationConfig::addAllowMethod(const std::string &method)
{ 
	allow_methods.push_back(method); 
}

void LocationConfig::setIndex(const std::string &idx) 
{ 
	index = idx; 
}

void LocationConfig::setRoot(const std::string &rt)
{ 
	root = rt; 
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
	upload_dir = dir;
}

std::string LocationConfig::getPath() const
{ 
	return path; 
}

std::vector<std::string> LocationConfig::getAllowMethods() const
{
	return allow_methods; 
}

std::string LocationConfig::getIndex() const
{ 
	return index; 
}

std::string LocationConfig::getRoot() const
{ 
	return root;
}

std::vector<std::string> LocationConfig::getCgiExtensions() const
{ 
	return cgi_extension;
}

std::map<std::string, std::string> LocationConfig::getCgiPaths() const
{ 
	return cgi_path;
}

std::string LocationConfig::getUploadDir() const
{ 
	return upload_dir;
}