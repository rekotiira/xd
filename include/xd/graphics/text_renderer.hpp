#ifndef H_XD_TEXT_RENDERER
#define H_XD_TEXT_RENDERER

#include <xd/config.hpp>
#include <xd/glm.hpp>
#include <xd/graphics/font.hpp>
#include <xd/graphics/text_formatter.hpp>
#include <xd/graphics/shaders.hpp>
#include <string>

namespace xd
{
	class XD_API text_renderer
	{
	public:
		text_renderer();
		text_renderer(shader_program::ptr shader);
		virtual ~text_renderer();

		void render(xd::font::ptr font, const xd::font_style& style, const mat4& mvp, const std::string& text);
		void render_formatted(xd::font::ptr font, xd::text_formatter::ptr formatter,
			const xd::font_style& style, const mat4& mvp, const std::string& text);

	private:
		shader_program::ptr m_shader;		
	};
}

#endif
