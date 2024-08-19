
#include "../includes/utils.hpp"

std::string itostr(int nb) {
	std::stringstream ss;
	std::string str;
	ss << nb;
	ss >> str;
	return str;
}

std::vector<std::string> split(std::string buf, const std::string &find) {
	std::vector<std::string> vector;
	if (buf.find(find) == std::string::npos) {
		vector.push_back(buf.substr(0, buf.size()));
		return vector;
	}
	for (int i = 0; !buf.empty(); i++) {
		vector.push_back(buf.substr(0, buf.find(find)));
		buf.erase(0, vector[i].size() + 1);
	}
	return vector;
}

std::string getPage(std::string filename) {
	std::ifstream fd(filename.c_str());
	std::string line;
	std::string response;
	while (getline(fd, line))
		response.append(line + "\n");
	fd.close();
	return response;
}