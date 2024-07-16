#include "../includes/ErrorPageConfig.hpp"

ErrorPageConfig::ErrorPageConfig(){}

ErrorPageConfig::ErrorPageConfig(ErrorPageConfig const &other)
{
	(void)other;
}

ErrorPageConfig &ErrorPageConfig::operator=(ErrorPageConfig const &other)
{
	(void)other;
	return (*this);
}

ErrorPageConfig::~ErrorPageConfig(){}

void ErrorPageConfig::setCode(int c)
{ 
	this->code = c;
}

void ErrorPageConfig::setPath(const std::string &p)
{ 
	this->path = p;
}

int ErrorPageConfig::getCode() const
{ 
	return (this->code);
}

std::string ErrorPageConfig::getPath() const
{ 
	return (this->path);
}