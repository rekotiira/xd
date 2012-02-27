#if !BOOST_PP_IS_ITERATING
#	error Do not include factory_call.hpp directly
#endif

#include <utility>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>

#define N BOOST_PP_ITERATION()
#define FORWARD(z, n, data) BOOST_PP_COMMA_IF(n) std::forward<T ## n>(p ## n)

template <typename R BOOST_PP_ENUM_TRAILING_PARAMS(N, typename T)>
static typename R::ptr create(BOOST_PP_ENUM_BINARY_PARAMS(N, T, && p))
{
	return R::ptr(new R(BOOST_PP_REPEAT(N, FORWARD,)));
}

#undef FORWARD
#undef N
