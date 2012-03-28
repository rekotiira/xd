#include <xd/graphics/stock_text_formatter.hpp>
#include <functional>

xd::stock_text_formatter::stock_text_formatter()
{
	// shortcut for using _1 _2 etc
	using namespace std::placeholders;

	// register default colors
	m_colors["white"]  = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	m_colors["black"]  = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_colors["gray"]   = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	m_colors["red"]    = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	m_colors["green"]  = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	m_colors["blue"]   = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	m_colors["yellow"] = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
	m_colors["purple"] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	m_colors["cyan"]   = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);

	// register default decorators
	register_decorator("size", std::bind(&stock_text_formatter::size_decorator, this, _1, _2, _3));
	register_decorator("type", std::bind(&stock_text_formatter::type_decorator, this, _1, _2, _3));
	register_decorator("bold", std::bind(&stock_text_formatter::bold_decorator, this, _1, _2, _3));
	register_decorator("italic", std::bind(&stock_text_formatter::italic_decorator, this, _1, _2, _3));
	register_decorator("color", std::bind(&stock_text_formatter::color_decorator, this, _1, _2, _3));
	register_decorator("shadow", std::bind(&stock_text_formatter::shadow_decorator, this, _1, _2, _3));
	register_decorator("outline", std::bind(&stock_text_formatter::outline_decorator, this, _1, _2, _3));
	register_decorator("spacing", std::bind(&stock_text_formatter::spacing_decorator, this, _1, _2, _3));
	register_decorator("rainbow", std::bind(&stock_text_formatter::rainbow_decorator, this, _1, _2, _3));
	register_decorator("typewriter", std::bind(&stock_text_formatter::typewriter_decorator, this, _1, _2, _3));
}

xd::stock_text_formatter::~stock_text_formatter()
{
}

bool xd::stock_text_formatter::has_color(const std::string& name)
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

void xd::stock_text_formatter::reset_typewriter(int timer)
{
	m_typewriter_timers[timer] = std::clock();
}

void xd::stock_text_formatter::size_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	decorator.push_size(args.get<int>(0));
	decorator.push_text(text);
}

void xd::stock_text_formatter::type_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	decorator.push_type(args.get<std::string>(0));
	decorator.push_text(text);
}

void xd::stock_text_formatter::bold_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	decorator.push_type("bold");
	decorator.push_text(text);
}

void xd::stock_text_formatter::italic_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	decorator.push_type("italic");
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
	float x = args.get<float>(0, 1);
	float y = args.get<float>(1, -1);
	vec4 color;
	if (args.count() == 3) {
		std::string name = args.get<std::string>(2);
		color_map_t::iterator i = m_colors.find(name);
		if (i == m_colors.end())
			throw text_decorator_exception("invalid color: "+name);
		color = i->second;
	} else if (args.count() >= 5) {
		float r = args.get<float>(2) / 255.0f;
		float g = args.get<float>(3) / 255.0f;
		float b = args.get<float>(4) / 255.0f;
		float a = args.get<float>(5, 255) / 255.0f;
		color = glm::vec4(r, g, b, a);
	} else if (args.count() != 0 && args.count() != 2) {
		throw text_decorator_exception("invalid arguments to shadow decorator");
	} else {
		// default shadow color
		color = glm::vec4(0, 0, 0, 1);
	}

	decorator.push_shadow(font_shadow(x, y, color));
	decorator.push_text(text);
}

void xd::stock_text_formatter::outline_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	int width = args.get<int>(0, 1);
	vec4 color;
	if (args.count() == 2) {
		std::string name = args.get<std::string>(1);
		color_map_t::iterator i = m_colors.find(name);
		if (i == m_colors.end())
			throw text_decorator_exception("invalid color: "+name);
		color = i->second;
	} else if (args.count() >= 4) {
		float r = args.get<float>(1) / 255.0f;
		float g = args.get<float>(2) / 255.0f;
		float b = args.get<float>(3) / 255.0f;
		float a = args.get<float>(4, 255) / 255.0f;
		color = glm::vec4(r, g, b, a);
	} else if (args.count() != 0 && args.count() != 1) {
		throw text_decorator_exception("invalid arguments to outline decorator");
	} else {
		// default outline color
		color = glm::vec4(0, 0, 0, 1);
	}

	decorator.push_outline(font_outline(width, color));
	decorator.push_text(text);
}

void xd::stock_text_formatter::spacing_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	decorator.push_letter_spacing(args.get<float>(0));
	decorator.push_text(text);
}

void xd::stock_text_formatter::rainbow_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	static std::vector<glm::vec4> rainbow_colors;
	if (rainbow_colors.size() == 0) {
		rainbow_colors.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		rainbow_colors.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
		rainbow_colors.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		rainbow_colors.push_back(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
		rainbow_colors.push_back(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
		rainbow_colors.push_back(glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
	}

	int color_index = 0;
	for (formatted_text::const_iterator i = text.begin(); i != text.end(); ++i) {
		decorator.push_color(rainbow_colors[color_index++ % rainbow_colors.size()]);
		decorator.push_text(*i);
	}
}

void xd::stock_text_formatter::typewriter_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args)
{
	// get the typewriter timer to use (default: 0)
	int timer = args.get<int>(0, 0);

	// the speed of the typewriter (default: 10 cps)
	float speed = args.get<float>(1, 10.0f);

	// find the start time, or create a new timer if one doesn't exist with current time
	std::clock_t started;
	typewriter_timer_map_t::iterator i = m_typewriter_timers.find(timer);
	if (i != m_typewriter_timers.end())
		started = i->second;
	else
		m_typewriter_timers[timer] = started = std::clock();

	// calculate time elapsed
	float elapsed = (float)(clock() - started) / 1000.0f;
	int index = 0;
	for (xd::formatted_text::const_iterator i = text.begin(); i != text.end(); ++i) {
		if ((elapsed*speed) < ++index)
			break;
		decorator.push_text(*i);
	}
}
