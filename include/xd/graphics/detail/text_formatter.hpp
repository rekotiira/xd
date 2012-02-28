#ifndef H_XD_GRAPHICS_DETAIL_TEXT_FORMATTER
#define H_XD_GRAPHICS_DETAIL_TEXT_FORMATTER

#include <xd/graphics/font.hpp>
#include <boost/variant.hpp>
#include <list>

namespace xd { namespace detail { namespace text_formatter {

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

} } }

#endif
