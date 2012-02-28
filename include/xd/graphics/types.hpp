#ifndef H_XD_GRAPHICS_TYPES
#define H_XD_GRAPHICS_TYPES

#include <xd/vendor/glm/glm.hpp>

namespace xd
{
	struct rect
	{
		rect()
			: x(0), y(0), w(0), h(0)
		{}

		explicit rect(float x, float y, float w, float h)
			: x(x), y(y), w(w), h(h)
		{}

		explicit rect(glm::vec2 xy, glm::vec2 wh)
			: x(xy.x), y(xy.y), w(wh.x), h(wh.y)
		{}

		explicit rect(glm::vec4 rect)
			: x(rect[0]), y(rect[1]), w(rect[2]), h(rect[3])
		{}

		// conversion constructor
		template <typename T>
		explicit rect (const T& x, const T& y, const T& w, const T& h)
			: x(static_cast<float>(x))
			, y(static_cast<float>(y))
			, w(static_cast<float>(w))
			, h(static_cast<float>(h))
		{}

		float x, y, w, h;
	};
}

#endif