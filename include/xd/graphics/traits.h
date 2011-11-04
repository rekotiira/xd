#ifndef H_XD_GRAPHICS_TRAITS
#define H_XD_GRAPHICS_TRAITS

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <xd/common.h>
#include <boost/noncopyable.hpp>
#include <unordered_map>
#include <string>
#include <utility>

namespace xd
{
	struct vertex_attr_traits
	{
		GLint size;
		GLenum type;
		GLboolean normalized;
		GLint offset;
	};

	struct vertex_traits : public std::unordered_map<GLuint, vertex_attr_traits>
	{
		void add_attr_traits(GLuint attr, const vertex_attr_traits& traits)
		{
			insert(std::make_pair(attr, traits));
		}

		GLsizei vertex_size;
	};
}

#endif
