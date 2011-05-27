#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <utility>
#include <fstream>
#include <list>
#include <cctype>
#include <boost/lexical_cast.hpp>
#include <xd/graphics/exceptions.h>
#include <xd/graphics/text_formatter.h>
#include <iostream>

namespace xd { namespace detail { namespace text_formatter {

	// formatted text list
	typedef std::list<formatted_text> formatted_text_list;

	// token types
	struct token_text
	{
		std::string text;
	};

	struct token_variable
	{
		std::string name;
		xd::text_formatter::variable_callback_t callback;
	};

	struct token_open_decorator
	{
		std::string name;
		text_decorator_args args;
		xd::text_formatter::decorator_callback_t callback;
	};

	struct token_close_decorator
	{
		std::string name;
	};

	// expand variables
	class expand_variables : public boost::static_visitor<>
	{
	public:
		expand_variables(token_list& tokens)
			: m_tokens(tokens)
		{
		}

		void operator()(const token_text& tok)
		{
			// if we have tokens
			if (m_tokens.size() != 0) {
				// if previous token is text
				token_text *prev_tok = boost::get<token_text>(&m_tokens.back());
				if (prev_tok) {
					// append variable to the previous token
					prev_tok->text += tok.text;
					return;
				}
			}

			// previous token wasn't text, push the value as new token
			m_tokens.push_back(tok);
		}

		void operator()(const token_variable& tok)
		{
			// evaluate the variable
			std::string value = tok.callback(tok.name);

			// if we have tokens
			if (m_tokens.size() != 0) {
				// if previous token is text
				token_text *prev_tok = boost::get<token_text>(&m_tokens.back());
				if (prev_tok) {
					// append variable to the previous token
					prev_tok->text += value;
					return;
				}
			}

			// previous token wasn't text, push the value as new token
			token_text new_tok;
			new_tok.text = value;
			m_tokens.push_back(new_tok);
		}

		void operator()(const token_open_decorator& tok)
		{
			// don't do anything to decorators
			m_tokens.push_back(tok);
		}

		void operator()(const token_close_decorator& tok)
		{
			// don't do anything to decorators
			m_tokens.push_back(tok);
		}

	private:
		token_list& m_tokens;
	};

	// decorate text
	class decorate_text : public boost::static_visitor<>
	{
	public:
		decorate_text(formatted_text_list& texts)
			: m_texts(texts)
			, m_prev_close_decorator(false)
			, m_level(0)
		{
		}

		void operator()(const token_text& tok)
		{
			// text node, create a formatted version of it
			formatted_text text(tok.text);
			text.m_level = m_level;
			m_texts.push_front(text);
			m_prev_close_decorator = false;
		}

		void operator()(const token_variable& tok)
		{
			// do nothing; variables are already expanded
		}

		void operator()(const token_open_decorator& tok)
		{
			// if there are no associated text node, create an empty one
			if (m_prev_close_decorator)
				m_texts.push_front(formatted_text());

			// decorate the associated text node
			formatted_text text = m_texts.front();
			xd::text_decorator decorator(m_level);
			m_texts.pop_front();
			tok.callback(decorator, text, tok.args);
			if (decorator.m_current_text.length()) {
				decorator.m_current_text.m_level = m_level;
				m_texts.push_front(decorator.m_current_text);
			}
			m_prev_close_decorator = false;

			// tokens are iterated in reverse order, hence we decrement
			m_level--;
		}

		void operator()(const token_close_decorator& tok)
		{
			// tokens are iterated in reverse order, hence we increment
			m_level++;
			m_prev_close_decorator = true;
		}

	private:
		formatted_text_list& m_texts;
		bool m_prev_close_decorator;
		int m_level;
	};

	struct stacked_font_style
	{
		stacked_font_style(const font_style& initial_state)
		{
			state_change_color color;
			color.value = initial_state.color;
			color.level = 0;
			colors.push_back(color);

			if (initial_state.type) {
				state_change_type type;
				type.value = *initial_state.type;
				type.level = 0;
				types.push_back(type);
			}

			if (initial_state.shadow) {
				state_change_shadow shadow;
				shadow.value = *initial_state.shadow;
				shadow.level = 0;
				shadows.push_back(shadow);
			}
		}

		font_style get_font_style()
		{
			font_style style;
			style.color = colors.back().value;
			if (types.size() != 0)
				style.type = types.back().value;
			if (shadows.size() != 0)
				style.shadow = shadows.back().value;
			return style;
		}

