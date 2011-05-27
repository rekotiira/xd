#ifndef H_TEST
#define H_TEST

#include <xd/system.h>
#include <xd/graphics.h>
#include <xd/graphics/font.h>
#include <xd/graphics/stock_text_formatter.h>
#include "types.h"

class test : public xd::window
{
public:
	test();
	~test();
	void run();
	bool on_fire(const xd::input_args& args);
	bool on_key_down(const xd::input_args& args);
	void draw_text(float x, float y, const std::string& text,
		const glm::vec4& color = glm::vec4(1, 1, 1, 1), std::string type = "");
private:
	xd::transform_geometry m_geometry;
	xd::flat_shader m_flat_shader;
	xd::shaded_shader m_shaded_shader;
	xd::text_shader m_text_shader;
	xd::vertex_batch<my_vertex_traits> m_triangle, m_quad;
	xd::stock_text_formatter m_text_formatter;
	xd::font m_font;
};

#endif
