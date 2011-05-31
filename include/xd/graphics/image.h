#ifndef H_XD_GRAPHICS_IMAGE
#define H_XD_GRAPHICS_IMAGE

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace xd
{
	namespace detail { namespace image {

		struct handle;
		typedef boost::shared_ptr<handle> handle_ptr;

	} }

	class image : public boost::noncopyable
	{
	public:
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

	typedef boost::shared_ptr<image> image_ptr;
}

#endif
