#ifndef H_XD_SCRIPTING_SCRIPT_LUA_BASE
#define H_XD_SCRIPTING_SCRIPT_LUA_BASE

#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>

namespace xd
{
	class script_base : public boost::noncopyable
	{
	public:
		virtual ~script_base() = 0 {}
		
	};

	typedef boost::shared_ptr<script_base> script_handle;
}

#endif
