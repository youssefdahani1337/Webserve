#include "../include/Manager.hpp"

int	main(int argc, char **argv)
{
	std::string				configFileName;

	if (argc != 1 && argc != 2)
		std::cerr << "Error: args" << std::endl;
	else
	{
		try
		{
			signal(SIGPIPE, Tools::sighandler);
			if (argc == 1)
				configFileName = "conf/WebServ.conf";
			else
				configFileName = argv[1];
		    Manager manager(configFileName);
		    manager.initServers();
        
        }
		catch (std::exception const &e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
}