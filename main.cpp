# include "Ft_Irc.hpp"
# include "Server.hpp"
bool running = 1;
static void signal_handler(int signal)
{
    if (signal == SIGINT)
		running = 0;
    if (signal == SIGTSTP)
        running = 0;
    if (signal == SIGQUIT)
        running = 1;
}

int main(int argc, char **argv)
{
    try
    {
        if (argc != 3)
            throw std::runtime_error("Not right numbers of arguments!");
        Server start("IRC");
        start.portAndPass(argv[1], argv[2]);
        signal(SIGINT, signal_handler);
        signal(SIGTSTP, signal_handler);
        start.creatingServer(start);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}