#if !BOOST_PP_IS_ITERATING
#	error Do not include factory_call.hpp directly
#endif

#include <utility>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>

#define XD_N BOOST_PP_ITERATION()
#define XD_FORWARD(Z, N, D) std::forward<BOOST_PP_CAT(T, N)>(BOOST_PP_CAT(p, N))

template <typename R BOOST_PP_ENUM_TRAILING_PARAMS(XD_N, typename T)>
typename R::ptr create(BOOST_PP_ENUM_BINARY_PARAMS(XD_N, T, && p))
{
	return xd::factory::create<R>(BOOST_PP_ENUM(XD_N, XD_FORWARD, _));
}

#undef XD_FORWARD
#undef XD_N
