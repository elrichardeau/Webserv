
#include "../includes/ErrorPage.hpp"

void createVector(std::vector<std::string> &tmp, const std::string &path, const std::string &message) {
	tmp.clear();
	tmp.push_back(path);
	tmp.push_back(message);
}

ErrorPage::ErrorPage() {}

ErrorPage::ErrorPage(std::map<int, std::string> errorPages) : _protocol("HTTP/1.1"), _contentType("text/html") {
	std::vector<std::string> tmp;
	createVector(tmp, "./pages/error/400.html", "Bad Request");
	this->_errorPage.insert(std::make_pair(400, tmp));
	createVector(tmp, "./pages/error/403.html", "Forbidden");
	this->_errorPage.insert(std::make_pair(403, tmp));
	createVector(tmp, "./pages/error/404.html", "Not found");
	this->_errorPage.insert(std::make_pair(404, tmp));
	createVector(tmp, "./pages/error/405.html", "Method Not Allowed");
	this->_errorPage.insert(std::make_pair(405, tmp));
	createVector(tmp, "./pages/error/406.html", "Not Acceptable");
	this->_errorPage.insert(std::make_pair(406, tmp));
	createVector(tmp, "./pages/error/500.html", "Internal Server Error");
	this->_errorPage.insert(std::make_pair(500, tmp));
	createVector(tmp, "./pages/error/501.html", "Not Implemented");
	this->_errorPage.insert(std::make_pair(501, tmp));
	createVector(tmp, "./pages/error/505.html", "Version not supported");
	this->_errorPage.insert(std::make_pair(505, tmp));
	for (MapIterator it = errorPages.begin(); it != errorPages.end(); it++) {
		if (!access(("." + it->second).c_str(), F_OK | R_OK)) {
			setNewPath(it->first, ("." + it->second));
		}
	}
}

ErrorPage& ErrorPage::operator=(const ErrorPage &copy) {
	if (this == &copy)
        return *this;
	this->_errorPage = copy._errorPage;
	this->_protocol = copy._protocol;
	this->_contentType = copy._contentType;
	return *this;
}

ErrorPage::~ErrorPage() {}
void ErrorPage::setNewPath(int statusCode, const std::string &newPath) {this->_errorPage[statusCode][0] = newPath;}
std::string ErrorPage::getPath(int statusCode) {return this->_errorPage[statusCode][0];}
std::string ErrorPage::getErrorMessage(int statusCode) {return this->_errorPage[statusCode][1];}
std::string ErrorPage::getProtocol() const {return this->_protocol;}
std::string ErrorPage::getContentType() const {return this->_contentType;}