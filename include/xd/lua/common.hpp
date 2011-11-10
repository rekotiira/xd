#ifndef H_XD_LUA_COMMON
#define H_XD_LUA_COMMON

#ifdef XD_LUA_EXPORT
#	ifndef XD_STATIC
#	define XD_LUA_API __declspec(dllexport)
#	else
#	define XD_LUA_API
#	endif
#else
#	ifndef XD_STATIC
#	define XD_LUA_API __declspec(dllimport)
#	else
#	define XD_LUA_API
#	endif
#endif

#endif

