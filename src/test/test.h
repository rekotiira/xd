#ifndef H_TEST
#define H_TEST

#include <xd/system.h>
#include <xd/graphics.h>

#include "types.h"

class test : public xd::window
{
public:
	test();
	~test();
	void run();
	bool on_fire(const xd::input_args& args);
	bool on_key_down(const xd::input_args& args);
private:
	xd::transform_geometry m_geometry;
	xd::flat_shader m_flat_shader;
	xd::shaded_shader m_shaded_shader;
	xd::vertex_batch<my_vertex_traits> m_triangle, m_quad;
};

#endif
