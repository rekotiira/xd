#ifndef H_XD_GRAPHICS_EXCEPTIONS
#define H_XD_GRAPHICS_EXCEPTIONS

#include <xd/common/exception.h>
#include <boost/lexical_cast.hpp>
#include <string>
#include <sstream>

namespace xd
{
	struct shader_build_failed : exception
	{
		shader_build_failed(const std::string& message)
			: exception("failed to build shader: " + message)
		{
		}
	};

	struct invalid_shader_uniform : exception
	{
		invalid_shader_uniform(const std::string& uniform)
			: exception("invalid_shader_uniform: " + uniform)
		{
		}
	};

	struct font_load_failed : exception
	{
		font_load_failed(const std::string& font)
			: exception("failed to load font: " + font)
		{
		}
	};

	struct glyph_load_failed : exception
	{
		glyph_load_failed(const std::string& font, int char_index)
			: exception("failed to load glyph for character " + boost::lexical_cast<std::string>(char_index) + " for font " + font)
		{
		}
	};

	struct invalid_font_type : exception
	{
		invalid_font_type(const std::string& type)
			: exception("invalid font type " + type)
		{
		}
	};

	struct text_formatter_exception : exception
	{
		text_formatter_exception(const std::string& message)
			: exception("text formatter error: "+message)
		{
		}
	};

	struct text_formatter_parse_exception : text_formatter_exception
	{
		text_formatter_parse_exception(const std::string& input, const std::string& message)
			: text_formatter_exception("failed to parse \""+input+"\": "+message)
		{
		}
	};

	struct text_decorator_exception : text_formatter_exception
	{
		text_decorator_exception(const std::string& message)
			: text_formatter_exception("decoration failed: "+message)
		{
		}
	};
}

#endif

