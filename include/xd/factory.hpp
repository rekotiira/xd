#ifndef H_XD_FACTORY
#define H_XD_FACTORY

#include <xd/config.hpp>
#include <boost/config.hpp>

#ifdef BOOST_NO_VARIADIC_TEMPLATES
#include <boost/preprocessor/iteration/iterate.hpp>
#endif

namespace xd
{
	class factory
	{
	public:
#ifndef BOOST_NO_VARIADIC_TEMPLATES
		template <typename R, typename... Args>
		static typename R::handle create(Args&&... args)
		{
			return R::handle(new R(std::forward<Args>(args)...));
		}
#else
		// generate xd::factory::create functions with file iteration (up to XD_MAX_ARITY overloads)
		#define BOOST_PP_ITERATION_PARAMS_1 (3, (0, XD_MAX_ARITY, <xd/detail/iterate_factory_create.hpp>))
		#include BOOST_PP_ITERATE()
#endif
	};

#ifndef BOOST_NO_VARIADIC_TEMPLATES
	template <typename R, typename... Args>
	typename R::handle create(Args&&... args)
	{
		return xd::factory::create<R>(std::forward<Args>(args)...);
	}
#else
	// generate convenience xd::create functions that proxy the call to xd::factory::create
	#define BOOST_PP_ITERATION_PARAMS_1 (3, (0, XD_MAX_ARITY, <xd/detail/iterate_create.hpp>))
	#include BOOST_PP_ITERATE()
#endif
}

#endif
