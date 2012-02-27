#ifndef H_XD_REF_COUNTED
#define H_XD_REF_COUNTED

#include <xd/config.hpp>
#include <boost/intrusive_ptr.hpp>

namespace xd
{
	class ref_counted
	{
	public:
		ref_counted()
			: m_ref_count(0)
		{
		}

		ref_counted(const ref_counted&)
			: m_ref_count(0)
		{
		}
		
		virtual ~ref_counted()
		{
		}

		friend void intrusive_ptr_add_ref(ref_counted*);
		friend void intrusive_ptr_release(ref_counted*);

	private:
		std::size_t m_ref_count;
	};

	inline void intrusive_ptr_add_ref(ref_counted *p)
	{
		++(p->m_ref_count);
	}

	inline void intrusive_ptr_release(ref_counted *p)
	{
		if (--(p->m_ref_count) == 0)
			delete p;
	}
}


#endif
