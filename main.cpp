#include "./server/Server.hpp"

int	main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Incorrect amount of arguments" << std::endl;
		return 1;
	}
	//check valid port number ft
	Server	server;
//	if (argv[1] == valid)
//		server.setPort(argv[1]); // is atoi allowed?
	
	try 
	{
		server.setPort(argv[1]);
		//These two handle signal interrupts. Do we want to include this?
		//Adding these for now because I am following a guide. Tbd.
		signal(SIGINT, Server::SignalHandler); // catches ctrl c
		signal(SIGQUIT, Server::SignalHandler); // catches ctrl /
		server.ServerInit();
	}	
	catch (const std::exception& e)
	{
		server.CloseFD();
		std::cerr << e.what() << std::endl;
	}
	std::cout << "Server closed. Thank you." << std::endl;
}
