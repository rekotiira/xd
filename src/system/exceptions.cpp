#include <xd/system/exceptions.h>

xd::window_creation_failed::window_creation_failed()
{
}

xd::window_creation_failed::~window_creation_failed()
{
}

const char* xd::window_creation_failed::what() const
{
	return "xd::window_creation_failed";
}