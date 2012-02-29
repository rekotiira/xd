#ifndef H_XD_LUA_CONFIG
#define H_XD_LUA_CONFIG

#include <xd/config.hpp>

// XD_LUA_MAX_ARITY specifies how many overloads to create for
// functions such as xd::lua::scheduler::yield or xd::lua::function::operator()
// If the arity is set to 10 (default), it'll generate overloads
// that at maximum take 10 parameters
#ifndef XD_LUA_MAX_ARITY
#define XD_LUA_MAX_ARITY XD_MAX_ARITY
#endif

// specifies how to export/import
// classes/functions from the library
#if defined(_WIN32) || defined(__CYGWIN__)
	#ifdef XD_LUA_EXPORT
		#ifdef XD_STATIC
			#define XD_LUA_API extern
		#else
			#define XD_LUA_API __declspec(dllexport)
		#endif
	#else
		#ifdef XD_STATIC
			#define XD_LUA_API
		#else
			#define XD_LUA_API __declspec(dllimport)
		#endif
	#endif
#else
	#ifdef XD_LUA_EXPORT
		#define XD_LUA_API extern
	#else
		#define XD_LUA_API
	#endif
#endif

#endif
