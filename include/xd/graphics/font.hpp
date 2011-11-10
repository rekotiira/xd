#ifndef H_XD_GRAPHICS_FONT
#define H_XD_GRAPHICS_FONT

#include <xd/vendor/glew/glew.h>
#include <xd/vendor/glm/glm.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>
#include <memory>
#include <unordered_map>
#include <xd/ref_counted.hpp>
#include <xd/graphics/types.hpp>
#include <xd/graphics/vertex_batch.hpp>
#include <xd/graphics/shader_program.hpp>
#include <xd/graphics/transform_geometry.hpp>
#include <xd/utf8.h>

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
	struct font_shadow
	{
		font_shadow() {}
		font_shadow(float x_, float y_, const glm::vec4& color_)
			: x(x_), y(y_), color(color_) {}

		float x;
		float y;
		glm::vec4 color;
	};

	struct font_outline
	{
		font_outline() {}
		font_outline(int width_, const glm::vec4& color_)
			: width(width_), color(color_) {}

		int width;
		glm::vec4 color;
	};

	struct font_style
	{
		font_style()
			: letter_spacing(0)
			, line_height(0)
		{
		}

		// required styles
		glm::vec4 color;
		float letter_spacing;
		float line_height;

		// optional styles
		boost::optional<std::string> type;
		boost::optional<font_shadow> shadow;
		boost::optional<font_outline> outline;
	};

	namespace detail { namespace font {

		struct glyph;
		struct face;

	} }

	// font class
	class XD_API font : public xd::ref_counted, public boost::noncopyable
	{
	public:
		typedef boost::intrusive_ptr<font> ptr;

		font(const std::string& filename, int size);
		~font();

		void link_font(const std::string& type, const std::string& filename);
		void link_font(const std::string& type, font::ptr font);
		void unlink_font(const std::string& type);

		void render(const std::string& text, const font_style& style,
			shader_program& shader, const glm::mat4& mvp, glm::vec2 *pos = 0);

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
		typedef std::unordered_map<std::string, font::ptr> font_map_t;
		const detail::font::glyph& load_glyph(utf8::uint32_t char_index);

		std::unique_ptr<detail::font::face> m_face;
		std::string m_filename;
		int m_size;
		glyph_map_t m_glyph_map;
		font_map_t m_linked_fonts;
		std::string m_mvp_uniform;
		std::string m_position_uniform;
		std::string m_color_uniform;
		std::string m_texture_uniform;
	};
}

#endif
