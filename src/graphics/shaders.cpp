#include <xd/graphics/vertex_traits.hpp>
#include <xd/graphics/shaders.hpp>

xd::flat_shader::flat_shader()
{
	static const char *vertex_shader_src =
		"#version 110\n"
		"uniform mat4 mvpMatrix;"
		"attribute vec4 vVertex;"
		"void main(void)"
		"{"
		"	gl_Position = mvpMatrix * vVertex;"
		"}";

	static const char *fragment_shader_src =
		"#version 110\n"
		"uniform vec4 vColor;"
		"void main(void)"
		"{"
		"	gl_FragColor = vColor;"
		"}";

	attach(GL_VERTEX_SHADER, vertex_shader_src);
	attach(GL_FRAGMENT_SHADER, fragment_shader_src);
	bind_attribute("vVertex", xd::VERTEX_POSITION);
	link();
}

void xd::flat_shader::setup(const glm::mat4& mvp, const glm::vec4& color)
{
	use();
	bind_uniform("mvpMatrix", mvp);
	bind_uniform("vColor", color);
}

xd::shaded_shader::shaded_shader()
{
	static const char *vertex_shader_src =
		"#version 110\n"
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
		"#version 110\n"
		"varying vec4 vVaryingColor;"
		"void main(void)"
		"{"
		"	gl_FragColor = vVaryingColor;"
		"}";

	attach(GL_VERTEX_SHADER, vertex_shader_src);
	attach(GL_FRAGMENT_SHADER, fragment_shader_src);
	bind_attribute("vVertex", xd::VERTEX_POSITION);
	bind_attribute("vColor", xd::VERTEX_COLOR);
	link();
}

void xd::shaded_shader::setup(const glm::mat4& mvp)
{
	use();
	bind_uniform("mvpMatrix", mvp);
}

xd::texture_shader::texture_shader()
{
	static const char *vertex_shader_src =
		"#version 110\n"
		"uniform mat4 mvpMatrix;"
		"attribute vec4 vVertex;"
		"attribute vec2 vTexCoords;"
		"varying vec2 vVaryingTexCoords;"
		"void main(void)"
		"{"
		"	vVaryingTexCoords = vTexCoords;"
		"	gl_Position = mvpMatrix * vVertex;"
		"}";

	static const char *fragment_shader_src =
		"#version 110\n"
		"uniform sampler2D colorMap;"
		"varying vec2 vVaryingTexCoords;"
		"void main(void)"
		"{"
		"	gl_FragColor = texture2D(colorMap, vVaryingTexCoords.st);"
		"}";

	attach(GL_VERTEX_SHADER, vertex_shader_src);
	attach(GL_FRAGMENT_SHADER, fragment_shader_src);
	bind_attribute("vVertex", xd::VERTEX_POSITION);
	bind_attribute("vTexCoords", xd::VERTEX_TEXTURE);
	link();
}

void xd::texture_shader::setup(const glm::mat4& mvp, const texture::ptr tex)
{
	use();
	tex->bind(GL_TEXTURE0);
	bind_uniform("mvpMatrix", mvp);
	bind_uniform("colorMap", 0);
}

xd::texture_mask_shader::texture_mask_shader()
{
	static const char *vertex_shader_src =
		"#version 110\n"
		"uniform mat4 mvpMatrix;"
		"attribute vec4 vVertex;"
		"attribute vec2 vTexCoords;"
		"varying vec2 vVaryingTexCoords;"
		"void main(void)"
		"{"
		"	vVaryingTexCoords = vTexCoords;"
		"	gl_Position = mvpMatrix * vVertex;"
		"}";

	static const char *fragment_shader_src =
		"#version 110\n"
		"uniform sampler2D colorMap;"
		"uniform sampler2D maskMap;"
		"varying vec2 vVaryingTexCoords;"
		"void main(void)"
		"{"
		"	gl_FragColor = texture2D(colorMap, vVaryingTexCoords.st);"
		"	gl_FragColor.a = texture2D(maskMap, vVaryingTexCoords.st).r;"
		"}";

	attach(GL_VERTEX_SHADER, vertex_shader_src);
	attach(GL_FRAGMENT_SHADER, fragment_shader_src);
	bind_attribute("vVertex", xd::VERTEX_POSITION);
	bind_attribute("vTexCoords", xd::VERTEX_TEXTURE);
	link();
}

void xd::texture_mask_shader::setup(const glm::mat4& mvp, const texture::ptr tex, const texture::ptr mask)
{
	use();
	tex->bind(GL_TEXTURE0);
	mask->bind(GL_TEXTURE1);
	bind_uniform("mvpMatrix", mvp);
	bind_uniform("colorMap", 0);
	bind_uniform("maskMap", 1);
}

xd::text_shader::text_shader()
{
	static const char *vertex_shader_src =
		"#version 110\n"
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
		"#version 110\n"
		"uniform vec4 vColor;"
		"uniform sampler2D colorMap;"
		"varying vec2 vVaryingTexCoords;"
		"void main(void)"
		"{"
		"	gl_FragColor.rgb = vColor.rgb;"
		"	gl_FragColor.a = vColor.a * texture2D(colorMap, vVaryingTexCoords.st).r;"
		"}";

	attach(GL_VERTEX_SHADER, vertex_shader_src);
	attach(GL_FRAGMENT_SHADER, fragment_shader_src);
	bind_attribute("vVertex", xd::VERTEX_POSITION);
	bind_attribute("vTexCoords", xd::VERTEX_TEXTURE);
	link();
}

xd::sprite_shader::sprite_shader()
{
	static const char *vertex_shader_src =
		"#version 110\n"
		"uniform mat4 mvpMatrix;"
		"attribute vec4 vVertex;"
		"attribute vec2 vTexCoords;"
		"varying vec2 vVaryingTexCoords;"
		"void main(void)"
		"{"
		"	vVaryingTexCoords = vTexCoords;"
		"	gl_Position = mvpMatrix * vVertex;"
		"}";

	static const char *fragment_shader_src =
		"#version 110\n"
		"uniform vec4 vColor;"
		"uniform sampler2D colorMap;"
		"varying vec2 vVaryingTexCoords;"
		"void main(void)"
		"{"
		"	gl_FragColor = vColor * texture2D(colorMap, vVaryingTexCoords.st);"
		"}";

	attach(GL_VERTEX_SHADER, vertex_shader_src);
	attach(GL_FRAGMENT_SHADER, fragment_shader_src);
	bind_attribute("vVertex", xd::VERTEX_POSITION);
	bind_attribute("vTexCoords", xd::VERTEX_TEXTURE);
	link();
}
