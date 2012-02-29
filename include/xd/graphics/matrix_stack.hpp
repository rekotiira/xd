#ifndef H_XD_GRAPHICS_MATRIX_STACK
#define H_XD_GRAPHICS_MATRIX_STACK

#include <stdexcept>
#include <stack>
#include <xd/glm.hpp>
#include <xd/vendor/glm/gtc/matrix_transform.hpp>

namespace xd
{
	class matrix_stack
	{
	public:
		matrix_stack()
		{
			m_stack.push(glm::mat4());
		}

		~matrix_stack()
		{
		}
		
		const glm::mat4& get() const
		{
			return m_stack.top();
		}

		void push()
		{
			m_stack.push(m_stack.top());
		}
		
		void push(const glm::mat4& mat)
		{
			m_stack.push(mat);
		}
		
		void pop()
		{
			if (m_stack.size() == 1)
				throw std::underflow_error("matrix stack underflow");
			m_stack.pop();
		}

		void identity()
		{
			m_stack.top() = glm::mat4();
		}
		
		void load(const glm::mat4& mat)
		{
			m_stack.top() = mat;
		}
		
		void multiply(const glm::mat4& mat)
		{
			m_stack.top() *= mat;
		}

		void translate(const glm::vec3& vec)
		{
			m_stack.top() = glm::translate(m_stack.top(), vec);
		}
		
		void translate(float x, float y, float z)
		{
			translate(glm::vec3(x, y, z));
		}
		
		void scale(const glm::vec3& vec)
		{
			m_stack.top() = glm::scale(m_stack.top(), vec);
		}
		
		void scale(float x, float y, float z)
		{
			scale(glm::vec3(x, y, z));
		}
		
		void scale(float s)
		{
			scale(glm::vec3(s, s, s));
		}

		void rotate(float a, const glm::vec3& vec)
		{
			m_stack.top() = glm::rotate(m_stack.top(), a, vec);
		}
		
		void rotate(float a, float x, float y, float z)
		{
			rotate(a, glm::vec3(x, y, z));
		}
	
	private:
		std::stack<glm::mat4> m_stack;
	};
}

#endif
