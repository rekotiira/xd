#include <xd/graphics.h>

xd::shader_program::shader_program()
	: m_program(0)
{
}

xd::shader_program::shader_program(const char *vertexShader, const char *fragmentShader)
	: m_program(0)
{
	load(vertexShader, fragmentShader);
}

xd::shader_program::shader_program(const char *vertexShader, const char *fragmentShader, const attrib_list& attribs)
	: m_program(0)
{
	load(vertexShader, fragmentShader, attribs);
}

xd::shader_program::~shader_program()
{
	free();
}

void xd::shader_program::load(const char *vertex_shader_src, const char *fragment_shader_src)
{
	free();
	build(vertex_shader_src, fragment_shader_src);
}

void xd::shader_program::load(const char *vertex_shader_src, const char *fragment_shader_src, const attrib_list& attribs)
{
	free();
	m_attribs = attribs;
	build(vertex_shader_src, fragment_shader_src);
}

void xd::shader_program::free()
{
	if (m_program) {
		m_attribs.clear();
		glDeleteProgram(m_program);
		m_program = 0;
	}
}

void xd::shader_program::use()
{
	prepare();
}

void xd::shader_program::bind_attrib(const char *name, GLuint attr)
{
	m_attribs[name] = attr;
}

void xd::shader_program::bind_uniform(const char *name, int val)
{
	glUniform1i(get_uniform_location(name), val);
}

void xd::shader_program::bind_uniform(const char *name, float val)
{
	glUniform1f(get_uniform_location(name), val);
}

void xd::shader_program::bind_uniform(const char *name, const glm::vec2& val)
{
	glUniform2fv(get_uniform_location(name), 1, &val[0]);
}

void xd::shader_program::bind_uniform(const char *name, const glm::vec3& val)
{
	glUniform3fv(get_uniform_location(name), 1, &val[0]);
}

void xd::shader_program::bind_uniform(const char *name, const glm::vec4& val)
{
	glUniform4fv(get_uniform_location(name), 1, &val[0]);
}

void xd::shader_program::bind_uniform(const char *name, const glm::mat2& val)
{
	glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(val));
}

void xd::shader_program::bind_uniform(const char *name, const glm::mat3& val)
{
	glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(val));
}

void xd::shader_program::bind_uniform(const char *name, const glm::mat4& val)
{
	glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(val));
}

void xd::shader_program::build(const char *vertex_shader_src, const char *fragment_shader_src)
{
	// create a program
	m_program = glCreateProgram();

	try {
		// compile vertex shader
		GLuint vertex_shader = compile(GL_VERTEX_SHADER, vertex_shader_src);
		glAttachShader(m_program, vertex_shader);
		glDeleteShader(vertex_shader);

		// compile fragment shader
		GLuint fragment_shader = compile(GL_FRAGMENT_SHADER, fragment_shader_src);
		glAttachShader(m_program, fragment_shader);
		glDeleteShader(fragment_shader);

		// load bound attributes
		load_attribs();

		// link the program
		glLinkProgram(m_program);

		GLint status;
		glGetShaderiv(m_program, GL_LINK_STATUS, &status);
		if (!status) {
			throw std::runtime_error("failed to link program");
		}
	} catch (...) {
		free();
	}
}

GLuint xd::shader_program::compile(GLuint type, const char *src)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glDeleteShader(shader);
		throw std::runtime_error("failed to compile vertex shader");
	}
	return shader;
}

void xd::shader_program::load_attribs()
{
	for (attrib_list::iterator i = m_attribs.begin(); i != m_attribs.end(); ++i) {
		glBindAttribLocation(m_program, i->second, i->first.c_str());
	}
}

void xd::shader_program::prepare()
{
	glUseProgram(m_program);
}

GLint xd::shader_program::get_uniform_location(const char *name)
{
	GLint location = glGetUniformLocation(m_program, name);
	if (location == -1) {
		throw std::runtime_error("invalid uniform");
	}
	return location;
}
