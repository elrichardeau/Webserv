
#include "../includes/ErrorPage.hpp"

ErrorPage::ErrorPage(std::map<int,std::string> errorPages) {
	this->_errorPage = {
		{400, "/error/400.html"},
		{404, "/error/404.html"},
		{404, "/error/404.html"},
		{405, "/error/405.html"},
		{406, "/error/406.html"},
		{500, "/error/500.html"},
		{505, "/error/505.html"}
	};
	std::map<int, std::vector<std::string>> g;
}

ErrorPage::~ErrorPage() {}