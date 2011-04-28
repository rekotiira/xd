#include <iostream>
#include <xd/common.h>
#include "test.h"

int main(int argc, char *argv[])
{
	try {
		test my_app;
		my_app.run();
	} catch (std::exception e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
