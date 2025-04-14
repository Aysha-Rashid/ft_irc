# include "Utils.hpp"
# include <algorithm>


void trim(std::string &str)
{
    str.erase(0,str.find_first_not_of("\t\n\v\f\r"));
    str.erase(str.find_last_not_of("\t\n\v\f\r") + 1);
}

std::vector<std::string> split(const std::string &str, int separator)
{
    std::vector<std::string> tokens;
    std::string tok = "";

    for(size_t i =0; i < str.length(); i++)
    {
        char ch = str[i];
        if(ch == separator)
        {
            trim(tok);
            tokens.push_back(tok);
            tok = "";
        }
        else
            tok += ch;
    }
    if (tok.length() > 0)
    {
        trim(tok);
        tokens.push_back(tok);
    }
    return (tokens);     
}

size_t stringToNumber(const std::string &str)
{
    size_t number = 0;

    for(size_t i = 0; i < str.length(); i++)
    {
        if(isdigit(str[i]))
            number = number * 10 + str[i] - '0';
    }
    return(number);
}

void checkError(int result, const char *error, const std::string &errmeg)
{
	if (result < 0)
	{
		perror(error);
		throw std::runtime_error(errmeg);
	}
}
