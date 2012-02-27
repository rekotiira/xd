#ifndef H_XD_LUA_SCHEDULER
#define H_XD_LUA_SCHEDULER

#include <xd/lua/common.hpp>
#include <xd/lua/types.hpp>
#include <xd/lua/script.hpp>
#include <xd/lua/virtual_machine.hpp>
#include <xd/lua/scheduler_task.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ref.hpp>
#include <lua.hpp>
#include <list>
#include <type_traits>

namespace xd
{
	namespace lua
	{
		namespace detail
		{
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
		}

		// the lua scheduler, supports yielding threads
		// from both C++ and lua's side
		class XD_LUA_API scheduler : public boost::noncopyable
		{
		public:
			scheduler(virtual_machine& vm);
			virtual ~scheduler();
			lua_State *current_thread();
			void start(luabind::object func);
			void run();
			void yield(scheduler_task::ptr task);
			int pending_tasks();

			// a convenience function for starting a xd::lua::function<T>
			// instead of having to cast it to luabind::object yourself
			template <typename T>
			void start(function<T> func)
			{
				start(func.object());
			}

			// yields a thread; copies the passed scheduler_task
			template <typename T>
			typename std::enable_if<std::is_base_of<scheduler_task, T>::value>::type
			yield(const T& task)
			{
				yield(new T(task));
			}

			// we also want to support plain function types as tasks
			void yield(bool (*callback)())
			{
				yield(new detail::callback_task(callback));
			}

			// for function tasks that have a result
			void yield(bool (*callback)(scheduler_task_result&))
			{
				yield(new detail::callback_task_result(callback));
			}

			// for class types, assume we're dealing with a function object
			// since we have to support tasks with and without result,
			// we also have to check for the functor arity, which we do by
			// proxying the call to one of the two following overloads
			template <typename F>
			typename std::enable_if<std::is_class<F>::value && !std::is_base_of<scheduler_task, F>::value>::type
			yield(F f)
			{
				yield(f, &F::operator());
			}

			// an overload for functor callbacks with no result
			// this doesn't have to be called directly; just call yield(functor)
			// and it'll proxy the call to this function
			template <typename F>
			void yield(F f, bool (F::*callback)() const)
			{
				yield(new detail::callback_task(f));
			}

			// an overload for functor callbacks with a result
			// this doesn't have to be called directly; just call yield(functor)
			// and it'll proxy the call to this function
			template <typename F>
			void yield(F f, bool (F::*callback)(scheduler_task_result&) const)
			{
				yield(new detail::callback_task_result(f));
			}

			// generate convenience yield functions that allow to construct task in-place
			// i.e. scheduler.yield<my_task>(param1, param2)
			// maximum of 10 parameters supported
			#define BOOST_PP_ITERATION_PARAMS_1 (3, (0, 10, <xd/lua/detail/iterate_scheduler_yield.hpp>))
			#include BOOST_PP_ITERATE()

			// a convenience function for registering a yielding, optionally stateful,
			// C++ function to lua. By specifying module you can export the function
			// in a given lua table
			template <typename S, typename F>
			void register_function(std::string name, F f, std::string module = "")
			{
				luabind::module(m_vm.lua_state(), module.size() ? module.c_str() : 0)
				[
					luabind::def(name.c_str(), luabind::tag_function<S>(f), luabind::yield)
				];
			}

		private:
			virtual_machine& m_vm;
			lua_State *m_current_thread;
			detail::scheduler_task_list *m_tasks;
			detail::thread_stack *m_thread_stack;
		};
	}
}

#endif
