#include <IL/il.h>
#include <xd/graphics/exceptions.h>
#include <xd/graphics/image.h>

namespace xd { namespace detail { namespace image {

	bool initialized = false;

	struct handle
	{
		ILuint image_id;
	};

} } }

xd::image::image(const std::string& filename)
	: m_width(0)
	, m_height(0)
{
	init();
	load(filename);
}

void xd::image::init()
{
	if (!detail::image::initialized) {
		ilInit();
		ilEnable(IL_ORIGIN_SET);
		ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_UPPER_LEFT);
		detail::image::initialized = true;
	}

	m_image = detail::image::handle_ptr(new detail::image::handle);
	ilGenImages(1, &m_image->image_id);
}

xd::image::~image()
{
	ilDeleteImages(1, &m_image->image_id);
}

void xd::image::load(const std::string& filename)
{
	// bind and load the image
	ilBindImage(m_image->image_id);
	if (ilLoadImage(filename.c_str()) == IL_FALSE)
		throw failed_to_load_image(filename);

	// set width and height
	m_width = ilGetInteger(IL_IMAGE_WIDTH);
	m_height = ilGetInteger(IL_IMAGE_HEIGHT);

	// convert image to RGBA
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
}

void xd::image::save(const std::string& filename) const
{
	// bind image
	ilBindImage(m_image->image_id);

	// save image
	ilSaveImage(filename.c_str());
}

int xd::image::width() const
{
	return m_width;
}

int xd::image::height() const
{
	return m_height;
}

void *xd::image::data()
{
	ilBindImage(m_image->image_id);
	return ilGetData();
}

const void *xd::image::data() const
{
	ilBindImage(m_image->image_id);
	return ilGetData();
}
