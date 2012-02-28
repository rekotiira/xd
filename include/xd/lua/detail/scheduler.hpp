#ifndef H_XD_LUA_DETAIL_SCHEDULER
#define H_XD_LUA_DETAIL_SCHEDULER

#include <xd/lua/scheduler_task.hpp>
#include <luabind/luabind.hpp>
#include <functional>

namespace xd { namespace lua { namespace detail {

	struct scheduler_task_list;
	struct thread_stack;

	// callback_task is a convenience scheduler task
	// that allows user to bind functions, function objects,
	// lambdas, etc. as tasks to the scheduler
	class callback_task : public scheduler_task
	{
	public:
		callback_task(std::function<bool ()> callback)
			: m_callback(callback)
		{
		}

		bool is_complete()
		{
			return m_callback();
		}

	private:
		std::function<bool ()> m_callback;
	};

	// callback_task_result is a convenience scheduler task
	// that allows user to bind functions, function objects,
	// lambdas, etc. as tasks to the scheduler
	// also supports returning a result back to lua
	// via scheduler_task_result interface
	class callback_task_result : public scheduler_task
	{
	public:
		callback_task_result(std::function<bool (scheduler_task_result&)> callback)
			: m_callback(callback)
		{
		}

		bool is_complete()
		{
			return m_callback(m_result);
		}

	private:
		std::function<bool (scheduler_task_result&)> m_callback;
	};

	// callback_task_lua is a convenience scheduler task
	// that allows user to bind functions, function objects,
	// lambdas, etc. as tasks to the scheduler from lua side
	class callback_task_lua : public scheduler_task
	{
	public:
		callback_task_lua(luabind::object callback)
			: m_callback(callback)
		{
		}

		bool is_complete()
		{
			// we must pass the scheduler_task_result via a boost::reference_wrapper
			// to ensure it is passed by reference, not by value which would make a copy
			return luabind::call_function<bool>(m_callback, boost::ref(m_result));
		}

	private:
		luabind::object m_callback;
	};

} } }

#endif
