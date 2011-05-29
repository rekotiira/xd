#include <xd/graphics/types.h>
#include <xd/graphics/shaders.h>

xd::flat_shader::flat_shader()
{
	static const char *vertex_shader_src =
		"#version 120\n"
		"uniform mat4 mvpMatrix;"
		"attribute vec4 vVertex;"
		"void main(void)"
		"{"
		"	gl_Position = mvpMatrix * vVertex;"
		"}";

	static const char *fragment_shader_src =
		"#version 120\n"
		"uniform vec4 vColor;"
		"void main(void)"
		"{"
		"	gl_FragColor = vColor;"
		"}";

	attach(GL_VERTEX_SHADER, vertex_shader_src);
	attach(GL_FRAGMENT_SHADER, fragment_shader_src);
	bind_attrib("vVertex", xd::vertex_attr_pos);
	link();
}

void xd::flat_shader::use(const glm::mat4& mvp, const glm::vec4& color)
{
	glUseProgram(m_program);
	bind_uniform("mvpMatrix", mvp);
	bind_uniform("vColor", color);
}

xd::shaded_shader::shaded_shader()
{
	static const char *vertex_shader_src =
		"#version 120\n"
		"uniform mat4 mvpMatrix;"
		"attribute vec4 vVertex;"
		"attribute vec4 vColor;"
		"varying vec4 vVaryingColor;"
		"void main(void)"
		"{"
		"	vVaryingColor = vColor;"
		"	gl_Position = mvpMatrix * vVertex;"
		"}";

	static const char *fragment_shader_src =
		"#version 120\n"
		"varying vec4 vVaryingColor;"
		"void main(void)"
		"{"
		"	gl_FragColor = vVaryingColor;"
		"}";

	attach(GL_VERTEX_SHADER, vertex_shader_src);
	attach(GL_FRAGMENT_SHADER, fragment_shader_src);
	bind_attrib("vVertex", xd::vertex_attr_pos);
	bind_attrib("vColor", xd::vertex_attr_color);
	link();
}

void xd::shaded_shader::use(const glm::mat4& mvp)
{
	glUseProgram(m_program);
	bind_uniform("mvpMatrix", mvp);
}

xd::text_shader::text_shader()
{
	static const char *vertex_shader_src =
		"#version 120\n"
		"uniform mat4 mvpMatrix;"
		"uniform vec2 vPosition;"
		"attribute vec4 vVertex;"
		"attribute vec2 vTexCoords;"
		"varying vec2 vVaryingTexCoords;"
		"void main(void)"
		"{"
		"	vVaryingTexCoords = vTexCoords;"
		"	gl_Position = mvpMatrix * (vVertex + vec4(vPosition.x, vPosition.y, 0, 0));"
		"}";

	static const char *fragment_shader_src =
		"#version 120\n"
		"uniform sampler2D colorMap;"
		"uniform vec4 vColor;"
		"varying vec2 vVaryingTexCoords;"
		"void main(void)"
		"{"
		"	gl_FragColor.rgb = vColor.rgb;"
		"	gl_FragColor.a = vColor.a * texture2D(colorMap, vVaryingTexCoords.st).r;"
		"}";

	attach(GL_VERTEX_SHADER, vertex_shader_src);
	attach(GL_FRAGMENT_SHADER, fragment_shader_src);
	bind_attrib("vVertex", xd::vertex_attr_pos);
	bind_attrib("vTexCoords", xd::vertex_attr_tex);
	link();
}
