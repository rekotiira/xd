#ifndef H_XD_GRAPHICS_TEXT_FORMATTER
#define H_XD_GRAPHICS_TEXT_FORMATTER

#include <list>
#include <vector>
#include <string>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <glm/glm.hpp>
#include <xd/graphics/exceptions.h>
#include <xd/graphics/font.h>
#include <xd/graphics/shader_program.h>

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
		struct state_change_value
		{
			T value;
			int level;
		};

		// state changes
		struct state_change_color : state_change_value<glm::vec4> {};
		struct state_change_type : state_change_value<std::string> {};
		struct state_change_shadow : state_change_value<font_shadow> {};

		// variant state change
		typedef boost::variant<
			state_change_color,			
			state_change_type,
			state_change_shadow
		> state_change;

		// token list
		typedef std::list<token> token_list;

		// state change list
		typedef std::list<state_change> state_change_list;

	} }

	class formatted_char
	{
	public:
		formatted_char(char chr)
			: m_chr(chr)
		{
		}

		operator char() const
		{
			return m_chr;
		}

	private:
		char m_chr;
		detail::text_formatter::state_change_list m_state_changes;

		friend class text_decorator;
		friend class text_formatter;
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
			m_chars.insert(m_chars.end(), text.begin(), text.end());
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
			return std::string(m_chars.begin(), m_chars.end());
		}

	private:
		std::vector<formatted_char> m_chars;
		int m_level;

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

	class text_decorator
	{
	public:
		text_decorator(int level);
		~text_decorator();

		void push_text(const formatted_text& text);
		void push_text(const std::string& text);
		void push_text(const formatted_char& chr);
		void push_text(char chr);
		void push_color(const glm::vec4& color);
		void push_type(const std::string& type);
		void push_shadow(const font_shadow& color);

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

	class text_formatter
	{
	public:
		typedef boost::function<void (text_decorator&, const formatted_text&, const text_decorator_args&)> decorator_callback_t;
		typedef boost::function<std::string (const std::string&)> variable_callback_t;

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
		typedef boost::unordered_map<std::string, decorator_callback_t> decorator_list_t;
		typedef boost::unordered_map<std::string, variable_callback_t> variable_list_t;

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
