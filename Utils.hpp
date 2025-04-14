# ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <vector>

std::vector<std::string> split(const std::string &str, int separator);
void trim(std::string &str);
size_t stringToNumber(const std::string &str);
void checkError(int result, const char *error, const std::string &errmeg);

# endif