#ifndef H_XD_LUA_VIRTUAL_MACHINE
#define H_XD_LUA_VIRTUAL_MACHINE

#include <string>
#include <boost/noncopyable.hpp>
#include <xd/common.hpp>
#include <xd/lua/common.hpp>
#include <xd/lua/script.hpp>
#include <xd/lua/types.hpp>
#include <xd/lua/exceptions.hpp>
#include <lua.hpp>
#include <luabind/luabind.hpp>

namespace xd
{
	namespace lua
	{
		class XD_LUA_API virtual_machine : public boost::noncopyable
		{
		public:
			virtual_machine();
			virtual ~virtual_machine();

			lua_State *lua_state();
			function<void> load(const std::string& code);
			function<void> load_file(const std::string& filename);
			void exec(const std::string& code);
			void exec_file(const std::string& filename);
			
			void load_library(const std::string& module_name = "xd");

			luabind::object globals()
			{
				return luabind::globals(m_lua_state);
			}

			template <typename T>
			luabind::object globals(const T& key)
			{
				return luabind::globals(m_lua_state)[key];
			}

			luabind::object registry()
			{
				return luabind::registry(m_lua_state);
			}

			template <typename T>
			luabind::object registry(const T& key)
			{
				return luabind::registry(m_lua_state)[key];
			}

		private:
			lua_State *m_lua_state;
		};
	}
}

#endif
