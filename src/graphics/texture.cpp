#include <stdexcept>
#include <xd/graphics/texture.hpp>

namespace xd { namespace detail { namespace texture {

	bool initialized = false;

} } }

xd::texture::texture(int width, int height, const void *data,
	GLint wrap_s, GLint wrap_t, GLint mag_filter, GLint min_filter)
{
	init();
	set_wrap(wrap_s, wrap_t);
	set_filter(mag_filter, min_filter);
	load(width, height, data);
}


xd::texture::texture(const std::string& filename,
	GLint wrap_s, GLint wrap_t, GLint mag_filter, GLint min_filter)
{
	init();
	set_wrap(wrap_s, wrap_t);
	set_filter(mag_filter, min_filter);
	load(filename);
}

xd::texture::texture(const xd::image& image,
	GLint wrap_s, GLint wrap_t, GLint mag_filter, GLint min_filter)
{
	init();
	set_wrap(wrap_s, wrap_t);
	set_filter(mag_filter, min_filter);
	load(image);
}

void xd::texture::init()
{
	m_width = 0;
	m_height = 0;

	glGenTextures(1, &m_texture_id);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);
}

xd::texture::~texture()
{
	glDeleteTextures(1, &m_texture_id);
}

void xd::texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_texture_id);
}

void xd::texture::bind(int unit) const
{
	glActiveTexture(unit);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);
}

void xd::texture::load(const std::string& filename)
{
	load(image(filename));
}

void xd::texture::load(const image& image)
{
	load(image.width(), image.height(), image.data());
}

void xd::texture::load(int width, int height, const void *data)
{
	if (width <= 0)
		throw std::invalid_argument("invalid texture width");
	if (height <= 0)
		throw std::invalid_argument("invalid texture height");

	// store the width and height
	m_width = width;
	m_height = height;

	// create storage for texture
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	if (data) {
		// load the pixel data
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
}

void xd::texture::load(const void *data)
{
	if (data) {
		// load the pixel data
		bind();
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
}

GLuint xd::texture::texture_id() const
{
	return m_texture_id;
}

int xd::texture::width() const
{
	return m_width;
}

int xd::texture::height() const
{
	return m_height;
}

void xd::texture::set_wrap(GLint wrap_s, GLint wrap_t)
{
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
}

void xd::texture::set_filter(GLint mag_filter, GLint min_filter)
{
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
}
