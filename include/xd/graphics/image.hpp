#ifndef H_XD_GRAPHICS_IMAGE
#define H_XD_GRAPHICS_IMAGE

#include <xd/graphics/detail/image.hpp>

#include <xd/config.hpp>
#include <xd/handle.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <memory>

#ifndef XD_STATIC
// disable warning about boost::noncopyable not being dll-exportable
// as well as the private members that can't be accessed by client
#pragma warning(disable: 4275 4251)
#endif

namespace xd
{
	class XD_API image : public boost::noncopyable
	{
	public:
		typedef handle<image> handle;
		typedef weak_handle<image> weak_handle;

		image(const std::string& filename);
		virtual ~image();

		void load(const std::string& filename);
		void save(const std::string& filename) const;

		int width() const;
		int height() const;

		void *data();
		const void *data() const;

	private:
		detail::image::handle_ptr m_image;
		int m_width;
		int m_height;

		void init();
	};
}

#endif
