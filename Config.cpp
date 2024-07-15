#include "Config.hpp"

std::map<std::string, std::string> parseSection(std::ifstream &file)
{
    std::map<std::string, std::string> section;
    std::string line;
    while (std::getline(file, line) && !line.empty())
    {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line[0] == '[')
            break; // Fin de section
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos)
        {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);
            section[key] = value;
        }
    }
    return section;
}

Config readConfig(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Could not open config file");

    std::string line;
    Config config;
    while (std::getline(file, line))
    {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty() || line[0] == '#' || line[0] == ';')
            continue; // Ignorer les lignes vides et les commentaires
        if (line[0] == '[')
        {
            std::string sectionName = line.substr(1, line.find(']') - 1);
            std::map<std::string, std::string> section = parseSection(file);

            if (sectionName == "server")
            {
                config.server.port = std::stoi(section["port"]);
                config.server.host = section["host"];
                config.server.root_directory = section["root_directory"];
                config.server.default_file = section["default_file"];
                config.server.error_page_404 = section["error_page_404"];
            } 
            else if (sectionName == "log")
            {
                config.log.log_file = section["log_file"];
                config.log.log_level = section["log_level"];
            }
        }
    }

    return config;
}
