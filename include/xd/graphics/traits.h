#ifndef H_XD_GRAPHICS_TRAITS
#define H_XD_GRAPHICS_TRAITS

#include <xd/common.h>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <string>

namespace xd
{
	struct vertex_attr_traits
	{
		GLint size;
		GLenum type;
		GLboolean normalized;
		GLint offset;
	};

	struct vertex_traits : public boost::unordered_map<GLuint, vertex_attr_traits>
	{
		GLsizei vertex_size;
	};
}

#endif
