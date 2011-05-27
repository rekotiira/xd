#include <xd/graphics/exceptions.h>

xd::shader_build_failed::shader_build_failed()
{
}

xd::shader_build_failed::~shader_build_failed()
{
}

const char* xd::shader_build_failed::what() const
{
	return "xd::lua::shader_build_failed";
}

const char* xd::shader_build_failed::error_message() const
{
	return m_error_message;
}
