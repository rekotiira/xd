#ifndef H_XD_GRAPHICS_FONT_MANAGER
#define H_XD_GRAPHICS_FONT_MANAGER

#include <string>
#include <boost/noncopyable.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <xd/graphics/font.h>

namespace xd
{
	class font_manager : public boost::noncopyable
	{
	public:
		font_manager();
		~font_manager();

		font_handle load_font(const std::string& filename);
	
	private:
		FT_Library m_library;
	};
}

#endif
