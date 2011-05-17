#ifndef H_XD_LUA_SCRIPT
#define H_XD_LUA_SCRIPT

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>
#include <xd/scripting/script_base.h>
#include <lua.hpp>

namespace xd
{
	namespace lua
	{
		class vm;
		class script : public script_base
		{
		public:
			virtual ~script();

			void run();
		
		private:
			// private constructor to be used by vm
			script(lua_State *vm, int ref);

			// member vars
			lua_State *m_vm;
			int m_script_ref;

			// friends
			friend class vm;
		};

		typedef boost::shared_ptr<script> script_handle;
	}
}

#endif
