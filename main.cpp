#include "./server/Server.hpp"

int	main(int argc, char** argv)
{
	Server	server;
	try 
	{
		//These two handle signal interrupts. Do we want to include this?
		//Adding these for now because I am following a guide. Tbd.
		signal(SIGINT, Server::SignalHandler); // catches ctrl c
		signal(SIGQUIT, Server::SignalHandler); // catches ctrl /
		server.ServerInit();
	}	
	catch (const std::exception& e)
	{
		server.closeFD();
		std::cerr << e.what() << std::endl;
	}
	std::cout << "Server closed. Thank you." << std::endl;
}
