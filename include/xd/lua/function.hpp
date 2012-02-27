#ifndef H_XD_LUA_FUNCTION
#define H_XD_LUA_FUNCTION

#include <xd/config.hpp>
#include <xd/lua/common.hpp>
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <boost/optional.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>

namespace xd
{
	namespace lua
	{
		template <typename T>
		struct function
		{
			function(const luabind::object& obj)
				: m_func(obj)
			{
			}

			function(const luabind::object& obj, const T& default_)
				: m_func(obj)
				, m_default(default_)
			{
			}

			luabind::object object()
			{
				return m_func;
			}

			// generate overloads for operator()
			// maximum of XD_MAX_ARITY parameters supported
			#define BOOST_PP_ITERATION_PARAMS_1 (4, (0, XD_MAX_ARITY, <xd/lua/detail/iterate_function_call.hpp>, 0))
			#include BOOST_PP_ITERATE()

		private:
			luabind::object m_func;
			boost::optional<T> m_default;
		};

		template <>
		struct function<void>
		{
			function(const luabind::object& obj)
				: m_func(obj)
			{
			}

			luabind::object object()
			{
				return m_func;
			}

			// generate overloads for operator()
			// maximum of XD_MAX_ARITY parameters supported
			#define BOOST_PP_ITERATION_PARAMS_1 (4, (0, XD_MAX_ARITY, <xd/lua/detail/iterate_function_call.hpp>, 1))
			#include BOOST_PP_ITERATE()
		private:
			luabind::object m_func;
		};
	}
}

#endif
