#ifndef H_XD_GRAPHICS_SPRITE_BATCH
#define H_XD_GRAPHICS_SPRITE_BATCH

#include <xd/graphics/detail/sprite_batch.hpp>
#include <xd/handle.hpp>
#include <xd/graphics/transform_geometry.hpp>
#include <xd/graphics/texture.hpp>
#include <xd/graphics/types.hpp>
#include <xd/glm.hpp>
#include <boost/noncopyable.hpp>

#pragma warning(disable: 4275)

namespace xd
{
	// sprite batch
	class XD_API sprite_batch : public boost::noncopyable
	{
	public:
		typedef xd::handle<sprite_batch> handle;
		typedef xd::weak_handle<sprite_batch> weak_handle;

		sprite_batch();
		virtual ~sprite_batch();

		void clear();
		void draw(const mat4& matrix);
		
		void set_scale(float scale);
		float get_scale() const;

		void add(const texture::handle texture, float x, float y,
			const vec4& color = vec4(1, 1, 1, 1), const vec2& origin = vec2(0, 0));
		void add(const texture::handle texture, float x, float y, float rotation, float scale,
			const vec4& color = vec4(1, 1, 1, 1), const vec2& origin = vec2(0, 0));
		void add(const texture::handle texture, float x, float y, float rotation, const vec2& scale,
			const vec4& color = vec4(1, 1, 1, 1), const vec2& origin = vec2(0, 0));

		void add(const texture::handle texture, const rect& src, float x, float y,
			const vec4& color = vec4(1, 1, 1, 1), const vec2& origin = vec2(0, 0));
		void add(const texture::handle texture, const rect& src, float x, float y, float rotation, float scale,
			const vec4& color = vec4(1, 1, 1, 1), const vec2& origin = vec2(0, 0));
		void add(const texture::handle texture, const rect& src, float x, float y, float rotation, const vec2& scale,
			const vec4& color = vec4(1, 1, 1, 1), const vec2& origin = vec2(0, 0));

	private:
		detail::sprite_batch_data *m_data;
		float m_scale;
	};
}

#endif