		template <typename T, typename V>
		void push(std::list<T>& list, const V& value, int level)
		{
			int current_level = (list.size() != 0 ? list.back().level : -1);
			if (level > current_level) {
				T state_change;
				state_change.value = value;
				state_change.level = level;
				list.push_back(state_change);
			} else if (level == current_level) {
				list.back().value = value;
			}
		}

		void push_color(const glm::vec4& value, int level)
		{
			push(colors, value, level);
		}

		void push_type(const std::string& value, int level)
		{
			push(types, value, level);
		}

		void push_shadow(const font_shadow& value, int level)
		{
			push(shadows, value, level);
		}

		template <typename T>
		void pop(std::list<T>& list, int level)
		{
			std::list<T>::iterator i = list.begin();
			while (i != list.end()) {
				if (i->level > level)
					i = list.erase(i);
				else
					++i;
			}
		}

		void purge(int level)
		{
			pop(colors, level);
			pop(types, level);
			pop(shadows, level);
		}

		std::list<state_change_color> colors;
		std::list<state_change_type> types;
		std::list<state_change_shadow> shadows;
	};


	class apply_state_changes : public boost::static_visitor<>
	{
	public:
		apply_state_changes(stacked_font_style& style_stack)
			: m_style_stack(style_stack)
		{
		}

		void operator()(const state_change_color& state_change)
		{
			m_style_stack.push_color(state_change.value, state_change.level);
		}

		void operator()(const state_change_type& state_change)
		{
			m_style_stack.push_type(state_change.value, state_change.level);
		}

		void operator()(const state_change_shadow& state_change)
		{
			m_style_stack.push_shadow(state_change.value, state_change.level);
		}

	private:
		stacked_font_style& m_style_stack;
	};

} } }

xd::text_decorator::text_decorator(int level)
	: m_current_level(level)
{
}

xd::text_decorator::~text_decorator()
{
}

void xd::text_decorator::push_text(const xd::formatted_text& text)
{
	if (text.length() != 0) {
		if (m_current_state_changes.size() != 0) {
			formatted_text format_text = text;
			detail::text_formatter::state_change_list&
				state_changes = format_text.begin()->m_state_changes;
			state_changes.insert(state_changes.begin(), m_current_state_changes.begin(), m_current_state_changes.end());
			m_current_text += format_text;
			m_current_state_changes.clear();
		} else {
			m_current_text += text;
		}
	}
}

void xd::text_decorator::push_text(const std::string& text)
{
	push_text(formatted_text(text));
}

void xd::text_decorator::push_text(char chr)
{
	formatted_text text;
	text += chr;
	push_text(text);
}

