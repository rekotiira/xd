#ifndef H_XD_GRAPHICS_UTILITY
#define H_XD_GRAPHICS_UTILITY

#include <xd/graphics.hpp>
#include <type_traits>

namespace xd
{
	template <typename V, typename S>
	void render(vertex_batch<V>& batch, S& shader)
	{
		shader.setup();
		batch.render();
	}

	template <typename V, typename S, typename T1>
	void render(vertex_batch<V>& batch, S& shader, const T1& p1)
	{
		shader.setup(p1);
		batch.render();
	}

	template <typename V, typename S, typename T1, typename T2>
	void render(vertex_batch<V>& batch, S& shader, const T1& p1, const T2& p2)
	{
		shader.setup(p1, p2);
		batch.render();
	}

	template <typename V, typename S, typename T1, typename T2, typename T3>
	void render(vertex_batch<V>& batch, S& shader, const T1& p1, const T2& p2, const T3& p3)
	{
		shader.setup(p1, p2, p3);
		batch.render();
	}

	template <typename V, typename S, typename T1, typename T2, typename T3, typename T4>
	void render(vertex_batch<V>& batch, S& shader, const T1& p1, const T2& p2, const T3& p3, const T4& p4)
	{
		shader.setup(p1, p2, p3, p4);
		batch.render();
	}
}

#endif
