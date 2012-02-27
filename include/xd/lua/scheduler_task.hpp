#ifndef H_XD_LUA_SCHEDULER_TASK
#define H_XD_LUA_SCHEDULER_TASK

#include <xd/config.hpp>
#include <xd/ref_counted.hpp>
#include <xd/lua/config.hpp>
#include <xd/lua/scheduler_task.hpp>
#include <boost/any.hpp>
#include <memory>

namespace xd
{
	namespace lua
	{
		namespace detail
		{
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
		}

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
		class scheduler_task : public xd::ref_counted
		{
		public:
				typedef boost::intrusive_ptr<scheduler_task> ptr;

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
