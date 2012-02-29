#ifndef H_XD_SYSTEM_OPTIONS
#define H_XD_SYSTEM_OPTIONS

namespace xd
{
	struct window_options
	{
		window_options()
			: fullscreen(false)
			, allow_resize(false)
			, display_cursor(true)
			, depth_bits(8)
			, stencil_bits(0)
			, antialiasing_level(0)
			, major_version(2)
			, minor_version(0)
		{}

		window_options(bool fullscreen, bool allow_resize, bool display_cursor, int depth_bits,
			int antialiasing_level, int major_version, int minor_version)
			: fullscreen(fullscreen)
			, allow_resize(allow_resize)
			, display_cursor(display_cursor)
			, depth_bits(depth_bits)
			, stencil_bits(stencil_bits)
			, antialiasing_level(antialiasing_level)
			, major_version(major_version)
			, minor_version(minor_version)
		{}

		bool fullscreen;
		bool allow_resize;
		bool display_cursor;
		int depth_bits;
		int stencil_bits;
		int antialiasing_level;
		int major_version;
		int minor_version;
	};
}

#endif
