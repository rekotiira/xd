#ifndef H_XD_RESOURCE_HANDLE
#define H_XD_RESOURCE_HANDLE

#include <xd/detail/resource_handle.hpp>
#include <functional>
#include <algorithm>

namespace xd
{
	template <typename T>
	class resource_handle
	{
	public:
		resource_handle()
			: m_ptr(nullptr)
			, m_deleter(detail::resource_handle_default_deleter<T>())
		{
		}

		resource_handle(T* obj)
			: m_ptr(obj)
			, m_deleter(detail::resource_handle_default_deleter<T>())
		{
			add_ref();
		}

		template <typename Deleter>
		resource_handle(T* obj, Deleter deleter)
			: m_ptr(obj)
			, m_deleter(deleter)
		{
			add_ref();
		}

		resource_handle(T& obj)
			: m_ptr(&obj)
			, m_deleter(detail::resource_handle_noop_deleter<T>())
		{
			add_ref();
		}

		resource_handle(T&& obj)
			: m_ptr(&obj)
			, m_deleter(detail::resource_handle_noop_deleter<T>())
		{
			add_ref();
		}

		// we need to overload the non-template version to prevent
		// compiler from defining its own copy-ctor
		resource_handle(const resource_handle& rhs)
			: m_ptr(rhs.m_ptr)
			, m_deleter(rhs.m_deleter)
		{
			add_ref();
		}

		// we need to specify this overload because otherwise
		// you couldn't call resource_handle<foo><resource_handle<bar>()>
		// where bar derives from foo, because resource_handle<foo>
		// and resource_handle<bar> are still unrelated types, although
		// foo and bar aren't
		template <typename U>
		resource_handle(const resource_handle<U>& rhs)
			: m_ptr(rhs.m_ptr)
			, m_deleter(rhs.m_deleter)
		{
			add_ref();
		}

		// we need to overload the non-template version to prevent
		// compiler from defining its own copy-ctor
		resource_handle(resource_handle&& rhs)
			: m_ptr(rhs.m_ptr)
			, m_deleter(std::move(rhs.m_deleter))
		{
			// assigning the rhs's pointer to nullptr
			// the ref count of the object won't get
			// decremented, hence we don't have to increment
			// nor decrement
			rhs.m_ptr = nullptr;
		}

		// we need to specify this overload because otherwise
		// you couldn't call resource_handle<foo><resource_handle<bar>()>
		// where bar derives from foo, because resource_handle<foo>
		// and resource_handle<bar> are still unrelated types, although
		// foo and bar aren't
		template <typename U>
		resource_handle(resource_handle<U>&& rhs)
			: m_ptr(rhs.m_ptr)
			, m_deleter(std::move(rhs.m_deleter))
		{
			rhs.m_ptr = nullptr;
		}

		resource_handle& operator=(const resource_handle& rhs)
		{
			resource_handle(rhs).swap(*this);
			return *this;
		}

		// we need to specify this overload because otherwise
		// you couldn't call resource_handle<foo><resource_handle<bar>()>
		// where bar derives from foo, because resource_handle<foo>
		// and resource_handle<bar> are still unrelated types, although
		// foo and bar aren't
		template <typename U>
		resource_handle& operator=(const resource_handle<U>& rhs)
		{
			resource_handle(rhs).swap(*this);
			return *this;
		}

		resource_handle& operator=(resource_handle&& rhs)
		{
			resource_handle(std::move(rhs)).swap(*this);
			return *this;
		}

		// we need to specify this overload because otherwise
		// you couldn't call resource_handle<foo><resource_handle<bar>()>
		// where bar derives from foo, because resource_handle<foo>
		// and resource_handle<bar> are still unrelated types, although
		// foo and bar aren't
		template <typename U>
		resource_handle& operator=(resource_handle<U>&& rhs)
		{
			resource_handle(std::move(rhs)).swap(*this);
			return *this;
		}

		~resource_handle()
		{
			release();
		}

		void reset()
		{
			resource_handle().swap(*this);
		}

		void reset(T* rhs)
		{
			resource_handle(rhs).swap(*this);
		}

		template <typename Deleter>
		void reset(T* rhs, Deleter deleter)
		{
			resource_handle(rhs, deleter).swap(*this);
		}

		T* get() const
		{
			return m_ptr;
		}

		T& operator*() const
		{
			return *m_ptr;
		}

		T* operator->() const
		{
			return m_ptr;
		}

		operator bool() const
		{
			return (m_ptr != 0);
		}

		void add_ref()
		{
			// resource_handle_add_ref(T*)
			// should be overloaded in the namespace where
			// T is defined in. ADL will correctly look-up
			// the correct overload
			if (m_ptr)
				resource_handle_add_ref(m_ptr);
		}

		void release()
		{
			// resource_handle_release(T*)
			// should be overloaded in the namespace where
			// T is defined in. ADL will correctly look-up
			// the correct overload
			if (m_ptr && resource_handle_release(m_ptr))
				m_deleter(m_ptr);
		}

		void swap(resource_handle& rhs)
		{
			std::swap(m_deleter, rhs.m_deleter);
			std::swap(m_ptr, rhs.m_ptr);
		}

	private:
		T* m_ptr;
		std::function<void (T*&)> m_deleter;
	};

	// std::swap will find this function when used with xd::resource_handle
	// using an argument dependent lookup (ADL); it's a nicer way than defining
	// the overload in std namespace, and in addition it'll also work for non-standard
	// swap algorithms, such as boost::swap
	template <typename T>
	inline void swap(resource_handle<T>& lhs, resource_handle<T>& rhs)
	{
		lhs.swap(rhs);
	}
}

#endif
