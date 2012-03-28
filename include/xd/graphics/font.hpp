#ifndef H_XD_GRAPHICS_FONT
#define H_XD_GRAPHICS_FONT

#include <xd/graphics/detail/font.hpp>
#include <xd/graphics/font_style.hpp>
#include <xd/vendor/glew/glew.h>
#include <xd/glm.hpp>
#include <xd/handle.hpp>
#include <xd/graphics/types.hpp>
#include <xd/graphics/vertex_batch.hpp>
#include <xd/graphics/shader_program.hpp>
#include <xd/graphics/transform_geometry.hpp>
#include <xd/vendor/utf8.h>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <memory>
#include <unordered_map>

#ifndef XD_STATIC
// disable warning about boost::noncopyable not being dll-exportable
// as well as the private members that can't be accessed by client
#pragma warning(disable: 4275 4251 4231)

// export instantiantions of used stdlib classes
//XD_API_TEMPLATE template class XD_API std::allocator<char>;
//XD_API_TEMPLATE template class XD_API std::basic_string<char, std::char_traits<char>, std::allocator<char> >;
#endif

namespace xd
{
	// font class
	class XD_API font : public boost::noncopyable
	{
	public:
		typedef handle<font> handle;
		typedef weak_handle<font> weak_handle;

		font(const std::string& filename);
		virtual ~font();

		void link_font(const std::string& type, const std::string& filename);
		void link_font(const std::string& type, font::handle font);
		void unlink_font(const std::string& type);

		void render(const std::string& text, const font_style& style,
			shader_program::handle shader, const glm::mat4& mvp, glm::vec2 *pos = 0);

		const std::string& get_mvp_uniform();
		const std::string& get_pos_uniform();
		const std::string& get_color_uniform();
		const std::string& get_texture_uniform();

		void set_mvp_uniform(const std::string&);
		void set_pos_uniform(const std::string&);
		void set_color_uniform(const std::string&);
		void set_texture_uniform(const std::string&);
	private:
		typedef std::unordered_map<int, std::unique_ptr<detail::font::glyph>> glyph_map_t;
		typedef std::unordered_map<std::string, font::handle> font_map_t;
		const detail::font::glyph& load_glyph(utf8::uint32_t char_index, int size);

		std::unique_ptr<detail::font::face> m_face;
		std::string m_filename;
		glyph_map_t m_glyph_map;
		font_map_t m_linked_fonts;
		std::string m_mvp_uniform;
		std::string m_position_uniform;
		std::string m_color_uniform;
		std::string m_texture_uniform;
	};
}

#endif
