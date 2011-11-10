#ifndef H_TRAITS
#define H_TRAITS

#include <xd/graphics.hpp>

// our vertex holds position and texture data
struct my_vertex
{
	glm::vec3 pos;
	glm::vec2 tex;
};

struct my_vertex_traits : public xd::vertex_traits
{
	my_vertex_traits()
	{
		// we have XY position
		xd::vertex_attr_traits pos;
		pos.type = GL_FLOAT;
		pos.size = 3;
		pos.offset = 0;
		pos.normalized = GL_FALSE;

		// and texture UV map
		xd::vertex_attr_traits tex;
		tex.type = GL_FLOAT;
		tex.size = 2;
		tex.offset = sizeof(glm::vec3);
		tex.normalized = GL_FALSE;

		// add the traits to the vertex
		add_attr_traits(xd::VERTEX_ATTR_POSITION, pos);
		add_attr_traits(xd::VERTEX_ATTR_TEXTURE, tex);

		// set the size of the vertex
		vertex_size = sizeof(my_vertex);
	}
};

#endif
