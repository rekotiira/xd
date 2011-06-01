#ifndef H_XD_GRAPHICS_UTILITY
#define H_XD_GRAPHICS_UTILITY

#include <xd/graphics/vertex_batch.h>
#include <xd/graphics/shader_program.h>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/mpl/assert.hpp>

namespace xd
{
	template <typename V, typename S>
	void render(vertex_batch<V>& batch, S& shader)
	{
		BOOST_MPL_ASSERT((boost::is_base_of<shader_program, S>));
		shader.setup();
		batch.render();
	}

	template <typename V, typename S, typename T1>
	void render(vertex_batch<V>& batch, S& shader, const T1& p1)
	{
		BOOST_MPL_ASSERT((boost::is_base_of<shader_program, S>));
		shader.setup(p1);
		batch.render();
	}

	template <typename V, typename S, typename T1, typename T2>
	void render(vertex_batch<V>& batch, S& shader, const T1& p1, const T2& p2)
	{
		BOOST_MPL_ASSERT((boost::is_base_of<shader_program, S>));
		shader.setup(p1, p2);
		batch.render();
	}

	template <typename V, typename S, typename T1, typename T2, typename T3>
	void render(vertex_batch<V>& batch, S& shader, const T1& p1, const T2& p2, const T3& p3)
	{
		BOOST_MPL_ASSERT((boost::is_base_of<shader_program, S>));
		shader.setup(p1, p2, p3);
		batch.render();
	}

	template <typename V, typename S, typename T1, typename T2, typename T3, typename T4>
	void render(vertex_batch<V>& batch, S& shader, const T1& p1, const T2& p2, const T3& p3, const T4& p4)
	{
		BOOST_MPL_ASSERT((boost::is_base_of<shader_program, S>));
		shader.setup(p1, p2, p3, p4);
		batch.render();
	}
}

#endif
