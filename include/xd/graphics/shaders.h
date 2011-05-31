#ifndef H_XD_GRAPHICS_SHADERS
#define H_XD_GRAPHICS_SHADERS

#include <xd/graphics/shader_program.h>
#include <xd/graphics/texture.h>

namespace xd
{
	class flat_shader : public shader_program
	{
	public:
		flat_shader();
		virtual void setup(const glm::mat4& mvp, const glm::vec4& color);
	};

	class shaded_shader : public shader_program
	{
	public:
		shaded_shader();
		virtual void setup(const glm::mat4& mvp);
	};

	class texture_shader : public shader_program
	{
	public:
		texture_shader();
		virtual void setup(const glm::mat4& mvp, const texture& tex);
	};

	class text_shader : public shader_program
	{
	public:
		text_shader();
	};
}

#endif
