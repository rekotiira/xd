#include <xd/factory.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <ctime>
#include <cmath>
#include "test.h"

void wave_decorator(xd::text_decorator& decorator, const xd::formatted_text& text, const xd::text_decorator_args& args)
{
	const float pi = 3.14159265f;
	float step = pi / 4;
	float height = 2;
	float start = (float)(clock()/1000 % 1000) / 1000.0f * 2*pi;
	for (xd::formatted_text::const_iterator i = text.begin(); i != text.end(); ++i) {
		float pos = sin(start)*height;
		decorator.push_position(xd::vec2(0, pos));
		//decorator.push_position(xd::vec2(0, int(pos > 0 ? pos + 0.5f : pos - 0.5f)));
		decorator.push_text(*i);
		decorator.pop_position();
		start += step;
	}
}

const float pi = 3.14159265f;
const float isometric = static_cast<float>(atan(sin(pi/4)) / pi * 180);
const float tile_size = 32;
const int map_size = 10;

test::test()
	: xd::window("3D Isometric Sample", 640, 480)
	, m_grass_batch(GL_QUADS)
	, m_edge_batch(GL_QUADS)
	, m_font("verdanab.ttf", 12)
	, m_grass_texture(xd::create<xd::texture>("grass.png"))
	, m_grass_edge_texture(xd::create<xd::texture>("grass_edge.png"))
	, m_edge_texture(xd::create<xd::texture>("edge.png"))
	, m_mask_texture(xd::create<xd::texture>("mask.png", GL_CLAMP, GL_CLAMP))
	, m_hor_animating(false)
	, m_ver_animating(false)
	, m_hor_angle(45.0f)
	, m_ver_angle(isometric)
	, m_hor_target_angle(45.0f)
	, m_ver_target_angle(isometric)
	, m_zoom(1.0f)
{
	// store pointer to model view for ease of access
	m_model_view = &m_geometry.model_view();

	// register variable and decorator
	m_text_formatter.register_decorator("wave", &wave_decorator);

	// enable multisampling, texturing and depth test
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_2D);

	// enable blending and alpha test
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0);

	// face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// clear color of window
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClearDepth(999.0f);

	// create a grass batch (a quad)
	{
		my_vertex data[4];

		data[0].pos = xd::vec3(-0.5f, 0.0f,  0.5f); data[0].tex = xd::vec2(0, 1);
		data[1].pos = xd::vec3( 0.5f, 0.0f,  0.5f); data[1].tex = xd::vec2(1, 1);
		data[2].pos = xd::vec3( 0.5f, 0.0f, -0.5f); data[2].tex = xd::vec2(1, 0);
		data[3].pos = xd::vec3(-0.5f, 0.0f, -0.5f); data[3].tex = xd::vec2(0, 0);

		m_grass_batch.load(&data[0], 4);
	}

	// create an edge batch (four quads)
	{
		my_vertex data[16];

		data[0].pos  = xd::vec3(-0.5f,  0.0f,  0.5f); data[0].tex  = xd::vec2(0, 0);
		data[1].pos  = xd::vec3(-0.5f,  0.0f, -0.5f); data[1].tex  = xd::vec2(1, 0);
		data[2].pos  = xd::vec3(-0.5f, -1.0f, -0.5f); data[2].tex  = xd::vec2(1, 1);
		data[3].pos  = xd::vec3(-0.5f, -1.0f,  0.5f); data[3].tex  = xd::vec2(0, 1);

		data[4].pos  = xd::vec3( 0.5f,  0.0f, -0.5f); data[4].tex  = xd::vec2(0, 0);
		data[5].pos  = xd::vec3( 0.5f,  0.0f,  0.5f); data[5].tex  = xd::vec2(1, 0);
		data[6].pos  = xd::vec3( 0.5f, -1.0f,  0.5f); data[6].tex  = xd::vec2(1, 1);
		data[7].pos  = xd::vec3( 0.5f, -1.0f, -0.5f); data[7].tex  = xd::vec2(0, 1);

		data[8].pos  = xd::vec3(-0.5f,  0.0f, -0.5f); data[8].tex  = xd::vec2(0, 0);
		data[9].pos  = xd::vec3( 0.5f,  0.0f, -0.5f); data[9].tex  = xd::vec2(1, 0);
		data[10].pos = xd::vec3( 0.5f, -1.0f, -0.5f); data[10].tex = xd::vec2(1, 1);
		data[11].pos = xd::vec3(-0.5f, -1.0f, -0.5f); data[11].tex = xd::vec2(0, 1);

		data[12].pos = xd::vec3( 0.5f,  0.0f,  0.5f); data[12].tex = xd::vec2(0, 0);
		data[13].pos = xd::vec3(-0.5f,  0.0f,  0.5f); data[13].tex = xd::vec2(1, 0);
		data[14].pos = xd::vec3(-0.5f, -1.0f,  0.5f); data[14].tex = xd::vec2(1, 1);
		data[15].pos = xd::vec3( 0.5f, -1.0f,  0.5f); data[15].tex = xd::vec2(0, 1);
		
		m_edge_batch.load(&data[0], 16);
	}

	// bind physical keys to virtual ones
	bind_key(xd::KEY_LEFT, "left");
	bind_key(xd::KEY_RIGHT, "right");
	bind_key(xd::KEY_A, "zoom_in");
	bind_key(xd::KEY_Z, "zoom_out");
	bind_key(xd::KEY_ESC, "quit");

	// register our frame update function for timed updates, target for 30 FPS
	register_tick_handler(boost::bind(&test::frame_update, this), 1000/60);
}

