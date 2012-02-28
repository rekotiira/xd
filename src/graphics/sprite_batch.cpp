#include <xd/graphics/sprite_batch.hpp>
#include <xd/graphics/texture.hpp>
#include <deque>

namespace xd { namespace detail {

	struct sprite
	{
		texture::ptr texture;
		float x, y;
		float rotation;
		glm::vec2 scale;
		glm::vec2 anchor;
	};

	struct sprite_list
	{
		std::deque<sprite> sprites;
	};

} }

xd::sprite_batch::sprite_batch(xd::transform_geometry& geometry_pipeline)
	: m_geometry_pipeline(geometry_pipeline)
	, m_sprites(new detail::sprite_list)
	, m_scale(1)
{
}

xd::sprite_batch::~sprite_batch()
{
	delete m_sprites;
}

void xd::sprite_batch::clear()
{
	m_sprites->sprites.clear();
}

void xd::sprite_batch::draw()
{
}

void xd::sprite_batch::set_scale(float scale)
{
	m_scale = scale;
}

float xd::sprite_batch::get_scale()
{
	return m_scale;
}

void xd::sprite_batch::add(xd::texture::ptr texture, float x, float y, const glm::vec2& anchor)
{
	detail::sprite sprite;
	sprite.x = x;
	sprite.y = y;
	sprite.rotation = 0;
	sprite.scale = glm::vec2(1);
	sprite.anchor = anchor;
	m_sprites->sprites.push_back(sprite);
}

void xd::sprite_batch::add(xd::texture::ptr texture, float x, float y, float rotation, float scale, const glm::vec2& anchor)
{
	detail::sprite sprite;
	sprite.x = x;
	sprite.y = y;
	sprite.rotation = rotation;
	sprite.scale = glm::vec2(scale);
	sprite.anchor = anchor;
	m_sprites->sprites.push_back(sprite);
}

void xd::sprite_batch::add(xd::texture::ptr texture, float x, float y, float rotation, const glm::vec2& scale, const glm::vec2& anchor)
{
	detail::sprite sprite;
	sprite.x = x;
	sprite.y = y;
	sprite.rotation = rotation;
	sprite.scale = scale;
	sprite.anchor = anchor;
	m_sprites->sprites.push_back(sprite);
}
