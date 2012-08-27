#include <xd/lua/scheduler.hpp>
#include <xd/lua/scheduler_task.hpp>
#include <xd/factory.hpp>
#include <boost/optional.hpp>
#include <stack>

namespace xd { namespace lua { namespace detail {

	struct scheduler_thread_task
	{
		lua_State *thread;
		scheduler_task::ptr task;
	};

	struct scheduler_task_list
	{
		std::list<scheduler_thread_task> tasks;
	};

	struct thread_stack
	{
		std::stack<lua_State*> threads;
	};

	// this light wrapper function allows us to pass the current thread
	// to scheduler_task_result::get_value()
	void scheduler_task_result_get_value(lua_State *thread, scheduler_task_result& result)
	{
		luabind::object val = result.get_value(thread);
		val.push(thread);
	}

} } }

xd::lua::scheduler::scheduler(virtual_machine& vm)
	: m_vm(vm)
	, m_current_thread(0)
{
	// register scheduler_task_result
	luabind::module(vm.lua_state())
	[
		// it's just an internal class; don't give it a name
		luabind::class_<scheduler_task_result>("")
			.def("set_value", &scheduler_task_result::set_value<luabind::object>)
			.def("get_value", &detail::scheduler_task_result_get_value)
			.def("has_value", &scheduler_task_result::has_value)
	];

	m_tasks = new detail::scheduler_task_list;
	m_thread_stack = new detail::thread_stack;
}

xd::lua::scheduler::~scheduler()
{
	delete m_tasks;
	delete m_thread_stack;
}

lua_State *xd::lua::scheduler::current_thread()
{
	return m_current_thread;
}

void xd::lua::scheduler::start(luabind::object func)
{
	// create a new thread
	lua_State *thread = lua_newthread(m_vm.lua_state());
	// push the function in the new thread, because the function
	// the thread calls must be called in its local environment
	func.push(thread);
	// construct a callable object from it, the value is copied from stack
	luabind::object thread_func(luabind::from_stack(thread, -1));
	// remove the original value from the stack
	lua_pop(thread, 1);
	// set the current thread
	m_thread_stack->threads.push(thread);
	m_current_thread = thread;
	// start the thread
	luabind::object result = luabind::resume_function<luabind::object>(thread_func);
	// if the thread was yielded from lua side, and the return value is a callable function
	int type = luabind::type(result);
	if (type == LUA_TFUNCTION || type == LUA_TTABLE || type == LUA_TUSERDATA) {
		yield(xd::create<detail::callback_task_lua>(result));
	}
	// reset current thread
	m_thread_stack->threads.pop();
	if (m_thread_stack->threads.empty())
		m_current_thread = nullptr;
	else
		m_current_thread = m_thread_stack->threads.top();
}

void xd::lua::scheduler::run()
{
	// iterate through all pending tasks
	for (auto i = m_tasks->tasks.begin(); i != m_tasks->tasks.end();) {
		// if the thread is dead, remove it
		if (lua_status(i->thread) != LUA_YIELD) {
			i = m_tasks->tasks.erase(i);
			continue;
		}

		// is this task complete?
		if (i->task->is_complete()) {
			// get the thread task
			detail::scheduler_thread_task thread_task = *i;
			// remove it from the pending list
			i = m_tasks->tasks.erase(i);
			// set the current thread
			m_thread_stack->threads.push(thread_task.thread);
			m_current_thread = thread_task.thread;
			// resume the thread, passing it (optional) result of the finished task
			luabind::object result;
			if (thread_task.task)
				result = luabind::resume<luabind::object>(thread_task.thread, thread_task.task->result(thread_task.thread));
			else
				result = luabind::resume<luabind::object>(thread_task.thread);
			// if the thread was yielded from lua side, and the return value is a callable function
			int type = luabind::type(result);
			if (type == LUA_TFUNCTION || type == LUA_TTABLE || type == LUA_TUSERDATA) {
				yield(xd::create<detail::callback_task_lua>(result));
			}
			// reset current thread
			m_thread_stack->threads.pop();
			if (m_thread_stack->threads.empty())
				m_current_thread = nullptr;
			else
				m_current_thread = m_thread_stack->threads.top();
		} else {
			++i;
		}
	}
}

void xd::lua::scheduler::yield(xd::lua::scheduler_task::ptr task)
{
	detail::scheduler_thread_task thread_task;
	thread_task.thread = m_current_thread;
	thread_task.task = task;
	m_tasks->tasks.push_back(thread_task);
	// pusnnil is used to ensure that a call to a C++ function
	// that yields will return a NIL object, otherwise the return
	// value would be the first parameter passed to the function
	// which could confuse the scheduler as it uses the return type
	// to determine whether it was lua or C++ function that yielded
	lua_pushnil(m_current_thread);
}

int xd::lua::scheduler::pending_tasks()
{
	return m_tasks->tasks.size();
}
