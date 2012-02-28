#ifndef H_XD_GRAPHICS_DETAIL_VERTEX_TRAITS
#define H_XD_GRAPHICS_DETAIL_VERTEX_TRAITS

#include <xd/vendor/glew/glew.h>
#include <xd/vendor/glm/glm.hpp>
#include <type_traits>

namespace xd
{
	namespace detail
	{
		struct vertex_attr_traits
		{
			GLint size;
			GLenum type;
			const GLvoid *offset;
			GLboolean normalized;
			GLsizei stride;
		};

		template <typename T>
		struct vertex_attr_trait_type
		{
			enum { value = vertex_attr_trait_type<typename T::value_type>::value };
		};

		template <>
		struct vertex_attr_trait_type<GLbyte>
		{
			enum { value = GL_BYTE };
		};

		template <>
		struct vertex_attr_trait_type<GLubyte>
		{
			enum { value = GL_UNSIGNED_BYTE };
		};

		template <>
		struct vertex_attr_trait_type<GLshort>
		{
			enum { value = GL_SHORT };
		};

		template <>
		struct vertex_attr_trait_type<GLushort>
		{
			enum { value = GL_UNSIGNED_SHORT };
		};

		template <>
		struct vertex_attr_trait_type<GLint>
		{
			enum { value = GL_INT };
		};

		template <>
		struct vertex_attr_trait_type<GLuint>
		{
			enum { value = GL_UNSIGNED_INT };
		};

		template <>
		struct vertex_attr_trait_type<GLdouble>
		{
			enum { value = GL_DOUBLE };
		};

		template <>
		struct vertex_attr_trait_type<GLfloat>
		{
			enum { value = GL_FLOAT };
		};

		template <typename T, typename Enable = void>
		struct vertex_attr_trait_size;

		template <typename T>
		struct vertex_attr_trait_size<T, typename std::enable_if<std::is_arithmetic<T>::value>::type>
		{
			enum { value = 1 };
		};

		template <typename T>
		struct vertex_attr_trait_size<glm::detail::tvec1<T>>
		{
			enum { value = 1 };
		};

		template <typename T>
		struct vertex_attr_trait_size<glm::detail::tvec2<T>>
		{
			enum { value = 2 };
		};

		template <typename T>
		struct vertex_attr_trait_size<glm::detail::tvec3<T>>
		{
			enum { value = 3 };
		};

		template <typename T>
		struct vertex_attr_trait_size<glm::detail::tvec4<T>>
		{
			enum { value = 4 };
		};
	}
}

#endif
