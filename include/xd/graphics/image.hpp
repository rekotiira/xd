#ifndef H_XD_GRAPHICS_IMAGE
#define H_XD_GRAPHICS_IMAGE

#include <boost/noncopyable.hpp>
#include <string>
#include <memory>
#include <xd/common.hpp>
#include <xd/ref_counted.hpp>

#ifndef XD_STATIC
// disable warning about boost::noncopyable not being dll-exportable
// as well as the private members that can't be accessed by client
#pragma warning(disable: 4275 4251)
#endif

namespace xd
{
	namespace detail { namespace image {

		struct handle;
		typedef std::shared_ptr<handle> handle_ptr;

	} }

	class XD_API image : public xd::ref_counted, public boost::noncopyable
	{
	public:
		typedef boost::intrusive_ptr<image> ptr;

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
