#ifndef H_XD_DETAIL_ENTITY
#define H_XD_DETAIL_ENTITY

#include <boost/intrusive_ptr.hpp>

namespace xd { namespace detail {

	template <typename T>
	class component_base : public xd::ref_counted
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
		typedef boost::intrusive_ptr<detail::logic_component<T>> ptr;
	private:
		friend T;
		virtual void update(T&) = 0;
	};
        
	template <typename T>
	class render_component : public virtual component_base<T>
	{
	public:
		typedef boost::intrusive_ptr<detail::render_component<T>> ptr;
	private:
		friend T;
		virtual void render(T&) = 0;
	};
        
	template <typename T>
	class component : public logic_component<T>, public render_component<T>
	{
	public:
		typedef boost::intrusive_ptr<detail::component<T>> ptr;
	};

	// used to disable template argument deduction
	template <typename T>
	struct identity
	{
		typedef T type;
	};

} }

#endif
