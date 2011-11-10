#include <string>
#include <algorithm>
#include <stdexcept>
#include <vector>
#include <utility>
#include <list>
#include <cctype>
#include <boost/lexical_cast.hpp>
#include <xd/graphics/exceptions.hpp>
#include <xd/graphics/text_formatter.hpp>
#include <xd/utf8.h>
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
			// set level for each character
			formatted_text text(tok.text);
			for (formatted_text::iterator i = text.begin(); i != text.end(); ++i) {
				i->m_level = m_level;
			}

			// text node, create a formatted version of it
			if (m_texts.size() && !m_prev_close_decorator) {
				m_texts.front().m_chars.insert(m_texts.front().m_chars.begin(),
					text.begin(), text.end());
			} else {
				m_texts.push_front(text);
			}

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
				// set level for each character
				formatted_text::iterator i = decorator.m_current_text.begin();
				while (i != decorator.m_current_text.end()) {
					if (m_level > i->m_level)
						i->m_level = m_level;
					++i;
				}

				// if we're inside another decorator, concatenate with previous string
				if (m_texts.size() && m_texts.front().begin()->m_level == (m_level-1)) {
					m_texts.front().m_chars.insert(m_texts.front().m_chars.begin(),
						decorator.m_current_text.begin(), decorator.m_current_text.end());
				} else {
					m_texts.push_front(decorator.m_current_text);
				}

			} else {
				m_texts.push_front(formatted_text());
			}

			// tokens are iterated in reverse order, hence we decrement
			m_prev_close_decorator = false;
			m_level--;
		}

		void operator()(const token_close_decorator& tok)
		{
			// tokens are iterated in reverse order, hence we increment
			m_prev_close_decorator = true;
			m_level++;
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
			nested_color color;
			color.value = initial_state.color;
			color.level = 0;
			colors.push_back(color);

			nested_letter_spacing letter_spacing;
			letter_spacing.value = initial_state.letter_spacing;
			letter_spacing.level = 0;
			letter_spacings.push_back(letter_spacing);

			if (initial_state.type) {
				nested_type type;
				type.value = *initial_state.type;
				type.level = 0;
				types.push_back(type);
			}

			if (initial_state.shadow) {
				nested_shadow shadow;
				shadow.value = *initial_state.shadow;
				shadow.level = 0;
				shadows.push_back(shadow);
			}

			if (initial_state.outline) {
				nested_outline outline;
				outline.value = *initial_state.outline;
				outline.level = 0;
				outlines.push_back(outline);
			}
		}

		font_style get_font_style()
		{
			font_style style;
			style.color = colors.back().value;
			style.letter_spacing = letter_spacings.back().value;
			if (alphas.size() != 0)
				style.color.a *= alphas.back().value;
			if (types.size() != 0)
				style.type = types.back().value;
			if (shadows.size() != 0)
				style.shadow = shadows.back().value;
			if (outlines.size() != 0)
				style.outline = outlines.back().value;
			return style;
		}

		template <typename T, typename V>
		void push(std::list<T>& list, const V& value, int level)
		{
			int current_level = (list.size() != 0 ? list.back().level : -1);
			if (level >= current_level) {
				T state_change;
				state_change.value = value;
				state_change.level = level;
				list.push_back(state_change);
			}
		}

		void push_color(const glm::vec4& value, int level)
		{
			if (colors.back().value.a != 1.0f) {
				glm::vec4 color = value;
				color.a *= colors.back().value.a;
				push(colors, color, level);
			} else {
				push(colors, value, level);
			}
		}

		void push_alpha(float value, int level)
		{
			if (alphas.size() != 0)
				push(alphas, value * alphas.back().value, level);
			else
				push(alphas, value, level);
		}

		void push_type(const std::string& value, int level)
		{
			push(types, value, level);
		}

		void push_shadow(const font_shadow& value, int level)
		{
			push(shadows, value, level);
		}

		void push_outline(const font_outline& value, int level)
		{
			push(outlines, value, level);
		}

		void push_position(const glm::vec2& value, int level)
		{
			if (positions.size() != 0)
				push(positions, value + positions.back().value, level);
			else
				push(positions, value, level);
		}

		void push_letter_spacing(float value, int level)
		{
			if (letter_spacings.size() != 0)
				push(letter_spacings, value, level);
			else
				push(letter_spacings, value, level);
		}

		template <typename T>
		void pop(std::list<T>& list, int level)
		{
			if (list.size() == 0 || list.back().level != level)
				throw xd::text_formatter_exception("no value to pop");
			list.pop_back();
		}

		void pop_color(int level)
		{
			pop(colors, level);
		}

		void pop_alpha(int level)
		{
			pop(alphas, level);
		}

		void pop_type(int level)
		{
			pop(types, level);
		}

		void pop_shadow(int level)
		{
			pop(shadows, level);
		}

		void pop_outline(int level)
		{
			pop(outlines, level);
		}

		void pop_position(int level)
		{
			pop(positions, level);
		}

		void pop_letter_spacing(int level)
		{
			pop(letter_spacings, level);
		}

		template <typename T>
		void pop_level(std::list<T>& list, int level)
		{
			typename std::list<T>::iterator i = list.begin();
			while (i != list.end()) {
				if (i->level > level)
					i = list.erase(i);
				else
					++i;
			}
		}

		void pop_level(int level)
		{
			pop_level(colors, level);
			pop_level(alphas, level);
			pop_level(types, level);
			pop_level(shadows, level);
			pop_level(outlines, level);
			pop_level(positions, level);
			pop_level(letter_spacings, level);
		}

		std::list<nested_color> colors;
		std::list<nested_alpha> alphas;
		std::list<nested_type> types;
		std::list<nested_shadow> shadows;
		std::list<nested_outline> outlines;
		std::list<nested_position> positions;
		std::list<nested_letter_spacing> letter_spacings;
	};


	class apply_state_changes : public boost::static_visitor<>
	{
	public:
		apply_state_changes(stacked_font_style& style_stack)
			: m_style_stack(style_stack)
		{
		}

		void operator()(const state_change_push_color& state_change)
		{
			m_style_stack.push_color(state_change.value, state_change.level);
		}

		void operator()(const state_change_push_alpha& state_change)
		{
			m_style_stack.push_alpha(state_change.value, state_change.level);
		}

		void operator()(const state_change_push_type& state_change)
		{
			m_style_stack.push_type(state_change.value, state_change.level);
		}

		void operator()(const state_change_push_shadow& state_change)
		{
			m_style_stack.push_shadow(state_change.value, state_change.level);
		}

		void operator()(const state_change_push_outline& state_change)
		{
			m_style_stack.push_outline(state_change.value, state_change.level);
		}

		void operator()(const state_change_push_position& state_change)
		{
			m_style_stack.push_position(state_change.value, state_change.level);
		}

		void operator()(const state_change_push_letter_spacing& state_change)
		{
			m_style_stack.push_letter_spacing(state_change.value, state_change.level);
		}

		void operator()(const state_change_pop_color& state_change)
		{
			m_style_stack.pop_color(state_change.level);
		}

		void operator()(const state_change_pop_alpha& state_change)
		{
			m_style_stack.pop_alpha(state_change.level);
		}

		void operator()(const state_change_pop_type& state_change)
		{
			m_style_stack.pop_type(state_change.level);
		}

		void operator()(const state_change_pop_shadow& state_change)
		{
			m_style_stack.pop_shadow(state_change.level);
		}

		void operator()(const state_change_pop_outline& state_change)
		{
			m_style_stack.pop_outline(state_change.level);
		}

		void operator()(const state_change_pop_position& state_change)
		{
			m_style_stack.pop_position(state_change.level);
		}

		void operator()(const state_change_pop_letter_spacing& state_change)
		{
			m_style_stack.pop_letter_spacing(state_change.level);
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

void xd::text_decorator::push_text(const formatted_char& chr)
{
	formatted_text text;
	text += chr;
	push_text(text);
}

void xd::text_decorator::push_text(utf8::uint32_t chr)
{
	push_text(formatted_char(chr));
}

void xd::text_decorator::push_color(const glm::vec4& value)
{
	detail::text_formatter::state_change_push_color state_change;
	state_change.value = value;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::push_alpha(float value)
{
	detail::text_formatter::state_change_push_alpha state_change;
	state_change.value = value;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::push_type(const std::string& value)
{
	detail::text_formatter::state_change_push_type state_change;
	state_change.value = value;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::push_shadow(const font_shadow& value)
{
	detail::text_formatter::state_change_push_shadow state_change;
	state_change.value = value;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::push_outline(const font_outline& value)
{
	detail::text_formatter::state_change_push_outline state_change;
	state_change.value = value;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::push_position(const glm::vec2& value)
{
	detail::text_formatter::state_change_push_position state_change;
	state_change.value = value;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::push_letter_spacing(float value)
{
	detail::text_formatter::state_change_push_letter_spacing state_change;
	state_change.value = value;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::pop_color()
{
	detail::text_formatter::state_change_pop_color state_change;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::pop_alpha()
{
	detail::text_formatter::state_change_pop_alpha state_change;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::pop_type()
{
	detail::text_formatter::state_change_pop_type state_change;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::pop_shadow()
{
	detail::text_formatter::state_change_pop_shadow state_change;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::pop_outline()
{
	detail::text_formatter::state_change_pop_outline state_change;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::pop_position()
{
	detail::text_formatter::state_change_pop_position state_change;
	state_change.level = m_current_level;
	m_current_state_changes.push_back(state_change);
}

void xd::text_decorator::pop_letter_spacing()
{
	detail::text_formatter::state_change_pop_letter_spacing state_change;
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
	int current_level = 0;
	for (detail::text_formatter::formatted_text_list::iterator i = texts.begin(); i != texts.end(); ++i) {
		// iterate through each char until a style change is met
		std::string current_str;

		for (formatted_text::iterator j = i->begin(); j != i->end(); ++j) {
			if (j->m_state_changes.size() || j->m_level < current_level) {
				// draw the current string using current style
				if (current_str.length() != 0) {
					if (style_stack.positions.size() != 0)
						pos += style_stack.positions.back().value;
					font.render(current_str, style_stack.get_font_style(), shader, mvp, &pos);
					if (style_stack.positions.size() != 0)
						pos -= style_stack.positions.back().value;
					current_str.clear();
				}

				// process the styles
				detail::text_formatter::apply_state_changes change_current_style(style_stack);
				std::for_each(j->m_state_changes.begin(), j->m_state_changes.end(), boost::apply_visitor(change_current_style));
			}

			// purge styles from the previous level if the new level is smaller
			if (j->m_level < current_level) {
				style_stack.pop_level(j->m_level);
			}

			// add char to the current string and keep track of current level
			current_level = j->m_level;
			//current_str += *j;
			utf8::append(j->m_chr, std::back_inserter(current_str));
		}

		// draw the rest of the string
		if (current_str.length() != 0) {
			if (style_stack.positions.size() != 0)
				pos += style_stack.positions.back().value;
			font.render(current_str, style_stack.get_font_style(), shader, mvp, &pos);
		}
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
			//start += m_decorator_open_delim.length();
			utf8::advance(start, utf8::distance(m_decorator_open_delim.begin(), m_decorator_open_delim.end()), end);

			// check is this opening or closing tag
			bool open_decorator;
			if (std::equal(m_decorator_terminate_delim.begin(), m_decorator_terminate_delim.end(), start)) {
				// closing tag
				//start += m_decorator_terminate_delim.length();
				utf8::advance(start, utf8::distance(m_decorator_terminate_delim.begin(), m_decorator_terminate_delim.end()), end);
				open_decorator = false;
			} else {
				open_decorator = true;
			}

			// get the decorator name
			std::string decorator_name;
			while (start != end && (*start != '=' || !open_decorator)
				&& !std::equal(m_decorator_close_delim.begin(), m_decorator_close_delim.end(), start))
			{
				//decorator_name += *start;
				//++start;
				utf8::append(utf8::next(start, end), std::back_inserter(decorator_name));
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
						++start;
					} else {
						// aggregate the argument
						// arg += *start;
						utf8::append(utf8::next(start, end), std::back_inserter(arg));
					}

					//++start;
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
			//start += m_decorator_close_delim.length();
			utf8::advance(start, utf8::distance(m_decorator_close_delim.begin(), m_decorator_close_delim.end()), end);

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
			//start += m_variable_open_delim.length();
			utf8::advance(start, utf8::distance(m_variable_open_delim.begin(), m_variable_open_delim.end()), end);

			// get the tag name
			std::string variable_name;
			while (start != end && !std::equal(m_variable_close_delim.begin(), m_variable_close_delim.end(), start)) {
				//variable_name += *start;
				//++start;
				utf8::append(utf8::next(start, end), std::back_inserter(variable_name));
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
			//start += m_variable_close_delim.length();
			utf8::advance(start, utf8::distance(m_variable_close_delim.begin(), m_variable_close_delim.end()), end);

			// push the token
			detail::text_formatter::token_variable tok;
			tok.name = variable_name;
			tok.callback = variable_pos->second;
			tokens.push_back(tok);

			continue;
		}

		//current_text += *start;
		//++start;
		utf8::append(utf8::next(start, end), std::back_inserter(current_text));
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
