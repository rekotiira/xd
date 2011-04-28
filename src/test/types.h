#ifndef H_TRAITS
#define H_TRAITS

#include <xd/graphics.h>

struct my_vertex
{
	glm::vec2 pos;
	glm::vec4 color;
};

struct my_vertex_traits : public xd::vertex_traits
{
	my_vertex_traits()
	{
		// we have XY position
		xd::vertex_attr_traits pos;
		pos.type = GL_FLOAT;
		pos.size = 2;
		pos.offset = 0;
		pos.normalized = GL_FALSE;
		insert(std::make_pair(xd::vertex_attr_pos, pos));

		// and RGBA color
		xd::vertex_attr_traits color;
		color.type = GL_FLOAT;
		color.size = 4;
		color.offset = sizeof(glm::vec2);
		color.normalized = GL_FALSE;
		insert(std::make_pair(xd::vertex_attr_color, color));

		// set the size and draw mode
		vertex_size = sizeof(my_vertex);
	}
};

#endif
