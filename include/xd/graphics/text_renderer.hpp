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
		text_renderer(shader_program::handle shader);
		virtual ~text_renderer();

		void render(xd::font::handle font, const xd::font_style& style, const mat4& mvp, const std::string& text);
		void render_formatted(xd::font::handle font, xd::text_formatter::handle formatter,
			const xd::font_style& style, const mat4& mvp, const std::string& text);

	private:
		shader_program::handle m_shader;		
	};
}

#endif
