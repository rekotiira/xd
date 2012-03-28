#include <xd/graphics/text_renderer.hpp>
#include <memory>

namespace xd { namespace detail {

	// keep only a single text_shader loaded
	// this is defined in simple_text_renderer.cpp
	extern xd::text_shader::handle text_renderer_shader;

} }

xd::text_renderer::text_renderer()
{
	// check if the default shader is not already loaded
	if (!detail::text_renderer_shader)
		detail::text_renderer_shader.reset(new text_shader);
	m_shader = detail::text_renderer_shader;
}

xd::text_renderer::text_renderer(xd::shader_program::handle shader)
	: m_shader(shader)
{
}

xd::text_renderer::~text_renderer()
{
}

void xd::text_renderer::render(xd::font::handle font, const xd::font_style& style, const xd::mat4& mvp, const std::string& text)
{
	font->render(text, style, m_shader, mvp);
}

void xd::text_renderer::render_formatted(xd::font::handle font, xd::text_formatter::handle formatter,
	const xd::font_style& style, const xd::mat4& mvp, const std::string& text)
{
	formatter->render(text, font, style, m_shader, mvp);
}
