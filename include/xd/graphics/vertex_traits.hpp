#ifndef H_XD_GRAPHICS_TRAITS
#define H_XD_GRAPHICS_TRAITS

#include <xd/graphics/detail/vertex_traits.hpp>

#include <xd/config.hpp>
#include <xd/vendor/glew/glew.h>
#include <unordered_map>
#include <type_traits>

namespace xd
{
	enum vertex_attribute_index
	{
		VERTEX_POSITION = 0,
		VERTEX_COLOR,
		VERTEX_TEXTURE,
		VERTEX_EXTRA1,
		VERTEX_EXTRA2,
		VERTEX_CUSTOM
	};

	template <typename Vertex, int Size = sizeof(Vertex)>
	class vertex_traits
	{
	public:
		enum { vertex_size = Size };

		void bind_vertex_attribute(GLuint index, GLenum type, GLint size, const GLvoid *offset, GLboolean normalized = GL_FALSE, GLsizei stride = 0)
		{
			detail::vertex_attribute attrib;
			attrib.type = type;
			attrib.size = size;
			attrib.offset = offset;
			attrib.normalized = normalized;
			attrib.stride = stride;
			m_attribs.insert(std::make_pair(index, attrib));
		}

		template <typename T, typename R>
		void bind_vertex_attribute(GLuint index, R T::*ptr, GLboolean normalized = GL_FALSE)
		{
			// make sure the types are related
			static_assert(std::is_same<T, Vertex>::value, "you can't bind a member of unrelated class");
			// automatically deduce the type, value, offset and stride
			bind_vertex_attribute(index, detail::vertex_attribute_type<R>::value, detail::vertex_attribute_size<R>::value,
				reinterpret_cast<const GLvoid*>(&(static_cast<T*>(nullptr)->*ptr)), normalized, vertex_size);
		}
	
	private:
		template <typename> friend class vertex_batch;
		std::unordered_map<GLuint, detail::vertex_attribute> m_attribs;
	};
}

#endif
