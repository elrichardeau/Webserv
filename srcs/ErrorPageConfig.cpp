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


void ErrorPageConfig::setErrorPage(int code, const std::string &path)
{
	errorPages[code] = path;
}

std::map<int, std::string> ErrorPageConfig::getErrorPages() const
{
	return (this->errorPages);
}