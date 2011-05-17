#ifndef X_H_SYSTEM_EXCEPTIONS
#define X_H_SYSTEM_EXCEPTIONS

#include <xd/common/exception.h>

namespace xd
{
	class window_creation_failed : public xd::exception
	{
	public:
		window_creation_failed();
		virtual ~window_creation_failed();
		virtual const char *what() const;
	};
}

#endif

