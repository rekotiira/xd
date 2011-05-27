#include <xd/lua/exceptions.h>

xd::lua::script_load_failed::script_load_failed(const std::string& message)
{
	const int size = sizeof(m_error_message)/sizeof(m_error_message[0])-1;
	strncpy(m_error_message, message.c_str(), size);
	m_error_message[size] = 0;
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
	return m_error_message;
}

xd::lua::panic_error::panic_error(const std::string& reason)
	: xd::exception("PANIC: "+reason)
{
}

xd::lua::panic_error::~panic_error()
{
}
