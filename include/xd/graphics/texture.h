#ifndef H_XD_GRAPHICS_TEXTURE
#define H_XD_GRAPHICS_TEXTURE

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <xd/graphics/image.h>

namespace xd
{
	class XD_API texture : public boost::noncopyable
	{
	public:
		texture(int width, int height, const void *data = 0,
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT,
			GLint mag_filter = GL_LINEAR, GLint min_filter = GL_LINEAR);
		texture(const std::string& filename,
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT,
			GLint mag_filter = GL_LINEAR, GLint min_filter = GL_LINEAR);
		texture(const xd::image& image,
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT,
			GLint mag_filter = GL_LINEAR, GLint min_filter = GL_LINEAR);
		virtual ~texture();

		void bind() const;
		void bind(int unit) const;

		void load(const std::string& filename);
		void load(const xd::image& image);
		void load(int width, int height, const void *data);
		void load(const void *data);

		GLuint texture_id() const;
		int width() const;
		int height() const;

		void set_wrap(GLint wrap_s, GLint wrap_t);
		void set_filter(GLint mag_filter, GLint min_filter);

	private:
		GLuint m_texture_id;
		int m_width;
		int m_height;

		void init();
	};

	typedef boost::shared_ptr<texture> texture_ptr;
}

#endif
