#ifndef H_XD_TRANSFORM_GEOMETRY
#define H_XD_TRANSFORM_GEOMETRY

#include "matrix_stack.h"

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
		
		matrix_stack& model_view()
		{
			return m_model_view;
		}

		glm::mat4 mvp()
		{
			return m_projection.get() * m_model_view.get();
		}

	private:
		matrix_stack m_projection;
		matrix_stack m_model_view;
	};
}

#endif
