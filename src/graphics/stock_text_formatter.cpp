#include <xd/graphics/stock_text_formatter.h>
#include <boost/bind.hpp>

xd::stock_text_formatter::stock_text_formatter()
{
	// register default colors
	m_colors["white"]  = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_colors["black"]  = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_colors["gray"]   = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	m_colors["red"]    = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	m_colors["green"]  = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	m_colors["blue"]   = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	m_colors["orange"] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	m_colors["purple"] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	m_colors["cyan"]   = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

	// register default decorators
	register_decorator("bold", boost::bind(&stock_text_formatter::bold_decorator, this, _1, _2, _3));
	register_decorator("color", boost::bind(&stock_text_formatter::color_decorator, this, _1, _2, _3));
	register_decorator("shadow", boost::bind(&stock_text_formatter::shadow_decorator, this, _1, _2, _3));
}

xd::stock_text_formatter::~stock_text_formatter()
{
}

bool xd::stock_text_formatter::is_color(const std::string& name)
{
	color_map_t::iterator i = m_colors.find(name);
	return (i != m_colors.end());
}

glm::vec4 xd::stock_text_formatter::get_color(const std::string& name)
{
	color_map_t::iterator i = m_colors.find(name);
	if (i == m_colors.end())
		throw std::invalid_argument("invalid color: "+name);
	return i->second;
}

void xd::stock_text_formatter::set_color(const std::string& name, const glm::vec4& color)
{
	m_colors[name] = color;
}

void xd::stock_text_formatter::bold_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	decorator.push_type("bold");
	decorator.push_text(text);
}

void xd::stock_text_formatter::color_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	glm::vec4 color;
	if (args.count() == 1) {
		std::string name = args.get<std::string>(0);
		color_map_t::iterator i = m_colors.find(name);
		if (i == m_colors.end())
			throw text_decorator_exception("invalid color: "+name);
		color = i->second;
	} else if (args.count() >= 3) {
		float r = args.get<float>(0) / 255.0f;
		float g = args.get<float>(1) / 255.0f;
		float b = args.get<float>(2) / 255.0f;
		float a = args.get<float>(3, 255) / 255.0f;
		color = glm::vec4(r, g, b, a);
	} else {
		throw text_decorator_exception("invalid arguments to color decorator");
	}

	decorator.push_color(color);
	decorator.push_text(text);
}

void xd::stock_text_formatter::shadow_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	font_shadow shadow;
	shadow.x = args.get<float>(0, 2);
	shadow.y = args.get<float>(1, -2);
	if (args.count() == 3) {
		std::string name = args.get<std::string>(0);
		color_map_t::iterator i = m_colors.find(name);
		if (i == m_colors.end())
			throw text_decorator_exception("invalid color: "+name);
		shadow.color = i->second;
	} else if (args.count() >= 5) {
		float r = args.get<float>(3) / 255.0f;
		float g = args.get<float>(4) / 255.0f;
		float b = args.get<float>(5) / 255.0f;
		float a = args.get<float>(6, 255) / 255.0f;
		shadow.color = glm::vec4(r, g, b, a);
	} else if (args.count() != 0) {
		throw text_decorator_exception("invalid arguments to shadow decorator");
	} else {
		// default shadow color
		shadow.color = glm::vec4(0, 0, 0, 1);
	}

	decorator.push_shadow(shadow);
	decorator.push_text(text);
}