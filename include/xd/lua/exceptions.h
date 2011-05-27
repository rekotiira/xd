#ifndef H_XD_LUA_EXCEPTIONS
#define H_XD_LUA_EXCEPTIONS

#include <xd/common/exception.h>
#include <string>

namespace xd
{
	namespace lua
	{
		struct script_load_failed : exception
		{
			script_load_failed(const std::string& message)
				: exception("failed to load script: " + message)
			{
			}
		};

		struct panic_error : exception
		{
			panic_error(const std::string& message)
				: exception("lua panic: " + message)
			{
			}
		};
	}
}

#endif

