#ifndef H_XD_SHADER_PROGRAM
#define H_XD_SHADER_PROGRAM

#include <xd/graphics.h>
#include <boost/noncopyable.hpp>
#include <string>
#include <map>

namespace xd
{
	class shader_program : boost::noncopyable
	{
	public:
		typedef std::map<std::string, GLuint> attrib_list;

		// constructors & destructors
		shader_program();
		shader_program(const char *vertex_shader_src, const char *fragment_shader_src);
		shader_program(const char *vertex_shader_src, const char *fragment_shader_src, const attrib_list& attribs);
		virtual ~shader_program();

		// load and free
		virtual void load(const char *vertex_shader_src, const char *fragment_shader_src);
		virtual void load(const char *vertex_shader_src, const char *fragment_shader_src, const attrib_list& attribs);
		virtual void free();

		// use it
		virtual void use();

		// bind attrib
		virtual void bind_attrib(const char *name, GLuint attr);

		// bind uniforms
		virtual void bind_uniform(const char *name, int val);
		virtual void bind_uniform(const char *name, float val);
		virtual void bind_uniform(const char *name, const glm::vec2& val);
		virtual void bind_uniform(const char *name, const glm::vec3& val);
		virtual void bind_uniform(const char *name, const glm::vec4& val);
		virtual void bind_uniform(const char *name, const glm::mat2& val);
		virtual void bind_uniform(const char *name, const glm::mat3& val);
		virtual void bind_uniform(const char *name, const glm::mat4& val);
	protected:
		// shader program and attrib list
		GLuint m_program;
		attrib_list m_attribs;

		// internal funcs
		void load_attribs();
		void prepare();

		// internal functions
		void build(const char *vertex_shader_src, const char *fragment_shader_src);
		GLuint compile(GLuint type, const char *src);
		GLint get_uniform_location(const char *name);
	};
}

#endif
