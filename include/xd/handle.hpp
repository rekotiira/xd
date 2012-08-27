/*#ifndef H_XD_HANDLE
#define H_XD_HANDLE

#include <xd/detail/noop_deleter.hpp>
#include <memory>
#include <algorithm>

namespace xd
{
	template <typename T> class weak_handle;

	template <typename T>
	class handle
	{
	public:
		typedef T element_type;
		typedef std::shared_ptr<T> ptr_type;

		handle()
		{
		}

		handle(T& obj)
			: m_ptr(&obj, detail::noop_deleter<T>())
		{
		}

		handle(T&& obj)
			: m_ptr(&obj, detail::noop_deleter<T>())
		{
		}

		explicit handle(T* obj)
			: m_ptr(obj)
		{
		}

		template <typename Deleter>
		handle(T* obj, Deleter deleter)
			: m_ptr(obj, deleter)
		{
		}

		template <typename Deleter, typename Alloc>
		handle(T* obj, Deleter deleter, Alloc alloc)
			: m_ptr(obj, deleter, alloc)
		{
		}

		handle(const handle& obj)
			: m_ptr(obj.m_ptr)
		{
		}

		template <typename U>
		handle(const handle<U>& obj)
			: m_ptr(obj.m_ptr)
		{
		}

		handle(handle&& obj)
			: m_ptr(std::move(obj.m_ptr))
		{
		}

		template <typename U>
		handle(handle<U>&& obj)
			: m_ptr(std::move(obj.m_ptr))
		{
		}

		template <typename U>
		explicit handle(const weak_handle<U>& obj)
			: m_ptr(obj.m_ptr)
		{
		}

		handle& operator=(const handle& rhs)
		{
			handle(rhs).swap(*this);
			return *this;
		}

		template <typename U>
		handle& operator=(const handle<U>& rhs)
		{
			handle(rhs).swap(*this);
			return *this;
		}

		handle& operator=(handle&& rhs)
		{
			handle(std::move(rhs)).swap(*this);
			return *this;
		}

		template <typename U>
		handle& operator=(handle<U>&& rhs)
		{
			handle(std::move(rhs)).swap(*this);
			return *this;
		}

		~handle()
		{
		}

		void reset()
		{
			handle().swap(*this);
		}

		void reset(T* obj)
		{
			handle(obj).swap(*this);
		}

		template <typename Deleter>
		void reset(T* obj, Deleter deleter)
		{
			handle(obj, deleter).swap(*this);
		}

		template <typename Deleter, typename Alloc>
		void reset(T* obj, Deleter deleter, Alloc alloc)
		{
			handle(obj, deleter, alloc).swap(*this);
		}

		T* get() const
		{
			return m_ptr.get();
		}

		T& operator*() const
		{
			return *(m_ptr.get());
		}

		T* operator->() const
		{
			return m_ptr.get();
		}

		std::size_t use_count() const
		{
			return m_ptr.use_count();
		}

		bool unique() const
		{
			return m_ptr.unique();
		}

		template <typename U>
		bool owner_before(const handle<U>& other) const
		{
			return m_ptr.owner_before(other.m_ptr);
		}

		template <typename U>
		bool owner_before(const weak_handle<U>& other) const
		{
			return m_ptr.owner_before(other.m_ptr);
		}

		operator bool() const
		{
			return (m_ptr.get() != 0);
		}

		void swap(handle& rhs)
		{
			std::swap(m_ptr, rhs.m_ptr);
		}

	private:
		template <typename>
		friend class weak_handle;

		ptr_type m_ptr;
	};

	template <typename T>
	inline void swap(handle<T>& lhs, handle<T>& rhs)
	{
		lhs.swap(rhs);
	}
}

#endif
*/