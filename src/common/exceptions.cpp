#include <xd/common/exception.h>

xd::exception::~exception()
{
}

const char* xd::exception::what() const
{
	return "xd::exception";
}