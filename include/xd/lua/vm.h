#ifndef H_XD_LUA_VM
#define H_XD_LUA_VM

#include <string>
#include <boost/noncopyable.hpp>
#include <xd/lua/script.h>
#include <lua.hpp>

namespace xd
{
	namespace lua
	{
		class vm : public boost::noncopyable
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