test::~test()
{
}

void test::draw_text(float x, float y, const std::string& text)
{
	xd::font_style style;
	style.color = xd::vec4(1, 1, 1, 1);
	style.shadow = xd::font_shadow(1, -1, xd::vec4(0, 0, 0, 1));

	m_model_view->push();
		m_model_view->translate(x, y, 0);
		m_model_view->scale(1, -1, 1);
		m_text_formatter.render(text, m_font, style, m_text_shader, m_geometry.mvp());
	m_model_view->pop();
}

void test::draw_tile(int x, int y)
{
	m_model_view->push();
		// translate to the center of the tile
		m_model_view->translate(static_cast<float>(x), 0.0f, static_cast<float>(-y));

		// draw the top tile
		xd::render(m_grass_batch, m_texture_shader, m_geometry.mvp(), m_grass_texture);

		// draw edges
		xd::render(m_edge_batch, m_texture_shader, m_geometry.mvp(), m_grass_edge_texture);
		for (int i = 0; i < 1; i++) {
			m_model_view->translate(0, -1, 0);
			xd::render(m_edge_batch, m_texture_shader, m_geometry.mvp(), m_edge_texture);
		}
		m_model_view->translate(0, -1, 0);
		xd::render(m_edge_batch, m_texture_mask_shader, m_geometry.mvp(), m_edge_texture, m_mask_texture);
	m_model_view->pop();
}

void test::frame_update()
{
	if (m_hor_animating) {
		if (m_hor_angle < m_hor_target_angle)
			m_hor_angle += 45.0f / 10.0f;
		else
			m_hor_angle -= 45.0f / 10.0f;
		if (fabs(m_hor_target_angle - m_hor_angle) < 1.0f) {
			m_hor_angle = m_hor_target_angle;
			m_hor_animating = false;
		}
	}

	if (m_ver_animating) {
		if (m_ver_angle < m_ver_target_angle)
			m_ver_angle += 5.0f / 10.0f;
		else
			m_ver_angle -= 5.0f / 10.0f;
		if (fabs(m_ver_target_angle - m_ver_angle) < 1.0f) {
			m_ver_angle = m_ver_target_angle;
			m_ver_animating = false;
		}
	}

	if (!m_hor_animating) {
		if (pressed("left")) {
			m_hor_animating = true;
			m_hor_target_angle -= 90.0f;
		}
		if (pressed("right")) {
			m_hor_animating = true;
			m_hor_target_angle += 90.0f;
		}
	}

	if (!m_ver_animating) {
		if (pressed("up")) {
			m_ver_animating = true;
			m_ver_target_angle += 5.0f;
		}
		if (pressed("down")) {
			m_ver_animating = true;
			m_ver_target_angle -= 5.0f;
		}
	}
		
	// zoom
	if (pressed("zoom_in") && m_zoom < 2.0f)
		m_zoom += 0.05f;
	if (pressed("zoom_out") && m_zoom > 0.5f)
		m_zoom -= 0.05f;
}

