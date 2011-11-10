#ifndef H_XD_GRAPHICS_TRANSFORM_GEOMETRY
#define H_XD_GRAPHICS_TRANSFORM_GEOMETRY

#include <xd/graphics/matrix_stack.hpp>

namespace xd
{
	class transform_geometry
	{
	public:
		transform_geometry()
		{
		}
		
		~transform_geometry()
		{
		}

		matrix_stack& projection()
		{
			return m_projection;
		}

		const matrix_stack& projection() const
		{
			return m_projection;
		}

		matrix_stack& model_view()
		{
			return m_model_view;
		}
		
		const matrix_stack& model_view() const
		{
			return m_model_view;
		}

		glm::mat4 mvp() const
		{
			return m_projection.get() * m_model_view.get();
		}

	private:
		matrix_stack m_projection;
		matrix_stack m_model_view;
	};
}

#endif
