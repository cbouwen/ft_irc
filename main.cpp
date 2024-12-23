#include "./server/Server.hpp"

int	main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
		return 1;
	}
	
	Server	server;
	server.setPassword(argv[2]);
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

//irssi -c localhost -p 4848 -w test
