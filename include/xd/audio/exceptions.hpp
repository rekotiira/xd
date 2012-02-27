#ifndef H_XD_GRAPHICS_EXCEPTIONS
#define H_XD_GRAPHICS_EXCEPTIONS

#include <xd/exception.hpp>
#include <string>

namespace xd
{
	struct audio_file_load_failed : exception
	{
		audio_file_load_failed(const std::string& filename)
			: exception("failed to load audio: " + filename)
		{
		}
	};
}

#endif

