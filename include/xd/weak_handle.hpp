#ifndef H_XD_WEAK_HANDLE
#define H_XD_WEAK_HANDLE

#include <xd/handle.hpp>
#include <memory>
#include <algorithm>

namespace xd
{
	template <typename T>
	class weak_handle
	{
	public:
		typedef T element_type;
		typedef std::weak_ptr<T> ptr_type;

		weak_handle()
		{
		}

		weak_handle(const weak_handle& obj)
			: m_ptr(obj.m_ptr)
		{
		}

		template <typename U>
		weak_handle(const weak_handle<U>& obj)
			: m_ptr(obj.m_ptr)
		{
		}

		template <typename U>
		weak_handle(const handle<U>& obj)
			: m_ptr(obj.m_ptr)
		{
		}

		weak_handle(weak_handle&& obj)
			: m_ptr(std::move(obj.m_ptr))
		{
		}

		template <typename U>
		weak_handle(weak_handle<U>&& obj)
			: m_ptr(std::move(obj.m_ptr))
		{
		}

		weak_handle& operator=(const weak_handle& rhs)
		{
			weak_handle(rhs).swap(*this);
			return *this;
		}

		template <typename U>
		weak_handle& operator=(const weak_handle<U>& rhs)
		{
			weak_handle(rhs).swap(*this);
			return *this;
		}

		template <typename U>
		weak_handle& operator=(const handle<U>& rhs)
		{
			weak_handle(rhs).swap(*this);
			return *this;
		}

		weak_handle& operator=(weak_handle&& rhs)
		{
			weak_handle(std::move(rhs)).swap(*this);
			return *this;
		}

		template <typename U>
		weak_handle& operator=(weak_handle<U>&& rhs)
		{
			weak_handle(std::move(rhs)).swap(*this);
			return *this;
		}

		~weak_handle()
		{
		}

		handle<T> lock() const
		{
			if (auto locked_ptr = m_ptr.lock()) {
				handle<T> result;
				result.m_ptr = locked_ptr;
				return result;
			}
			return handle<T>();
		}

		std::size_t use_count() const
		{
			return m_ptr.use_count();
		}

		bool expired() const
		{
			return m_ptr.expired();
		}

		void reset()
		{
			weak_handle().swap(*this);
		}

		template <typename U>
		bool owner_before(const weak_handle<U>& other) const
		{
			return m_ptr.owner_before(other.m_ptr);
		}

		template <typename U>
		bool owner_before(const handle<U>& other) const
		{
			return m_ptr.owner_before(other.m_ptr);
		}

		operator bool() const
		{
			return (m_ptr != 0);
		}

		void swap(weak_handle& rhs)
		{
			std::swap(m_ptr, rhs.m_ptr);
		}

	private:
		template <typename>
		friend class handle;

		ptr_type m_ptr;
	};

	template <typename T>
	inline void swap(weak_handle<T>& lhs, weak_handle<T>& rhs)
	{
		lhs.swap(rhs);
	}
}

#endif
