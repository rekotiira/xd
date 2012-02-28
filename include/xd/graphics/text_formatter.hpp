#ifndef H_XD_GRAPHICS_TEXT_FORMATTER
#define H_XD_GRAPHICS_TEXT_FORMATTER

#include <xd/graphics/detail/text_formatter.hpp>

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <list>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <xd/vendor/glm/glm.hpp>
#include <xd/graphics/exceptions.hpp>
#include <xd/graphics/font.hpp>
#include <xd/graphics/shader_program.hpp>
#include <xd/vendor/utf8.h>

#ifndef XD_STATIC
// disable warnings about nonstandard extension
// see: http://support.microsoft.com/kb/168958
#pragma warning(disable: 4231)

//XD_API_TEMPLATE template class XD_API std::allocator<char>;
//XD_API_TEMPLATE template class XD_API std::basic_string<char, std::char_traits<char>, std::allocator<char> >;
#endif

namespace xd
{
	class text_formatter;

	class formatted_char
	{
	public:
		formatted_char(utf8::uint32_t chr)
			: m_chr(chr)
			, m_level(0)
		{
		}

		utf8::uint32_t get() const
		{
			return m_chr;
		}

	private:
		utf8::uint32_t m_chr;
		detail::text_formatter::state_change_list m_state_changes;
		int m_level;

		friend class text_decorator;
		friend class text_formatter;
		friend class detail::text_formatter::decorate_text;
	};

	class formatted_text
	{
	public:
		typedef std::vector<formatted_char>::iterator iterator;
		typedef std::vector<formatted_char>::const_iterator const_iterator;

		formatted_text()
		{
		}

		formatted_text(const std::string& text)
		{
			*this += text;
		}

		formatted_text& operator+=(const formatted_char& chr)
		{
			m_chars.push_back(chr);
			return *this;
		}

		formatted_text& operator+=(const formatted_text& text)
		{
			if (this != &text) {
				m_chars.insert(m_chars.end(), text.m_chars.begin(), text.m_chars.end());
			}
			return *this;
		}

		formatted_text& operator+=(const std::string& text)
		{
			std::string::const_iterator i = text.begin();
			while (i != text.end()) {
				m_chars.push_back(utf8::next(i, text.end()));
			}
			return *this;
		}

		formatted_char& operator[](size_t index)
		{
			return m_chars[index];
		}

		const formatted_char& operator[](size_t index) const
		{
			return m_chars[index];
		}

		iterator begin()
		{
			return m_chars.begin();
		}

		iterator end()
		{
			return m_chars.end();
		}

		const_iterator begin() const
		{
			return m_chars.begin();
		}

		const_iterator end() const
		{
			return m_chars.end();
		}

		size_t length() const
		{
			return m_chars.size();
		}

		void clear()
		{
			m_chars.clear();
		}

		std::string get_unformatted() const
		{
			//return std::string(m_chars.begin(), m_chars.end());
			std::string unformatted;
			for (const_iterator i = m_chars.begin(); i != m_chars.end(); ++i) {
				utf8::append(i->get(), std::back_inserter(unformatted));
			}
			return unformatted;
		}

	private:
		std::vector<formatted_char> m_chars;

		friend class text_decorator;
		friend class text_formatter;
		friend class detail::text_formatter::decorate_text;
	};

	class text_decorator_args
	{
	public:
		template <typename T>
		T get(size_t n) const
		{
			if (n < m_args.size())
				return boost::lexical_cast<T>(m_args[n]);
			else
				throw text_formatter_exception("argument out of bounds");
		}

		template <typename T>
		T get(size_t n, const T& def) const
		{
			if (n < m_args.size())
				return boost::lexical_cast<T>(m_args[n]);
			else
				return def;
		}

		size_t count() const
		{
			return m_args.size();
		}

	private:
		std::vector<std::string> m_args;

		friend class text_formatter;
	};

	class XD_API text_decorator
	{
	public:
		text_decorator(int level);
		virtual ~text_decorator();

		void push_text(const formatted_text& text);
		void push_text(const std::string& text);
		void push_text(const formatted_char& chr);
		void push_text(utf8::uint32_t chr);

		void push_color(const glm::vec4& color);
		void push_alpha(float alpha);
		void push_type(const std::string& type);
		void push_shadow(const font_shadow& shadow);
		void push_outline(const font_outline& outline);
		void push_position(const glm::vec2& position);
		void push_letter_spacing(float letter_spacing);

		void pop_color();
		void pop_alpha();
		void pop_type();
		void pop_shadow();
		void pop_outline();
		void pop_position();
		void pop_letter_spacing();

		template <typename T>
		void push_text(const T& val)
		{
			std::string str = boost::lexical_cast<std::string>(val);
			push_text(str);
		}

	private:
		// current state
		int m_current_level;
		formatted_text m_current_text;
		detail::text_formatter::state_change_list m_current_state_changes;

		// decorate_text needs to be able to modify current state
		friend class detail::text_formatter::decorate_text;
	};

	class XD_API text_formatter
	{
	public:
		typedef std::function<void (text_decorator&, const formatted_text&, const text_decorator_args&)> decorator_callback_t;
		typedef std::function<std::string (const std::string&)> variable_callback_t;

		text_formatter();
		virtual ~text_formatter();

		const std::string& get_decorator_open_delim() const;
		const std::string& get_decorator_close_delim() const;
		const std::string& get_decorator_terminate_delim() const;
		void set_decorator_open_delim(const std::string& delim);
		void set_decorator_close_delim(const std::string& delim);
		void set_decorator_terminate_delim(const std::string& delim);
		void set_decorator_delims(const std::string& open, const std::string& close, const std::string& terminate);
		const std::string& get_variable_open_delim() const;
		const std::string& get_variable_close_delim() const;
		void set_variable_open_delim(const std::string& delim);
		void set_variable_close_delim(const std::string& delim);
		void set_variable_delims(const std::string& open, const std::string& close);

		void register_decorator(const std::string& name, decorator_callback_t decorator);
		void register_variable(const std::string& name, variable_callback_t variable);

		void unregister_decorator(const std::string& name);
		void unregister_variable(const std::string& name);

		void render(const std::string& text, xd::font& font, const xd::font_style& style,
			xd::shader_program& shader, const glm::mat4& mvp);

	private:
		//typedef std::list<detail::text_formatter_token> token_list_t;
		typedef std::unordered_map<std::string, decorator_callback_t> decorator_list_t;
		typedef std::unordered_map<std::string, variable_callback_t> variable_list_t;

		// parse
		void parse(const std::string& text, detail::text_formatter::token_list& tokens);

		// callbacks
		decorator_list_t m_decorators;
		variable_list_t m_variables;

		// delimiters
		std::string m_decorator_open_delim;
		std::string m_decorator_close_delim;
		std::string m_decorator_terminate_delim;
		std::string m_variable_open_delim;
		std::string m_variable_close_delim;
	};
}

#endif
