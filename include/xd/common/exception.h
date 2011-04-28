#ifndef H_XD_COMMON_EXCEPTION
#define H_XD_COMMON_EXCEPTION

#include <stdexcept>

namespace xd
{
	class exception : public std::exception
	{
	public:
		virtual ~exception();
		virtual const char* what() const throw();
	};
}

#endif
