#ifndef H_XD_GRAPHICS_SHADER_PROGRAM
#define H_XD_GRAPHICS_SHADER_PROGRAM

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <xd/common.h>

#pragma warning(disable: 4275)

namespace xd
{
	class XD_API shader_program : boost::noncopyable
	{
	public:
		// constructors & destructors
		shader_program();
		virtual ~shader_program();

		// attach shader and link
		void attach(GLuint type, const std::string& src);
		void link();

		// use it
		virtual void use();
		virtual void setup();

		// bind attrib
		virtual void bind_attrib(const std::string& name, GLuint attr);

		// bind uniforms
		virtual void bind_uniform(const std::string& name, int val);
		virtual void bind_uniform(const std::string& name, float val);
		virtual void bind_uniform(const std::string& name, const glm::vec2& val);
		virtual void bind_uniform(const std::string& name, const glm::vec3& val);
		virtual void bind_uniform(const std::string& name, const glm::vec4& val);
		virtual void bind_uniform(const std::string& name, const glm::mat2& val);
		virtual void bind_uniform(const std::string& name, const glm::mat3& val);
		virtual void bind_uniform(const std::string& name, const glm::mat4& val);

		// get uniform location
		GLint get_uniform_location(const std::string& name);
	protected:
		// shader program and attrib list
		GLuint m_program;
	};
}

#endif
