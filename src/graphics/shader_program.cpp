#include <xd/vendor/glm/gtc/type_ptr.hpp>
#include <xd/graphics/shader_program.hpp>
#include <xd/graphics/exceptions.hpp>

xd::shader_program::shader_program()
{
	m_program = glCreateProgram();
}

xd::shader_program::~shader_program()
{
	glDeleteProgram(m_program);
}

void xd::shader_program::attach(GLuint type, const std::string& src)
{
	// compile the shader
	const char *srcp = src.c_str();
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &srcp, NULL);
	glCompileShader(shader);

	// check for errors
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char *buf = new char[length];
		glGetShaderInfoLog(shader, length, NULL, buf);
		std::string message(buf);
		delete[] buf;

		// delete the shader
		glDeleteShader(shader);

		// throw the exception
		throw xd::shader_build_failed(message);
	}

	// attach the shader to the program and delete it afterwards
	glAttachShader(m_program, shader);
	glDeleteShader(shader);
}

void xd::shader_program::link()
{
	// link the program
	glLinkProgram(m_program);

	// check for errors
	GLint status;
	glGetProgramiv(m_program, GL_LINK_STATUS, &status);
	if (!status) {
		int length;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
		char *buf = new char[length];
		glGetProgramInfoLog(m_program, length, NULL, buf);
		std::string message(buf);
		delete[] buf;

		// throw the exception
		throw xd::shader_build_failed(message);
	}
}

void xd::shader_program::use()
{
	glUseProgram(m_program);
}

void xd::shader_program::setup()
{
	use();
}

void xd::shader_program::bind_attrib(const std::string& name, GLuint attr)
{
	glBindAttribLocation(m_program, attr, name.c_str());
}

void xd::shader_program::bind_uniform(const std::string& name, int val)
{
	glUniform1i(get_uniform_location(name), val);
}

void xd::shader_program::bind_uniform(const std::string& name, float val)
{
	glUniform1f(get_uniform_location(name), val);
}

void xd::shader_program::bind_uniform(const std::string& name, const glm::vec2& val)
{
	glUniform2fv(get_uniform_location(name), 1, &val[0]);
}

void xd::shader_program::bind_uniform(const std::string& name, const glm::vec3& val)
{
	glUniform3fv(get_uniform_location(name), 1, &val[0]);
}

void xd::shader_program::bind_uniform(const std::string& name, const glm::vec4& val)
{
	glUniform4fv(get_uniform_location(name), 1, &val[0]);
}

void xd::shader_program::bind_uniform(const std::string& name, const glm::mat2& val)
{
	glUniformMatrix2fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(val));
}

void xd::shader_program::bind_uniform(const std::string& name, const glm::mat3& val)
{
	glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(val));
}

void xd::shader_program::bind_uniform(const std::string& name, const glm::mat4& val)
{
	glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(val));
}

GLint xd::shader_program::get_uniform_location(const std::string& name)
{
	GLint location = glGetUniformLocation(m_program, name.c_str());
	if (location == -1) {
		throw invalid_shader_uniform(name);
	}
	return location;
}
