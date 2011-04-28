#ifndef H_XD_SHADERS
#define H_XD_SHADERS

#include <xd/graphics/shader_program.h>

namespace xd
{
	class flat_shader : public shader_program
	{
	public:
		flat_shader();
		void use(const glm::mat4& mvp, const glm::vec4& color);

	};

	class shaded_shader : public shader_program
	{
	public:
		shaded_shader();
		void use(const glm::mat4& mvp);
	};
}

#endif
