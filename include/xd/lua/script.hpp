#ifndef H_XD_LUA_SCRIPT
#define H_XD_LUA_SCRIPT

#include <boost/noncopyable.hpp>
#include <string>
#include <memory>
#include <xd/common.hpp>
#include <xd/ref_counted.hpp>
#include <xd/lua/common.hpp>
#include <lua.hpp>

#ifndef XD_STATIC
// disable warning about boost::noncopyable not being dll-exportable
#pragma warning(disable: 4275)
#endif
/*
namespace xd
{
	namespace lua
	{
		class vm;
		class XD_LUA_API script : public xd::ref_counted, public boost::noncopyable
		{
		public:
			typedef boost::intrusive_ptr<script> ptr;

			virtual ~script();
			void run();
		
		private:
			// private constructor to be used by vm
			script(vm& vm, int ref);

			// member vars
			vm& m_vm;
			int m_script_ref;

			// friends
			friend class vm;
		};
	}
}
*/
#endif
