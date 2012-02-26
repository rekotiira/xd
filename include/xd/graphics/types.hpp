#ifndef H_XD_GRAPHICS_TYPES
#define H_XD_GRAPHICS_TYPES

#include <xd/vendor/glm/glm.hpp>

namespace xd
{
	enum vertex_attr
	{
		VERTEX_ATTR_POSITION = 0,
		VERTEX_ATTR_COLOR,
		VERTEX_ATTR_TEXTURE
	};

	struct rect
	{
		rect(float x, float y, float w, float h)
			: x(x), y(y), w(w), h(h)
		{}
		
		rect(glm::vec2 xy, glm::vec2 wh)
			: x(xy.x), y(xy.y), w(wh.x), h(wh.y)
		{}

		rect(glm::vec4 rect)
			: x(rect[0]), y(rect[1]), w(rect[2]), h(rect[3])
		{}

		float x, y, w, h;
	};
}

#endif