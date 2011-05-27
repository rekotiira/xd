#include <iostream>
#include <boost/bind.hpp>
#include "test.h"

std::string my_variable(const std::string& variable)
{
	return "00:23";
}

test::test()
	: xd::window("test app")
	, m_triangle(GL_TRIANGLES)
	, m_quad(GL_QUADS)
	, m_font("verdana.ttf", 24)
{
	// setup projection
	m_geometry.projection().load(glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f));

	// register variable and decorator
	m_text_formatter.register_variable("time", &my_variable);
	//m_text_formatter.register_decorator("c", &my_color_formatter);
	//m_text_formatter.register_decorator("b", &my_bold_formatter);

	// link bold
	m_font.link_font("bold", xd::font_ptr(new xd::font("verdanab.ttf", 24)));

	// enable texturing
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.5f, 0.5f, 0.7f, 1);

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
	bind_key(xd::key_space, "fire");
	bind_key(xd::mouse_left, "fire");
	bind_key(xd::key_esc, "quit");

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

void test::draw_text(float x, float y, const std::string& text, const glm::vec4& color, std::string type)
{
	xd::font_style style;
	style.type = type;
	style.color = color;
	//style.shadow = xd::font_shadow(2, -2, glm::vec4(0, 0, 0, 1));

	xd::matrix_stack& model_view = m_geometry.model_view();
	model_view.push();
		model_view.translate(x, y, 0);
		model_view.scale(1, -1, 1);
		m_text_formatter.render(text, m_font, style, m_text_shader, m_geometry.mvp());
	model_view.pop();
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

		/*if (triggered("fire", xd::mod_lctrl|xd::mod_lshift)) {
			std::cout << "fire" << std::endl;
		}*/

		// clear screen
		clear();

		// disable blending
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);

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

		// enable blending
		glEnable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// draw text
		model_view.identity();
		draw_text(20, 40, "You can now draw {bold}decorated text{/bold}");
		draw_text(20, 70, "You can even use {color=red}different{/color} {color=100,200,100}colors{/color}");
		draw_text(20, 100, "{bold}{color=green}Nested{/color} tags{/bold} are supported");
		draw_text(20, 130, "{shadow}How do you like 'em {bold}{color=red}shadows{/color}{/bold}?{/shadow}");

		model_view.translate(600, 50, 0);
		model_view.rotate(45, 0, 0, 1);
		draw_text(0, 0, "{shadow}{bold}Rotated text!{/bold}{/shadow}");

		swap();
	}
}
