#include <xd/lua/exceptions.h>

xd::lua::script_load_failed::script_load_failed(const std::string& message)
	: m_error_message(message)
{
}

xd::lua::script_load_failed::~script_load_failed()
{
}

const char* xd::lua::script_load_failed::what() const
{
	return "xd::lua::script_load_failed";
}

const char* xd::lua::script_load_failed::error_message() const
{
	return m_error_message.c_str();
}

xd::lua::panic_error::panic_error(const std::string& reason)
	: xd::exception("PANIC: "+reason)
{
}

xd::lua::panic_error::~panic_error()
{
}
