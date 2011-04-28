#include <iostream>
#include <boost/bind.hpp>
#include "test.h"

test::test()
	: xd::window("test app")
	, m_triangle(GL_TRIANGLES)
	, m_quad(GL_QUADS)
{
	// setup projection
	m_geometry.projection().load(glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f));

	// create a triangle
	{
		my_vertex data[3];
		data[0].pos = glm::vec2(-1, -1);
		data[1].pos = glm::vec2(1, -1);
		data[2].pos = glm::vec2(0, 1);
		data[0].color = glm::vec4(1, 0, 0, 1);
		data[1].color = glm::vec4(0, 1, 0, 1);
		data[2].color = glm::vec4(0, 0, 1, 1);
		m_triangle.load(&data[0], 3);
	}

	// create a quad
	{
		my_vertex data[4];
		data[0].pos = glm::vec2(-1, -1);
		data[1].pos = glm::vec2(1, -1);
		data[2].pos = glm::vec2(1, 1);
		data[3].pos = glm::vec2(-1, 1);
		data[0].color = glm::vec4(1, 1, 1, 1);
		data[1].color = glm::vec4(1, 1, 1, 1);
		data[2].color = glm::vec4(1, 1, 1, 1);
		data[3].color = glm::vec4(1, 1, 1, 1);
		m_quad.load(&data[0], 4);
	}

	// bind physical keys to virtual ones
	bind_key(xd::keyb(xd::key_space), "fire");
	bind_key(xd::mouseb(xd::mouse_left), "fire");
	bind_key(xd::keyb(xd::key_esc), "quit");

	// test event callbacks
	bind_input_event("key_down", &test::on_key_down, this, xd::input_filter(xd::keyboard));
	bind_input_event("key_down", &test::on_fire, this, xd::input_filter("fire", xd::mod_lctrl|xd::mod_lshift));
}

test::~test()
{
}

bool test::on_fire(const xd::input_args& args)
{
	std::cout << "fire!" << std::endl;
	return false;
}

bool test::on_key_down(const xd::input_args& args)
{
	std::cout << "key down: " << args.physical_key.code;
	if (args.virtual_key) {
		std::cout << " (virtual: " << *args.virtual_key << ")";
	}
	std::cout << std::endl;
	return true;
}

void test::run()
{
	while (true)
	{
		// process the events
		update();

		// check for window close and quit key
		if (closed() || triggered("quit")) {
			// break from the game loop
			break;
		}

		if (triggered("fire", xd::mod_lctrl|xd::mod_lshift)) {
			std::cout << "fire" << std::endl;
		}

		// clear screen
		clear();

		// get model view stack
		xd::matrix_stack& model_view = m_geometry.model_view();

		// translate and scale
		model_view.identity();
		model_view.translate(400.0f, 300.0f, 0.0f);
		model_view.scale(200.0f, -150.0f, 0.0f);

		// draw the triangle with shaded shader (uses mvp matrix)
		model_view.push();
			model_view.scale(0.5f, 0.5f, 0.0f);
			model_view.translate(-1.5f, 0.0f, 0.0f);
			xd::draw(m_triangle, m_shaded_shader, m_geometry.mvp());
		model_view.pop();

		// draw the quad with flat shader (uses mvp matrix and a color)
		model_view.push();
			model_view.scale(0.5f, -0.5f, 0.0f);
			model_view.translate(1.5f, 0.0f, 0.0f);
			xd::draw(m_quad, m_flat_shader, m_geometry.mvp(), glm::vec4(1, 0, 0, 1));
		model_view.pop();

		swap();
	}
}
