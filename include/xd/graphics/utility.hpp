#ifndef H_XD_GRAPHICS_UTILITY
#define H_XD_GRAPHICS_UTILITY

#include <xd/graphics.hpp>
#include <boost/config.hpp>
#include <type_traits>

#ifdef BOOST_NO_VARIADIC_TEMPLATES
#include <boost/preprocessor/iteration/iterate.hpp>
#endif

namespace xd
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
	template <typename V, typename S, typename... Args>
	void render(vertex_batch<V>& batch, S& shader, Args&&... args)
	{
		shader.setup(std::forward<Args>(args)...);
		batch.render();
	}
#else
	// generate convenience render functions
	// maximum of XD_MAX_ARITY parameters supported
	#define BOOST_PP_ITERATION_PARAMS_1 (3, (0, XD_MAX_ARITY, <xd/graphics/detail/iterate_render.hpp>))
	#include BOOST_PP_ITERATE()
#endif
}

#endif
