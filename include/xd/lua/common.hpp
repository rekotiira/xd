#ifndef H_XD_LUA_COMMON
#define H_XD_LUA_COMMON

#if defined(_WIN32) || defined(__CYGWIN__)
	#ifdef XD_LUA_EXPORT
		#define LUABIND_BUILDING
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
