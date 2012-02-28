#ifndef H_XD_GRAPHICS_DETAIL_SPRITE_BATCH
#define H_XD_GRAPHICS_DETAIL_SPRITE_BATCH

#include <xd/graphics/vertex_traits.hpp>

namespace xd { namespace detail {

	struct sprite_batch_data;

	struct sprite_vertex
	{
		glm::vec2 pos;
		glm::vec2 texpos;
	};

	struct sprite_vertex_traits : vertex_traits<sprite_vertex>
	{
		sprite_vertex_traits()
		{
			bind_vertex_attribute(VERTEX_POSITION, &sprite_vertex::pos);
			bind_vertex_attribute(VERTEX_TEXTURE, &sprite_vertex::texpos);
		}
	};

} }

#endif
