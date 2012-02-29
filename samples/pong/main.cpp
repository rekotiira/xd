#include "pong.hpp"
#include <iostream>

int main()
{
	try {
		pong game;
		game.run();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}
