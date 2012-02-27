#ifndef H_XD_FACTORY
#define H_XD_FACTORY

namespace xd
{
	class factory
	{
	public:
		// generate xd::factory::create functions with file iteration (up to 10 overloads)
		#define BOOST_PP_ITERATION_PARAMS_1 (3, (0, 10, <xd/detail/iterate_factory_create.hpp>))
		#include BOOST_PP_ITERATE()
	};

	// generate convenience xd::create functions that proxy the call to xd::factory::create
	#define BOOST_PP_ITERATION_PARAMS_1 (3, (0, 10, <xd/detail/iterate_create.hpp>))
	#include BOOST_PP_ITERATE()
}

#endif
