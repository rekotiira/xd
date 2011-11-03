#ifndef H_XD_COMMON
#define H_XD_COMMON

#ifdef XD_EXPORT
#	ifndef XD_STATIC
#	define XD_API __declspec(dllexport)
#	define XD_API_TEMPLATE
#	else
#	define XD_API extern
#	endif
#else
#	ifndef XD_STATIC
#	define XD_API __declspec(dllimport)
#	define XD_API_TEMPLATE extern
#	else
#	define XD_API
#	endif
#endif

#include <xd/common/types.h>
#include <xd/common/exception.h>
#include <xd/common/event_bus.h>

#endif
