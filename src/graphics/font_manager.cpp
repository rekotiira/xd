#include <xd/graphics/font_manager.h>

xd::font_manager::font_manager()
{
	FT_Init_FreeType(&m_library);
}

xd::font_manager::~font_manager()
{
	FT_Done_FreeType(m_library);
}

xd::font_handle xd::font_manager::load_font(const std::string& filename)
{
	//font *font = new font();

}
