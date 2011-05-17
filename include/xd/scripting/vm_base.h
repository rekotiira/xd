#ifndef H_XD_SCRIPTING_VM_BASE
#define H_XD_SCRIPTING_VM_BASE

#include <string>
#include <boost/noncopyable.hpp>
#include <xd/scripting/script_base.h>

namespace xd
{
	class vm_base : public boost::noncopyable
	{
	public:
		virtual ~vm_base() {}
		virtual script_handle load(const std::string& filename) = 0;
	};
}

#endif
