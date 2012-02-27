#ifndef H_XD_CONFIG
#define H_XD_CONFIG

// XD_MAX_ARITY specifies how many overloads to create for
// forwarders such as xd::create or xd::lua::scheduler::yield
// If the arity is set to 10 (default), it'll generate overloads
// that at maximum take 10 parameters
#ifndef XD_MAX_ARITY
#define XD_MAX_ARITY 10
#endif

#endif
