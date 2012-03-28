#ifndef H_XD_GRAPHICS_FONT_STYLE
#define H_XD_GRAPHICS_FONT_STYLE

#include <xd/glm.hpp>
#include <xd/graphics/exceptions.hpp>
#include <boost/optional.hpp>

namespace xd
{
	struct font_shadow
	{
		font_shadow()
			: x(1), y(1), color(vec4(0,0,0,1)) {}
		font_shadow(float x, float y, const vec4& color)
			: x(x), y(y), color(color) {}

		float x;
		float y;
		vec4 color;
	};

	struct font_outline
	{
		font_outline()
			: width(1), color(vec4(0,0,0,1)) {}
		font_outline(int width, const vec4& color)
			: width(width), color(color) {}

		int width;
		vec4 color;
	};

	class font_style
	{
	public:
		font_style(const vec4& color, int size)
			: m_color(color)
			, m_size(size)
			, m_letter_spacing(0)
			, m_line_height(0)
		{
		}

		// setters for required styles
		font_style& color(const vec4& color) { m_color = color; return *this; }
		font_style& size(int size) { m_size = size; return *this; }
		font_style& letter_spacing(float letter_spacing) { m_letter_spacing = letter_spacing; return *this; }
		font_style& line_height(float line_height) { m_line_height = line_height; return *this; }

		// getters for required styles
		vec4 color() const { return m_color; }
		int size() const { return m_size; }
		float letter_spacing() const { return m_letter_spacing; }
		float line_height() const { return m_line_height; }

		// modifiable getters for required styles
		vec4& color() { return m_color; }
		int& size() { return m_size; }
		float& letter_spacing() { return m_letter_spacing; }
		float& line_height() { return m_line_height; }

		// setters for optional styles
		font_style& type(const std::string& type) { m_type = type; return *this; }
		font_style& shadow(const font_shadow& shadow) { m_shadow = shadow; return *this; }
		font_style& shadow(float x, float y, const vec4& color) { m_shadow = font_shadow(x, y, color); return *this; }
		font_style& outline(int width, const vec4& color) { m_outline = font_outline(width, color); return *this; }
		font_style& outline(const font_outline& outline) { m_outline = outline; return *this; }

		// resetters for optional styles
		font_style& reset_type() { m_type = boost::none; return *this; }
		font_style& reset_shadow() { m_shadow = boost::none; return *this; }
		font_style& reset_outline() { m_outline = boost::none; return *this; }

		// checkers for optional styles
		bool has_type() const { return m_type.is_initialized(); }
		bool has_shadow() const { return m_shadow.is_initialized(); }
		bool has_outline() const { return m_outline.is_initialized(); }

		// getters for optional styles
		std::string type() const
		{
			if (!m_type)
				throw font_style_undefined("type");
			return *m_type;
		}

		font_shadow shadow() const
		{
			if (!m_shadow)
				throw font_style_undefined("shadow");
			return *m_shadow;
		}

		font_outline outline() const
		{
			if (!m_outline)
				throw font_style_undefined("outline");
			return *m_outline;
		}

		// modifiable getters for optional styles
		std::string& type()
		{
			if (!m_type)
				throw font_style_undefined("type");
			return *m_type;
		}

		font_shadow& shadow()
		{
			if (!m_shadow)
				throw font_style_undefined("shadow");
			return *m_shadow;
		}

		font_outline& outline()
		{
			if (!m_outline)
				throw font_style_undefined("outline");
			return *m_outline;
		}

	private:
		// font is a friend class
		friend class font;

		// required styles
		vec4 m_color;
		int m_size;
		float m_letter_spacing;
		float m_line_height;

		// optional styles
		boost::optional<std::string> m_type;
		boost::optional<font_shadow> m_shadow;
		boost::optional<font_outline> m_outline;
	};
}

#endif
