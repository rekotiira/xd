#ifndef H_XD_LUA_SCHEDULER_TASK
#define H_XD_LUA_SCHEDULER_TASK

#include <xd/lua/detail/scheduler_task.hpp>
#include <xd/config.hpp>
#include <xd/handle.hpp>
#include <xd/lua/config.hpp>
#include <luabind/luabind.hpp>
#include <memory>

namespace xd
{
	namespace lua
	{
		// a class that represents a task result
		// this can be passed to functions that act
		// as tasks, which allows them to return
		// a result
		class scheduler_task_result
		{
		public:
			virtual ~scheduler_task_result()
			{
			}

			// set a result value, this will reset any old value that is stored
			template <typename T>
			void set_value(const T& val)
			{
				m_value_ptr.reset(new detail::scheduler_task_result_value<T>(val));
			}

			// get and cast the stored value; a bad cast can result in
			// boost::bad_any_cast exception being thrown
			template <typename T>
			T get_value() const
			{
				if (has_value())
					return boost::any_cast<T>(m_value_ptr->any_value());
				else
					return T();
			}

			// get the value as a luabind object; will associate it with the given thread
			luabind::object get_value(lua_State *thread) const
			{
				if (has_value())
					return m_value_ptr->lua_value(thread);
				else
					return luabind::object();
			}

			// return whether the result has a value stored
			bool has_value() const
			{
				return (m_value_ptr.use_count() != 0);
			}

		private:
			// we use a smart pointer to ensure the concrete value is deleted
			std::shared_ptr<detail::scheduler_task_result_placeholder> m_value_ptr;
		};


		// a base class for scheduler tasks
		class scheduler_task
		{
		public:
				typedef handle<scheduler_task> handle;
				typedef weak_handle<scheduler_task> weak_handle;

				virtual ~scheduler_task() {}
				virtual bool is_complete() = 0;

				luabind::object result(lua_State *thread) const
				{
					return m_result.get_value(thread);
				}

				template <typename T>
				void set_result(const T& result)
				{
					m_result.set_value(result);
				}

		protected:
				scheduler_task_result m_result;
		};

	}
}

#endif
