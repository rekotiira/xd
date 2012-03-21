#ifndef H_TEST
#define H_TEST

#include <xd/system.hpp>
#include <xd/graphics.hpp>
#include "types.h"

class test : public xd::window
{
public:
	// cstr & dstr
	test();
	~test();

	// run the game
	void run();

	// process timed update
	void frame_update();

	// draws text
	void draw_text(float x, float y, const std::string& text);
	void draw_tile(int x, int y);
private:
	// vertex batch type using our traits
	typedef xd::vertex_batch<my_vertex_traits> my_vertex_batch;

	// geometry transformation
	xd::transform_geometry m_geometry;
	xd::matrix_stack* m_model_view;

	// shaders for rendering textured quads and text
	xd::texture_shader m_texture_shader;
	xd::texture_mask_shader m_texture_mask_shader;
	xd::text_shader m_text_shader;

	// vertex batch for grass and edges
	my_vertex_batch m_grass_batch, m_edge_batch;

	// textures
	xd::texture::handle m_grass_texture;
	xd::texture::handle m_grass_edge_texture;
	xd::texture::handle m_edge_texture;
	xd::texture::handle m_mask_texture;

	// text rendering stuff
	xd::stock_text_formatter m_text_formatter;
	xd::font m_font;

	// game variables
	bool m_hor_animating;
	bool m_ver_animating;
	float m_hor_angle;
	float m_ver_angle;
	float m_hor_target_angle;
	float m_ver_target_angle;
	float m_zoom;
};

#endif
