#ifndef H_XD_GRAPHICS_SHADERS
#define H_XD_GRAPHICS_SHADERS

#include <xd/graphics/shader_program.hpp>
#include <xd/graphics/texture.hpp>

namespace xd
{
	class XD_API flat_shader : public shader_program
	{
	public:
		flat_shader();
		virtual void setup(const glm::mat4& mvp, const glm::vec4& color);
	};

	class XD_API shaded_shader : public shader_program
	{
	public:
		shaded_shader();
		virtual void setup(const glm::mat4& mvp);
	};

	class XD_API texture_shader : public shader_program
	{
	public:
		texture_shader();
		virtual void setup(const glm::mat4& mvp, const texture::ptr tex);
	};

	class XD_API texture_mask_shader : public shader_program
	{
	public:
		texture_mask_shader();
		virtual void setup(const glm::mat4& mvp, const texture::ptr tex, const texture::ptr mask);
	};

	class XD_API text_shader : public shader_program
	{
	public:
		text_shader();
	};

	class XD_API sprite_shader : public shader_program
	{
	public:
		sprite_shader();
	};
}

#endif
