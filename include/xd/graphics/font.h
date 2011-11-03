#ifndef H_XD_GRAPHICS_FONT
#define H_XD_GRAPHICS_FONT

//#include <GL/glew.h>
#include <glm/glm.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <boost/optional.hpp>
#include <xd/graphics/types.h>
#include <xd/graphics/vertex_batch.h>
#include <xd/graphics/shader_program.h>
#include <xd/graphics/transform_geometry.h>
#include <xd/utf8.h>

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

	// smart pointer to font
	class font;
	typedef boost::shared_ptr<font> font_ptr;

	// font class
	class font : public boost::noncopyable
	{
	public:
		font(const std::string& filename, int size);
		~font();

		void link_font(const std::string& type, const std::string& filename);
		void link_font(const std::string& type, font_ptr font);
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
		typedef boost::unordered_map<int, detail::font::glyph> glyph_map_t;
		typedef boost::unordered_map<std::string, font_ptr> font_map_t;
		const detail::font::glyph& load_glyph(utf8::uint32_t char_index);

		detail::font::face *m_face;
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
