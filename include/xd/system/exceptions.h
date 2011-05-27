#ifndef H_XD_SYSTEM_EXCEPTIONS
#define H_XD_SYSTEM_EXCEPTIONS

#include <xd/common/exception.h>

namespace xd
{
	struct window_creation_failed : exception
	{
		window_creation_failed()
			: exception("failed to create window")
		{
		}
	};
}

#endif

