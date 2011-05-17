#ifndef H_XD_COMMON_EXCEPTION
#define H_XD_COMMON_EXCEPTION

#include <stdexcept>
#include <string>

namespace xd
{
	class exception : public std::runtime_error
	{
	public:
		exception() : std::runtime_error("xd::exception") {}
		exception(const std::string& what) : std::runtime_error(what) {}
		virtual ~exception() {}
	};
}

#endif
