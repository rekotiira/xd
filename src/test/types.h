#ifndef H_TRAITS
#define H_TRAITS

#include <xd/graphics.hpp>

// our vertex holds position and texture data
struct my_vertex
{
	glm::vec3 pos;
	glm::vec2 tex;
};

struct my_vertex_traits : xd::vertex_traits<my_vertex>
{
	my_vertex_traits()
	{
		// bind vertex attribs
		bind_vertex_attribute(xd::VERTEX_POSITION, &my_vertex::pos);
		bind_vertex_attribute(xd::VERTEX_TEXTURE, &my_vertex::tex);
	}
};

#endif
