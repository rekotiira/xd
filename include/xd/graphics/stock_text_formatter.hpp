#ifndef H_XD_GRAPHICS_STOCK_TEXT_FORMATTER
#define H_XD_GRAPHICS_STOCK_TEXT_FORMATTER

#include <xd/glm.hpp>
#include <string>
#include <ctime>
#include <unordered_map>
#include <xd/graphics/text_formatter.hpp>

namespace xd
{
	class XD_API stock_text_formatter : public text_formatter
	{
	public:
		typedef boost::intrusive_ptr<stock_text_formatter> ptr;

		stock_text_formatter();
		virtual ~stock_text_formatter();

		bool has_color(const std::string& name);
		glm::vec4 get_color(const std::string& name);
		void set_color(const std::string& name, const glm::vec4& color);

		void reset_typewriter(int timer = 0);

	private:
		typedef std::unordered_map<std::string, glm::vec4> color_map_t;
		typedef std::unordered_map<int, std::clock_t> typewriter_timer_map_t;

		void size_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args);
		void type_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args);
		void bold_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args);
		void italic_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args);
		void color_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args);
		void shadow_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args);
		void outline_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args);
		void spacing_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args);
		void rainbow_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args);
		void typewriter_decorator(text_decorator& decorator, const formatted_text& text, const text_decorator_args& args);

		color_map_t m_colors;
		typewriter_timer_map_t m_typewriter_timers;
	};
}

#endif

