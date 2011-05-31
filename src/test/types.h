#ifndef H_TRAITS
#define H_TRAITS

#include <xd/graphics.h>

struct my_vertex
{
	glm::vec2 pos;
	glm::vec4 color;
	glm::vec2 tex;
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

		// and RGBA color
		xd::vertex_attr_traits color;
		color.type = GL_FLOAT;
		color.size = 4;
		color.offset = sizeof(glm::vec2);
		color.normalized = GL_FALSE;

		// and texture UV map
		xd::vertex_attr_traits tex;
		tex.type = GL_FLOAT;
		tex.size = 2;
		tex.offset = color.offset + sizeof(glm::vec4);
		tex.normalized = GL_FALSE;

		// add the traits to the vertex
		add_attr_traits(xd::vertex_attr_position, pos);
		add_attr_traits(xd::vertex_attr_color, color);
		add_attr_traits(xd::vertex_attr_texture, tex);

		// set the size of the vertex
		vertex_size = sizeof(my_vertex);
	}
};

#endif
