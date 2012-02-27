#if !BOOST_PP_IS_ITERATING
#	error Do not include factory_call.hpp directly
#endif

#include <utility>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>

#define N BOOST_PP_ITERATION()

// we pass 0 as a flag for non-void function
#if BOOST_PP_ITERATION_FLAGS() == 0
	#if N > 0
	template <BOOST_PP_ENUM_PARAMS(N, typename T)>
	#endif
	T operator()(BOOST_PP_ENUM_BINARY_PARAMS(N, T, const& p))
	{
		luabind::object ret = m_func(BOOST_PP_ENUM_PARAMS(N, p));
		if (m_default && luabind::type(ret) == LUA_TNIL)
			return *m_default;
		return luabind::object_cast<T>(ret);
	}
#else
	#if N > 0
	template <BOOST_PP_ENUM_PARAMS(N, typename T)>
	#endif
	void operator()(BOOST_PP_ENUM_BINARY_PARAMS(N, T, const& p))
	{
		m_func(BOOST_PP_ENUM_PARAMS(N, p));
	}
#endif

#undef N
