#ifndef H_XD_GRAPHICS_SPRITE_BATCH
#define H_XD_GRAPHICS_SPRITE_BATCH

#include <xd/graphics/detail/sprite_batch.hpp>

#include <xd/graphics/transform_geometry.hpp>
#include <xd/graphics/texture.hpp>
#include <xd/graphics/types.hpp>
#include <xd/vendor/glm/glm.hpp>
#include <boost/noncopyable.hpp>

#pragma warning(disable: 4275)

namespace xd
{
	// sprite batch
	class XD_API sprite_batch : public xd::ref_counted, public boost::noncopyable
	{
	public:
		typedef boost::intrusive_ptr<sprite_batch> ptr;

		sprite_batch(xd::transform_geometry& geometry_pipeline);
		virtual ~sprite_batch();

		void clear();
		void draw();
		
		void set_scale(float scale);
		float get_scale() const;

		void add(const xd::texture::ptr texture, float x, float y,
			const glm::vec4& color = glm::vec4(1, 1, 1, 1), const glm::vec2& anchor = glm::vec2(0, 0));
		void add(const xd::texture::ptr texture, float x, float y, float rotation, float scale,
			const glm::vec4& color = glm::vec4(1, 1, 1, 1), const glm::vec2& anchor = glm::vec2(0, 0));
		void add(const xd::texture::ptr texture, float x, float y, float rotation, const glm::vec2& scale,
			const glm::vec4& color = glm::vec4(1, 1, 1, 1), const glm::vec2& anchor = glm::vec2(0, 0));

		void add(const xd::texture::ptr texture, const xd::rect& src, float x, float y,
			const glm::vec4& color = glm::vec4(1, 1, 1, 1), const glm::vec2& anchor = glm::vec2(0, 0));
		void add(const xd::texture::ptr texture, const xd::rect& src, float x, float y, float rotation, float scale,
			const glm::vec4& color = glm::vec4(1, 1, 1, 1), const glm::vec2& anchor = glm::vec2(0, 0));
		void add(const xd::texture::ptr texture, const xd::rect& src, float x, float y, float rotation, const glm::vec2& scale,
			const glm::vec4& color = glm::vec4(1, 1, 1, 1), const glm::vec2& anchor = glm::vec2(0, 0));

	private:
		transform_geometry& m_geometry_pipeline;
		detail::sprite_batch_data *m_data;
		float m_scale;
	};
}

#endif
