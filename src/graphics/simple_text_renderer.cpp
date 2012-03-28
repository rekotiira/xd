#include <xd/graphics/simple_text_renderer.hpp>
#include <memory>

namespace xd { namespace detail {

	// keep only a single text_shader loaded
	xd::text_shader::handle text_renderer_shader;

} }

xd::simple_text_renderer::simple_text_renderer(float width, float height)
{
	// setup an orthographic projection where 0 = top and height = bottom
	m_projection = ortho(0.0f, width, 0.0f, height, -100.0f, 100.0f);
	// check if the default shader is not already loaded
	if (!detail::text_renderer_shader)
		detail::text_renderer_shader.reset(new text_shader);
	m_shader = detail::text_renderer_shader;
}

xd::simple_text_renderer::simple_text_renderer(float width, float height, xd::shader_program::handle shader)
	: m_shader(shader)
{
	// setup an orthographic projection where 0 = top and height = bottom
	m_projection = ortho(0.0f, width, 0.0f, height, -100.0f, 100.0f);
}

xd::simple_text_renderer::~simple_text_renderer()
{
}

void xd::simple_text_renderer::render(xd::font::handle font, const xd::font_style& style, float x, float y, const std::string& text)
{
	glm::vec2 pos(x, y);
	font->render(text, style, m_shader, m_projection, &pos);
}

void xd::simple_text_renderer::render_formatted(xd::font::handle font, xd::text_formatter::handle formatter,
	const xd::font_style& style, float x, float y, const std::string& text)
{
	auto mvp = xd::translate(m_projection, vec3(x, y, 0));
	formatter->render(text, font, style, m_shader, mvp);
}
