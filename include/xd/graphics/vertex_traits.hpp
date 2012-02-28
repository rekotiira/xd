#ifndef H_XD_GRAPHICS_TRAITS
#define H_XD_GRAPHICS_TRAITS

#include <xd/graphics/detail/vertex_traits.hpp>

#include <xd/config.hpp>
#include <xd/vendor/glew/glew.h>
#include <unordered_map>
#include <type_traits>

namespace xd
{
	template <typename Vertex, int Size = sizeof(Vertex)>
	class vertex_traits
	{
	public:
		enum { vertex_size = Size };

		void bind_attr_traits(GLuint attr, GLenum type, GLint size, const GLvoid *offset, GLboolean normalized = GL_FALSE, GLsizei stride = 0)
		{
			detail::vertex_attr_traits traits;
			traits.type = type;
			traits.size = size;
			traits.offset = offset;
			traits.normalized = normalized;
			traits.stride = stride;
			m_attr_traits.insert(std::make_pair(attr, traits));
		}

		template <typename T, typename R>
		void bind_attr_traits(GLuint attr, R T::*ptr, GLboolean normalized = GL_FALSE)
		{
			// make sure the types are related
			static_assert(std::is_same<T, Vertex>::value, "you can't bind a member of unrelated class");
			// automatically deduce the type, value, offset and stride
			bind_attr_traits(attr, detail::vertex_attr_trait_type<R>::value, detail::vertex_attr_trait_size<R>::value,
				reinterpret_cast<const GLvoid*>(&(static_cast<T*>(nullptr)->*ptr)), normalized, vertex_size);
		}
	
	private:
		template <typename> friend class vertex_batch;
		std::unordered_map<GLuint, detail::vertex_attr_traits> m_attr_traits;
	};
}

#endif
