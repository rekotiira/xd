#ifndef H_XD_LUA_DETAIL_SCHEDULER_TASK
#define H_XD_LUA_DETAIL_SCHEDULER_TASK

#include <boost/any.hpp>
#include <lua.hpp>
#include <luabind/luabind.hpp>

namespace xd { namespace lua { namespace detail {

	// a common placeholder base for a scheduler task result value
	// this enables us to hide the result type from the result class
	// via type-erasure, which allows us to defer the creation of
	// luabind::object via virtual dispatch.
	// we do this because luabind::object requires the lua_State*
	// thread to construct the result in correct thread's environment
	// but we don't want to bind the task to a specific thread
	// the luabind::object is only created when scheduler calls
	// get_value() on the scheduler_task_result, passing it the
	// current thread
	struct scheduler_task_result_placeholder
	{
		virtual ~scheduler_task_result_placeholder() {}
		virtual boost::any any_value() const = 0;
		virtual luabind::object lua_value(lua_State *thread) const = 0;
	};

	// a concrete result value, calls to any_value() and lua_value()
	// will be proxied to instantations of this class via virtual dispatch
	template <typename T>
	struct scheduler_task_result_value : scheduler_task_result_placeholder
	{
		scheduler_task_result_value(const T& val)
			: val(val)
		{
		}

		virtual boost::any any_value() const
		{
			return boost::any(val);
		}

		virtual luabind::object lua_value(lua_State *thread) const
		{
			return luabind::object(thread, val);
		}

		T val;
	};

	template <>
	struct scheduler_task_result_value<luabind::object> : scheduler_task_result_placeholder
	{
		scheduler_task_result_value(const luabind::object& val)
			: val(val)
		{
		}

		virtual boost::any any_value() const
		{
			return boost::any(val);
		}

		virtual luabind::object lua_value(lua_State *thread) const
		{
			// if the value is owned by the calling thread, return it
			if (val.interpreter() == thread)
				return val;
			// otherwise copy it to the calling thread
			val.push(thread);
			luabind::object obj(luabind::from_stack(thread, -1));
			lua_pop(thread, 1);
			return val;
		}

		luabind::object val;
	};

} } }

#endif
