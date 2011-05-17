#ifndef H_XD_LUA_EXCEPTIONS
#define H_XD_LUA_EXCEPTIONS

#include <xd/common/exception.h>
#include <string>

namespace xd
{
	namespace lua
	{
		class script_load_failed : public xd::exception
		{
		public:
			script_load_failed(const std::string& message);
			virtual ~script_load_failed();
			virtual const char *what() const;
			const char *error_message() const;
		private:
			std::string m_error_message;
		};

		class panic_error : public xd::exception
		{
		public:
			panic_error(const std::string& reason);
			virtual ~panic_error();
		};
	}
}

#endif

