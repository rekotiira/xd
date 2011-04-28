#include <xd/graphics/types.h>
#include <xd/graphics/shaders.h>

xd::flat_shader::flat_shader()
{
	static const char *vertexShader =
		"#version 330\n"
		"uniform mat4 mvpMatrix;"
		"in vec4 vVertex;"
		"void main(void)"
		"{"
		"	gl_Position = mvpMatrix * vVertex;"
		"}";

	static const char *fragmentShader =
		"#version 330\n"
		"out vec4 vFragColor;"
		"uniform vec4 vColor;"
		"void main(void)"
		"{"
		"	vFragColor = vColor;"
		"}";

	bind_attrib("vVertex", xd::vertex_attr_pos);
	load(vertexShader, fragmentShader);
}

void xd::flat_shader::use(const glm::mat4& mvp, const glm::vec4& color)
{
	prepare();
	bind_uniform("mvpMatrix", mvp);
	bind_uniform("vColor", color);
}

xd::shaded_shader::shaded_shader()
{
	static const char *vertexShader =
		"#version 330\n"
		"uniform mat4 mvpMatrix;"
		"in vec4 vVertex;"
		"in vec4 vColor;"
		"out vec4 vVaryingColor;"
		"void main(void)"
		"{"
		"	vVaryingColor = vColor;"
		"	gl_Position = mvpMatrix * vVertex;"
		"}";

	static const char *fragmentShader =
		"#version 330\n"
		"out vec4 vFragColor;"
		"in vec4 vVaryingColor;"
		"void main(void)"
		"{"
		"	vFragColor = vVaryingColor;"
		"}";

	bind_attrib("vVertex", xd::vertex_attr_pos);
	bind_attrib("vColor", xd::vertex_attr_color);
	load(vertexShader, fragmentShader);
}

void xd::shaded_shader::use(const glm::mat4& mvp)
{
	prepare();
	bind_uniform("mvpMatrix", mvp);
}