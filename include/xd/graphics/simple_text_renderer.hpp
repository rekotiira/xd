#ifndef H_XD_SIMPLE_TEXT_RENDERER
#define H_XD_SIMPLE_TEXT_RENDERER

#include <xd/config.hpp>
#include <xd/glm.hpp>
#include <xd/graphics/font.hpp>
#include <xd/graphics/text_formatter.hpp>
#include <xd/graphics/shaders.hpp>
#include <string>

namespace xd
{
	class XD_API simple_text_renderer
	{
	public:
		simple_text_renderer(float width, float height);
		simple_text_renderer(float width, float height, shader_program::ptr shader);
		virtual ~simple_text_renderer();

		void render(xd::font::ptr font, const xd::font_style& style, float x, float y, const std::string& text);
		void render_formatted(xd::font::ptr font, xd::text_formatter::ptr formatter,
			const xd::font_style& style, float x, float y, const std::string& text);

	private:
		mat4 m_projection;
		shader_program::ptr m_shader;		
	};
}

#endif
