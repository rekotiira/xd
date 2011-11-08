#ifndef H_XD_GRAPHICS_TEXT_FORMATTER
#define H_XD_GRAPHICS_TEXT_FORMATTER

#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <list>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <glm/glm.hpp>
#include <xd/graphics/exceptions.h>
#include <xd/graphics/font.h>
#include <xd/graphics/shader_program.h>
#include <xd/utf8.h>

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

	namespace detail { namespace text_formatter {

		// text decorator visitor
		class decorate_text;

		// tokens
		struct token_text;
		struct token_variable;
		struct token_open_decorator;
		struct token_close_decorator;

		// variant token
		typedef boost::variant<
			token_text,
			token_variable,
			token_open_decorator,
			token_close_decorator
		> token;

		// nested value
		template <typename T>
		struct nested_value
		{
			T value;
			int level;
		};

		// no value attached
		template <>
		struct nested_value<void>
		{
			int level;
		};

		// typedefs for styles
		typedef nested_value<glm::vec4> nested_color;
		typedef nested_value<float> nested_alpha;
		typedef nested_value<std::string> nested_type;
		typedef nested_value<font_shadow> nested_shadow;
		typedef nested_value<font_outline> nested_outline;
		typedef nested_value<glm::vec2> nested_position;
		typedef nested_value<float> nested_letter_spacing;
		typedef nested_value<void> nested_void;

		// state changes
		struct state_change_push_color : nested_color {};
		struct state_change_push_alpha : nested_alpha {};
		struct state_change_push_type : nested_type {};
		struct state_change_push_shadow : nested_shadow {};
		struct state_change_push_outline : nested_outline {};
		struct state_change_push_position : nested_position {};
		struct state_change_push_letter_spacing : nested_letter_spacing {};
		struct state_change_pop_color : nested_void {};
		struct state_change_pop_alpha : nested_void {};
		struct state_change_pop_type : nested_void {};
		struct state_change_pop_shadow : nested_void {};
		struct state_change_pop_outline : nested_void {};
		struct state_change_pop_position : nested_void {};
		struct state_change_pop_letter_spacing : nested_void {};

		// variant state change
		typedef boost::variant<
			state_change_push_color,
			state_change_push_alpha,
			state_change_push_type,
			state_change_push_shadow,
			state_change_push_outline,
			state_change_push_position,
			state_change_push_letter_spacing,
			state_change_pop_color,
			state_change_pop_alpha,
			state_change_pop_type,
			state_change_pop_shadow,
			state_change_pop_outline,
			state_change_pop_position,
			state_change_pop_letter_spacing
		> state_change;

		// token list
		typedef std::list<token> token_list;

		// state change list
		typedef std::list<state_change> state_change_list;

	} }

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
		~text_decorator();

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
