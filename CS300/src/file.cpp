#include "file.hpp"

#include <fstream>
#include <sstream>

auto loadFile(std::string_view path) -> std::string {
	std::fstream shader_file(std::string(path), std::ios::in);
	std::stringstream source_ss;
	source_ss << shader_file.rdbuf();
	std::string source = source_ss.str();
	return source;
}
