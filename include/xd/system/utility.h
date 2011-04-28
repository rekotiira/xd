#ifndef H_XD_SYSTEM_UTILITY
#define H_XD_SYSTEM_UTILITY

#include <xd/system/types.h>

namespace xd
{
	inline key keyb(key_code code)
	{
		key k;
		k.type = xd::keyboard;
		k.code = code;
		return k;
	}

	inline key mouseb(key_code code)
	{
		key k;
		k.type = xd::mouse;
		k.code = code;
		return k;
	}
};

#endif
