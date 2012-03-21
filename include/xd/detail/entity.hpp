#ifndef H_XD_DETAIL_ENTITY
#define H_XD_DETAIL_ENTITY

#include <xd/detail/identity.hpp>
#include <xd/resource.hpp>
#include <xd/resource_handle.hpp>

namespace xd { namespace detail {

	template <typename T>
	class component_base : public xd::resource
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
		typedef resource_handle<detail::logic_component<T>> handle;
	private:
		friend T;
		virtual void update(T&) = 0;
	};
        
	template <typename T>
	class render_component : public virtual component_base<T>
	{
	public:
		typedef resource_handle<detail::render_component<T>> handle;
	private:
		friend T;
		virtual void render(T&) = 0;
	};
        
	template <typename T>
	class component : public logic_component<T>, public render_component<T>
	{
	public:
		typedef resource_handle<detail::component<T>> handle;
	};

} }

#endif
