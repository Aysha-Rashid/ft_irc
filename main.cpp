#include "Ft_Irc.hpp"
#include <vector>
#include <cctype>
#include <cstdlib>


void portAndPass(const std::string& port, char *password)
{
    for (std::string::const_iterator it = port.begin(); it != port.end(); ++it) {
        if (!std::isdigit(*it))
            throw std::runtime_error("Invalid Port");
    }
    char *end;
    int value = strtol(port.c_str(), &end, 10);
}


int main(int argc, char **argv)
{
    try
    {
        if (argc != 3)
            throw std::runtime_error("Not right numbers of arguments!");
        portAndPass(argv[1], argv[2]);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}
