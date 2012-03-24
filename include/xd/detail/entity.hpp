#ifndef H_XD_DETAIL_ENTITY
#define H_XD_DETAIL_ENTITY

#include <xd/detail/identity.hpp>
#include <xd/handle.hpp>

namespace xd { namespace detail {

	template <typename T>
	class component_base
	{
	public:
		virtual ~component_base() {}
	private:
		friend T;
		virtual void init(T&) {}
	};

	template <typename T>
	class logic_component : public virtual component_base<T>
	{
	public:
		typedef handle<detail::logic_component<T>> handle;
		typedef weak_handle<detail::logic_component<T>> weak_handle;
	private:
		friend T;
		virtual void update(T&) = 0;
	};
        
	template <typename T>
	class render_component : public virtual component_base<T>
	{
	public:
		typedef handle<detail::render_component<T>> handle;
		typedef weak_handle<detail::render_component<T>> weak_handle;
	private:
		friend T;
		virtual void render(T&) = 0;
	};
        
	template <typename T>
	class component : public logic_component<T>, public render_component<T>
	{
	public:
		typedef xd::handle<detail::component<T>> handle;
		typedef xd::weak_handle<detail::component<T>> weak_handle;
	};

} }

#endif