void test::run()
{
	// run our game loop
	while (true)
	{
		// process the events
		update();

		// check for window close and quit key
		if (closed() || triggered("quit")) {
			// break from the game loop
			break;
		}

		glViewport(0, 0, width(), height());

		// clear screen
		clear();

		// setup orthographic projection for isometric view
		m_geometry.projection().load(xd::ortho(-320.0f, 320.0f, -240.0f, 240.0f, -999.0f, 999.0f));

		// enable depth testing
		glEnable(GL_DEPTH_TEST);

		// translate and scale
		m_model_view->identity();

		// rotate to isometric view
		m_model_view->rotate(m_ver_angle, 1.0f, 0, 0.0f);
		m_model_view->rotate(m_hor_angle, 0.0f, 1.0f, 0.0f);

		// scale to tile size so we can deal with "tile units", take zoom into account
		float scale = tile_size * m_zoom;
		m_model_view->scale(scale, scale, scale);

		// translate to the middle of the map
		float middle = static_cast<float>(map_size/2.0f-0.5f);
		m_model_view->translate(-middle, 0, middle);

		for (int x = 0; x < map_size; x++) {
			for (int z = 0; z < map_size; z++) {
				draw_tile(x, z);
			}
		}

		// setup orthographic projection for text rendering
		m_geometry.projection().load(xd::ortho<float>(0, 640, 480, 0, -100, 100));
		m_model_view->identity();

		// disable depth testing and face culling
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		// create a sprite batch
		xd::sprite_batch sprite_batch;
		// draw a sprite that rotates around its center (texture, x, y, rotation, scale, color, anchor)
		sprite_batch.add(m_grass_texture, 320, 240, ticks()/1000.0f*45, 1, xd::vec4(1,1,1,1), xd::vec2(0.5,0.5));
		// draw a partly transparent sprite (texture, x, y, color)
		sprite_batch.add(m_grass_texture, 240, 240, xd::vec4(1,1,1,0.5));
		// draw a scaled sprite that is also red (texture, x, y, rotation, scale, color)
		sprite_batch.add(m_grass_texture, 400, 240, 0, 1.5, xd::vec4(1,0,0,1));
		// draw all of them at same time
		sprite_batch.draw(m_geometry.mvp());

		// alpha testing
		//glAlphaFunc(GL_GREATER, 0.5f);

		// draw text
		m_model_view->translate(50, 20, 0);
		m_model_view->rotate(10, 0, 0, 1);
		m_model_view->scale(1.2f);
		draw_text(10, 20, "{color=yellow}FPS:{/color} "+boost::lexical_cast<std::string>(fps()));
		m_model_view->translate(0, 20, 0);
		m_model_view->rotate(-15, 0, 0, 1);
		m_model_view->scale(0.9f);
		draw_text(10, 40, "{color=yellow}Rot X:{/color} "+boost::lexical_cast<std::string>(m_hor_angle));
		m_model_view->rotate(22, 0, 0, 1);
		m_model_view->scale(0.8f);
		draw_text(10, 60, "{color=yellow}Rot Y:{/color} "+boost::lexical_cast<std::string>(m_ver_angle));
		m_model_view->translate(0, 10, 0);
		m_model_view->rotate(-15, 0, 0, 1);
		m_model_view->scale(1.1f);
		draw_text(10, 80, "{color=yellow}Zoom:{/color} "+boost::lexical_cast<std::string>(m_zoom));

		swap();
	}
}