void xd::text_decorator::push_color(const glm::vec4& value)
{
	detail::text_formatter::state_change_color state_change;
	state_change.value = value;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::push_type(const std::string& value)
{
	detail::text_formatter::state_change_type state_change;
	state_change.value = value;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::push_shadow(const font_shadow& value)
{
	detail::text_formatter::state_change_shadow state_change;
	state_change.value = value;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

xd::text_formatter::text_formatter()
	: m_decorator_open_delim("{")
	, m_decorator_close_delim("}")
	, m_decorator_terminate_delim("/")
	, m_variable_open_delim("${")
	, m_variable_close_delim("}")
{
}

xd::text_formatter::~text_formatter()
{
}

const std::string& xd::text_formatter::get_decorator_open_delim() const
{
	return m_decorator_open_delim;
}

const std::string& xd::text_formatter::get_decorator_close_delim() const
{
	return m_decorator_close_delim;
}

const std::string& xd::text_formatter::get_decorator_terminate_delim() const
{
	return m_decorator_terminate_delim;
}

void xd::text_formatter::set_decorator_open_delim(const std::string& delim)
{
	m_decorator_open_delim = delim;
}

void xd::text_formatter::set_decorator_close_delim(const std::string& delim)
{
	m_decorator_close_delim = delim;
}

void xd::text_formatter::set_decorator_terminate_delim(const std::string& delim)
{
	m_decorator_terminate_delim = delim;
}

void xd::text_formatter::set_decorator_delims(const std::string& open, const std::string& close, const std::string& terminate)
{
	m_decorator_open_delim = open;
	m_decorator_close_delim = close;
	m_decorator_terminate_delim = terminate;
}

const std::string& xd::text_formatter::get_variable_open_delim() const
{
	return m_variable_open_delim;
}

const std::string& xd::text_formatter::get_variable_close_delim() const
{
	return m_variable_close_delim;
}

void xd::text_formatter::set_variable_open_delim(const std::string& delim)
{
	m_variable_open_delim = delim;
}

void xd::text_formatter::set_variable_close_delim(const std::string& delim)
{
	m_variable_close_delim = delim;
}

void xd::text_formatter::register_decorator(const std::string& name, xd::text_formatter::decorator_callback_t decorator)
{
	// make sure it's not already registered
	decorator_list_t::iterator i = m_decorators.find(name);
	if (i != m_decorators.end()) {
		throw text_formatter_exception("decorator \""+name+"\" already registered");
	}

	m_decorators[name] = decorator;
}

void xd::text_formatter::register_variable(const std::string& name, xd::text_formatter::variable_callback_t variable)
{
	// make sure it's not already registered
	variable_list_t::iterator i = m_variables.find(name);
	if (i != m_variables.end()) {
		throw text_formatter_exception("variable \""+name+"\" already registered");
	}

	m_variables[name] = variable;
}

void xd::text_formatter::unregister_decorator(const std::string& name)
{
	// make sure it's registered
	decorator_list_t::iterator i = m_decorators.find(name);
	if (i == m_decorators.end()) {
		throw text_formatter_exception("variable \""+name+"\" not registered");
	}

	m_decorators.erase(i);
}

void xd::text_formatter::unregister_variable(const std::string& name)
{
	// make sure it's registered
	variable_list_t::iterator i = m_variables.find(name);
	if (i == m_variables.end()) {
		throw text_formatter_exception("variable \""+name+"\" not registered");
	}

	m_variables.erase(i);
}

void xd::text_formatter::set_variable_delims(const std::string& open, const std::string& close)
{
	m_variable_open_delim = open;
	m_variable_close_delim = close;
}

void xd::text_formatter::render(const std::string& text, xd::font& font, const xd::font_style& style,
	xd::shader_program& shader, const glm::mat4& mvp)
{
	// parse the input
	detail::text_formatter::token_list tokens;
	parse(text, tokens);

	// first pass: replace variables and concatenate strings
	detail::text_formatter::token_list expanded_tokens;
	detail::text_formatter::expand_variables expand_variables_step(expanded_tokens);
	std::for_each(tokens.begin(), tokens.end(), boost::apply_visitor(expand_variables_step));

	// second pass: iterate through the tokens in reverse order, decorating the text
	detail::text_formatter::formatted_text_list texts;
	detail::text_formatter::decorate_text decorate_text_step(texts);
	std::for_each(expanded_tokens.rbegin(), expanded_tokens.rend(), boost::apply_visitor(decorate_text_step));

	// render text
	detail::text_formatter::stacked_font_style style_stack(style);
	glm::vec2 pos;
	for (detail::text_formatter::formatted_text_list::iterator i = texts.begin(); i != texts.end(); ++i) {
		// purge all styles above this level
		style_stack.purge(i->m_level);

		// iterate through each char until a style change is met
		std::string current_str;
		for (formatted_text::iterator j = i->begin(); j != i->end(); ++j) {
			if (j->m_state_changes.size()) {
				// draw the current string using current style
				if (current_str.length() != 0) {
					font.render(current_str, style_stack.get_font_style(), shader, mvp, &pos);
					current_str.clear();
				}

				// process the styles
				detail::text_formatter::apply_state_changes change_current_style(style_stack);
				std::for_each(j->m_state_changes.begin(), j->m_state_changes.end(), boost::apply_visitor(change_current_style));
			}

			// add char to the current string
			current_str += *j;
		}

		// draw the rest of the string
		if (current_str.length() != 0)
			font.render(current_str, style_stack.get_font_style(), shader, mvp, &pos);
	}
}

void xd::text_formatter::parse(const std::string& text, detail::text_formatter::token_list& tokens)
{
	// parser state variables
	std::list<std::string> open_decorators;
	std::string current_text;

	// parse!
	std::string::const_iterator start = text.begin(), end = text.end();
	while (start != end) {
		// check if we have an opening decorator delimiter
		if (std::equal(m_decorator_open_delim.begin(), m_decorator_open_delim.end(), start)) {
			// push current text node
			if (current_text.length() != 0) {
				detail::text_formatter::token_text tok;
				tok.text = current_text;
				tokens.push_back(tok);
				current_text = "";
			}

			// consume the delimiter
			start += m_decorator_open_delim.length();

			// check is this opening or closing tag
			bool open_decorator;
			if (std::equal(m_decorator_terminate_delim.begin(), m_decorator_terminate_delim.end(), start)) {
				// closing tag
				start += m_decorator_terminate_delim.length();
				open_decorator = false;
			} else {
				open_decorator = true;
			}

			// get the decorator name
			std::string decorator_name;
			while (start != end && (*start != '=' || !open_decorator)
				&& !std::equal(m_decorator_close_delim.begin(), m_decorator_close_delim.end(), start))
			{
				decorator_name += *start;
				++start;
			}

			// check if we have parameters
			text_decorator_args args;
			if (start != end && *start == '=' && open_decorator) {
				// consume the assignment sign
				++start;

				// parse arguments
				std::string arg;
				while (start != end && !std::equal(m_decorator_close_delim.begin(), m_decorator_close_delim.end(), start)) {
					if (*start == ',') {
						// push and reset the argument
						args.m_args.push_back(arg);
						arg = "";
					} else {
						// aggregate the argument
						arg += *start;
					}

					++start;
				}

				// push the last arg
				args.m_args.push_back(arg);
			}

			// closing delimiter not found
			if (start == end) {
				throw text_formatter_parse_exception(text, "closing delimiter for decorator \""+decorator_name+"\" not found");
			}

			// make sure it's registered
			decorator_list_t::iterator decorator_pos = m_decorators.find(decorator_name);
			if (decorator_pos == m_decorators.end()) {
				throw text_formatter_parse_exception(text, "decorator \""+decorator_name+"\" not registered");
			}

			// tag closed
			start += m_decorator_close_delim.length();

			// push the tag in tokens
			if (open_decorator) {
				// push the open decorator in open decorators list
				open_decorators.push_back(decorator_name);

				// push the token
				detail::text_formatter::token_open_decorator tok;
				tok.name = decorator_name;
				tok.args = args;
				tok.callback = decorator_pos->second;
				tokens.push_back(tok);
			} else {
				// check that tags are closed in correct order
				if (open_decorators.size() == 0) {
					throw text_formatter_parse_exception(text, "decorator \""+decorator_name+"\" closed without opening tag");
				} else if (open_decorators.back() != decorator_name) {
					throw text_formatter_parse_exception(text, "decorator \""+decorator_name+"\" closed out of order, expected \""+open_decorators.back()+"\"");
				}

				// pop the open decorator
				open_decorators.pop_back();

				// push the token
				detail::text_formatter::token_close_decorator tok;
				tok.name = decorator_name;
				tokens.push_back(tok);
			}

			continue;
		}

		// check if we have an opening variable delimiter
		if (std::equal(m_variable_open_delim.begin(), m_variable_open_delim.end(), start)) {
			// push current text node
			if (current_text.length() != 0) {
				detail::text_formatter::token_text tok;
				tok.text = current_text;
				tokens.push_back(tok);
				current_text = "";
			}

			// consume the delimiter
			start += m_variable_open_delim.length();

			// get the tag name
			std::string variable_name;
			while (start != end && !std::equal(m_variable_close_delim.begin(), m_variable_close_delim.end(), start)) {
				variable_name += *start;
				++start;
			}

			// closing delimiter not found
			if (start == end) {
				throw text_formatter_parse_exception(text, "closing delimiter for variable \""+variable_name+"\" not found");
			}

			// make sure it's registered
			variable_list_t::iterator variable_pos = m_variables.find(variable_name);
			if (variable_pos == m_variables.end()) {
				throw text_formatter_parse_exception(text, "variable \""+variable_name+"\" not registered");
			}

			// tag closed
			start += m_variable_close_delim.length();

			// push the token
			detail::text_formatter::token_variable tok;
			tok.name = variable_name;
			tok.callback = variable_pos->second;
			tokens.push_back(tok);

			continue;
		}

		current_text += *start;
		++start;
	}

	// check if any decorators were left open
	if (open_decorators.size() != 0) {
		throw text_formatter_parse_exception(text, "unclosed decorator \""+open_decorators.back()+"\"");
	}

	// push the last text node if one exists at this point
	if (current_text.length() != 0) {
		detail::text_formatter::token_text tok;
		tok.text = current_text;
		tokens.push_back(tok);
	}
}
