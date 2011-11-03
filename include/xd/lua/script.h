#ifndef H_XD_LUA_SCRIPT
#define H_XD_LUA_SCRIPT

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>
#include <xd/common.h>
#include <xd/lua/common.h>
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
		class vm;
		class XD_LUA_API script : public boost::noncopyable
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
