#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <boost/bind.hpp>
#include <ctime>
#include <cmath>
#include "test.h"

std::string current_time_variable(const std::string& variable)
{
	std::time_t current_time = std::time(0);
	std::tm *time_info = localtime(&current_time);
	std::ostringstream ss;
	ss << time_info->tm_hour << ":" << std::setw(2) << std::setfill('0') << time_info->tm_min;
	return ss.str();
}

void time_decorator(xd::text_decorator& decorator, const xd::formatted_text& text, const xd::text_decorator_args& args)
{
	float alpha = (std::time(0) % 2 == 0) ? 1.0f : 0.0f;
	std::istringstream is(text.get_unformatted());
	int hours, mins;
	char delim;
	is >> hours >> delim >> mins;
	decorator.push_text(hours);
	decorator.push_alpha(alpha);
	decorator.push_text(delim);
	decorator.pop_alpha();
	if (mins < 10)
		decorator.push_text(0);
	decorator.push_text(mins);
}

void sup_decorator(xd::text_decorator& decorator, const xd::formatted_text& text, const xd::text_decorator_args& args)
{
	decorator.push_type("small");
	decorator.push_position(glm::vec2(0, 6));
	decorator.push_text(text);
}

void wave_decorator(xd::text_decorator& decorator, const xd::formatted_text& text, const xd::text_decorator_args& args)
{
	const float pi = 3.14159265f;
	float step = pi / 4;
	float height = 2;
	float start = (float)(clock() % 1000) / 1000.0f * 2*pi;
	for (xd::formatted_text::const_iterator i = text.begin(); i != text.end(); ++i) {
		float pos = sin(start)*height;
		decorator.push_position(glm::vec2(0, pos));
		//decorator.push_position(glm::vec2(0, int(pos > 0 ? pos + 0.5f : pos - 0.5f)));
		decorator.push_text(*i);
		decorator.pop_position();
		start += step;
	}
}

test::test()
	: xd::window("test app", 500, 300)
	, m_triangle(GL_TRIANGLES)
	, m_quad(GL_QUADS)
	, m_font("verdana.ttf", 16)
{
	// setup projection
	m_geometry.projection().load(glm::ortho(0.0f, (float)width(), (float)height(), 0.0f, -1.0f, 1.0f));

	// register variable and decorator
	m_text_formatter.register_variable("current_time", &current_time_variable);
	m_text_formatter.register_decorator("time", &time_decorator);
	m_text_formatter.register_decorator("sup", &sup_decorator);
	m_text_formatter.register_decorator("wave", &wave_decorator);

	// link other styles
	m_font.link_font("bold", "verdanab.ttf");
	m_font.link_font("italic", "verdanai.ttf");
	m_font.link_font("jap", xd::font_ptr(new xd::font("jap.ttf", 32)));
	m_font.link_font("small", xd::font_ptr(new xd::font("verdana.ttf", 10)));
	m_font.link_font("big", xd::font_ptr(new xd::font("verdanab.ttf", 24)));

	// enable texturing
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

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
	std::ifstream file("input.txt");
	std::string line;
	std::getline(file, line);

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
		/*model_view.identity();
		model_view.translate(400.0f, 300.0f, 0.0f);
		model_view.scale(200.0f, -150.0f, 0.0f);

		// draw the triangle with shaded shader (uses mvp matrix)
		model_view.push();
			model_view.scale(0.5f, 0.5f, 0.0f);
			model_view.translate(-1.5f, 0.0f, 0.0f);
			xd::render(m_triangle, m_shaded_shader, m_geometry.mvp());
		model_view.pop();

		// draw the quad with flat shader (uses mvp matrix and a color)
		model_view.push();
			model_view.scale(0.5f, -0.5f, 0.0f);
			model_view.translate(1.5f, 0.0f, 0.0f);
			xd::render(m_quad, m_flat_shader, m_geometry.mvp(), glm::vec4(1, 0, 0, 1));
		model_view.pop();*/

		// enable blending
		glEnable(GL_BLEND);
		glEnable(GL_ALPHA_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glAlphaFunc(GL_GREATER, 0);

		// reset typewriters
		if (triggered(xd::key_1))
			m_text_formatter.reset_typewriter(0);
		if (triggered(xd::key_2))
			m_text_formatter.reset_typewriter(1);

		// draw text
		model_view.identity();
		draw_text(10, 20, "You can now draw {bold}decorated text{/bold}");
		draw_text(10, 50, "You can even use {color=red}different{/color} {color=100,200,100}colors{/color}");
		draw_text(10, 80, "{bold}{color=green}Nested{/color} tags{/bold} are supported");
		draw_text(10, 110, "{shadow}How do you like 'em {bold}{color=red}shadows{/color}{/bold}?{/shadow}");
		draw_text(10, 140, "You can use {italic}variables{/italic}, the time is: {shadow}{time}${current_time}{/time}{/shadow}");
		draw_text(10, 170, "{shadow}2{sup}4{/sup} = 16{/shadow}");
		draw_text(10, 200, "{shadow}It's a {rainbow}rainbow{/rainbow}{/shadow}");
		draw_text(10, 230, "{shadow}{wave}{typewriter}This text should appear one character at a time.{/typewriter}{/wave}{/shadow}");
		draw_text(10, 270, line);

		// rotate and draw
		model_view.translate(450, 10, 0);
		model_view.rotate(90, 0, 0, 1);
		draw_text(0, 0, "{typewriter=1,2}Some {color=purple}rotated{/color} text{/typewriter}");

		swap();
	}
}
