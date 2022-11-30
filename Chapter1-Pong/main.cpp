#include <exception>
#include <iostream>
#include "Game.hpp"

int main()
{
	try
	{
		Game pong;

		pong.Run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();
	}

	return 0;
}