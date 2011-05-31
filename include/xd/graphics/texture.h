#ifndef H_XD_GRAPHICS_TEXTURE
#define H_XD_GRAPHICS_TEXTURE

#include <GL/glew.h>
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <xd/graphics/image.h>

namespace xd
{
	class texture : public boost::noncopyable
	{
	public:
		texture(const std::string& filename);
		texture(const xd::image& image);
		texture(int width, int height, const void *data = 0);
		virtual ~texture();

		void bind() const;
		void bind(int unit) const;

		void load(const std::string& filename);
		void load(const xd::image& image);
		void load(int width, int height, const void *data);
		
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
