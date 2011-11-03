#ifndef H_XD_LUA_VM
#define H_XD_LUA_VM

#include <string>
#include <boost/noncopyable.hpp>
#include <xd/common.h>
#include <xd/lua/common.h>
#include <xd/lua/script.h>
#include <lua.hpp>

#ifndef XD_STATIC
// disable warning about boost::noncopyable not being dll-exportable
// as well as the private members that can't be accessed by client
#pragma warning(disable: 4275)
#endif

namespace xd
{
	namespace lua
	{
		class XD_LUA_API vm : public boost::noncopyable
		{
		public:
			vm();
			~vm();

			lua_State *get_vm();

			script_handle load(const std::string& code);
			script_handle load_file(const std::string& filename);

			void load_library(const std::string& module_name = "xd");

		private:
			lua_State *m_vm;
		};
	}
}

#endif
