#ifndef H_XD_CONFIG
#define H_XD_CONFIG

// XD_MAX_ARITY specifies how many overloads to create for
// forwarders such as xd::create
// If the arity is set to 10 (default), it'll generate overloads
// that at maximum take 10 parameters
#ifndef XD_MAX_ARITY
#define XD_MAX_ARITY 10
#endif

// specifies how to export/import
// classes/functions from the library
#if defined(_WIN32) || defined(__CYGWIN__)
	#ifdef XD_EXPORT
		#ifdef XD_STATIC
			#define XD_API extern
		#else
			#define XD_API __declspec(dllexport)
		#endif
		#define XD_API_TEMPLATE
	#else
		#ifdef XD_STATIC
			#define XD_API
		#else
			#define XD_API __declspec(dllimport)
		#endif
		#define XD_API_TEMPLATE extern
	#endif
#else
	#ifdef XD_EXPORT
		#define XD_API extern
		#define XD_API_TEMPLATE
	#else
		#define XD_API
		#define XD_API_TEMPLATE extern
	#endif
#endif

#endif
