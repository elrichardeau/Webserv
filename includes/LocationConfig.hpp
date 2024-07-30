#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <iostream>

class LocationConfig
{
public:
    LocationConfig();
    LocationConfig(LocationConfig const &other);
    LocationConfig &operator=(LocationConfig const &other);
    ~LocationConfig();

    void setUploadDir(const std::string &dir);
    void setReturnDirective(const std::string &directive);
    void setPath(const std::string &p);
    void setIndex(const std::string &idx);
    void setAutoIndex(const std::string &autoidx);
    void setRoot(const std::string &rt);
    void setAllowMethods(const std::vector<std::string> &methods); 
    void addCgiExtension(const std::string &ext);
    void addCgiPath(const std::string &ext, const std::string &path);
    void addAllowMethod(const std::string &method);

    std::string getPath() const;
    std::vector<std::string> getAllowMethods() const;
    std::string getIndex() const;
    std::string getautoIndex() const;
    std::string getRoot() const;
    std::vector<std::string> getCgiExtensions() const;
    std::map<std::string, std::string> getCgiPaths() const;
    std::string getUploadDir() const;
    std::string getReturnDirective() const;

private:

    std::string path;
    std::vector<std::string> allowMethods;
    std::string index;
    std::string autoindex;
    std::string root;
    std::vector<std::string> cgi_extension;
    std::map<std::string, std::string> cgi_path;
    std::string upload_dir;
    std::string returnDirective;
};