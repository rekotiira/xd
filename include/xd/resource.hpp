#ifndef H_XD_RESOURCE
#define H_XD_RESOURCE

#include <cstddef>

namespace xd
{
	class resource
	{
	public:
		resource()
			: m_ref_count(0)
		{
		}

		resource(const resource&)
			: m_ref_count(0)
		{
		}
		
		virtual ~resource()
		{
		}

		friend void resource_handle_add_ref(resource*);
		friend bool resource_handle_release(resource*);

	private:
		std::size_t m_ref_count;
	};

	inline void resource_handle_add_ref(resource *p)
	{
		++(p->m_ref_count);
	}

	inline bool resource_handle_release(resource *p)
	{
		return (--(p->m_ref_count) == 0);
	}
};

#endif
