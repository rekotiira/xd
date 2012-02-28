#include <xd/graphics/sprite_batch.hpp>
#include <xd/graphics/texture.hpp>
#include <xd/graphics/vertex_batch.hpp>
#include <xd/graphics/shader_program.hpp>
#include <xd/graphics/utility.hpp>
#include <boost/optional.hpp>
#include <deque>

namespace xd { namespace detail {

	struct sprite
	{
		texture::ptr texture;
		xd::rect src;
		float x, y;
		float rotation;
		glm::vec2 scale;
		glm::vec2 anchor;
		glm::vec4 color;
		float depth;
	};

	struct sprite_batch_data
	{
		std::deque<sprite> sprites;
		xd::sprite_shader shader;
	};

} }

xd::sprite_batch::sprite_batch(xd::transform_geometry& geometry_pipeline)
	: m_geometry_pipeline(geometry_pipeline)
	, m_data(new detail::sprite_batch_data)
	, m_scale(1)
{
}

xd::sprite_batch::~sprite_batch()
{
	delete m_data;
}

void xd::sprite_batch::clear()
{
	m_data->sprites.clear();
}

void xd::sprite_batch::draw()
{
	// create a vertex batch for sending vertex data
	vertex_batch<detail::sprite_vertex_traits> batch(GL_QUADS);

	// setup the shader
	m_data->shader.use();

	// get the modelview matrix for convenience
	auto& model_view = m_geometry_pipeline.model_view();

	// iterate through all sprites
	for (auto i = m_data->sprites.begin(); i != m_data->sprites.end(); ++i) {
		// push matrix
		model_view.push();
		// create a quad for rendering sprites
		detail::sprite_vertex quad[4];
		// assign position
		auto tw = i->texture->width();
		auto th = i->texture->height();
		auto& src = i->src;
		auto& anchor = i->anchor;
		quad[0].pos = glm::vec2((0-anchor.x)*src.w, (1-anchor.y)*src.h);
		quad[1].pos = glm::vec2((1-anchor.x)*src.w, (1-anchor.y)*src.h);
		quad[2].pos = glm::vec2((1-anchor.x)*src.w, (0-anchor.y)*src.h);
		quad[3].pos = glm::vec2((0-anchor.x)*src.w, (0-anchor.y)*src.h);
		// assign texture pos
		quad[0].texpos = glm::vec2(src.x/tw        , (src.y+src.h)/th);
		quad[1].texpos = glm::vec2((src.x+src.w)/tw, (src.y+src.h)/th);
		quad[2].texpos = glm::vec2((src.x+src.w)/tw, src.y/th);
		quad[3].texpos = glm::vec2(src.x/tw        , src.y/th);
		// load the quad
		batch.load(&quad[0], 4);
		// translate to the correct position
		model_view.translate(i->x, i->y, i->depth);
		// setup rotation and scaling
		model_view.rotate(i->rotation, 0, 0, 1);
		model_view.scale(m_scale*i->scale.x, m_scale*i->scale.y, 1);
		// give required params to shader
		m_data->shader.bind_uniform("mvpMatrix", m_geometry_pipeline.mvp());
		m_data->shader.bind_uniform("vColor", i->color);
		// bind the texture
		i->texture->bind(GL_TEXTURE0);
		// draw it
		batch.render();
		// pop matrix
		model_view.pop();
	}
}

void xd::sprite_batch::set_scale(float scale)
{
	m_scale = scale;
}

float xd::sprite_batch::get_scale() const
{
	return m_scale;
}

void xd::sprite_batch::add(const xd::texture::ptr texture, float x, float y, const glm::vec4& color, const glm::vec2& anchor)
{
	add(texture, xd::rect(0, 0, texture->width(), texture->height()), x, y, 0, glm::vec2(1, 1), color, anchor);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, float x, float y, float rotation, float scale, const glm::vec4& color, const glm::vec2& anchor)
{
	add(texture, xd::rect(0, 0, texture->width(), texture->height()), x, y, rotation, glm::vec2(scale, scale), color, anchor);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, float x, float y, float rotation, const glm::vec2& scale, const glm::vec4& color, const glm::vec2& anchor)
{
	add(texture, xd::rect(0, 0, texture->width(), texture->height()), x, y, rotation, scale, color, anchor);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, const xd::rect& src, float x, float y, const glm::vec4& color, const glm::vec2& anchor)
{
	add(texture, src, x, y, 0, glm::vec2(1, 1), color, anchor);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, const xd::rect& src, float x, float y, float rotation, float scale, const glm::vec4& color, const glm::vec2& anchor)
{
	add(texture, src, x, y, rotation, glm::vec2(scale, scale), color, anchor);
}

void xd::sprite_batch::add(const xd::texture::ptr texture, const xd::rect& src, float x, float y, float rotation, const glm::vec2& scale, const glm::vec4& color, const glm::vec2& anchor)
{
	detail::sprite sprite;
	sprite.texture = texture;
	sprite.src = src;
	sprite.x = x;
	sprite.y = y;
	sprite.rotation = rotation;
	sprite.scale = scale;
	sprite.anchor = anchor;
	sprite.color = color;
	sprite.depth = 0.0f;
	m_data->sprites.push_back(sprite);
}
