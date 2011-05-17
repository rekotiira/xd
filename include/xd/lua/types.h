#ifndef H_XD_LUA_TYPES
#define H_XD_LUA_TYPES

#include <lua.h>
#include <luabind/luabind.hpp>
#include <stdexcept>
#include <boost/optional.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_void.hpp>

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

			function(const luabind::object& obj, const T& default)
				: m_func(obj)
				, m_default(default)
			{
			}

			T operator()()
			{
				luabind::object ret = m_func();
				if (m_default && luabind::type(ret) == LUA_TNIL)
					return *m_default;
				return luabind::object_cast<T>(ret);
			}

			template <typename P1>
			T operator()(const P1& p1)
			{
				luabind::object ret = m_func(p1);
				if (m_default && luabind::type(ret) == LUA_TNIL)
					return *m_default;
				return luabind::object_cast<T>(ret);
			}

			template <typename P1, typename P2>
			T operator()(const P1& p1, const P2& p2)
			{
				luabind::object ret = m_func(p1, p2);
				if (m_default && luabind::type(ret) == LUA_TNIL)
					return *m_default;
				return luabind::object_cast<T>(ret);
			}

			template <typename P1, typename P2, typename P3>
			T operator()(const P1& p1, const P2& p2, const P3& p3)
			{
				luabind::object ret = m_func(p1, p2, p3);
				if (m_default && luabind::type(ret) == LUA_TNIL)
					return *m_default;
				return luabind::object_cast<T>(ret);
			}

			template <typename P1, typename P2, typename P3, typename P4>
			T operator()(const P1& p1, const P2& p2, const P3& p3, const P4& p4)
			{
				luabind::object ret = m_func(p1, p2, p3, p4);
				if (m_default && luabind::type(ret) == LUA_TNIL)
					return *m_default;
				return luabind::object_cast<T>(ret);
			}

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

			void operator()()
			{
				m_func();
			}

			template <typename P1>
			void operator()(const P1& p1)
			{
				m_func(p1);
			}

			template <typename P1, typename P2>
			void operator()(const P1& p1, const P2& p2)
			{
				m_func(p1, p2);
			}

			template <typename P1, typename P2, typename P3>
			void operator()(const P1& p1, const P2& p2, const P3& p3)
			{
				m_func(p1, p2, p3);
			}

			template <typename P1, typename P2, typename P3, typename P4>
			void operator()(const P1& p1, const P2& p2, const P3& p3, const P4& p4)
			{
				m_func(p1, p2, p3, p4);
			}
		private:
			luabind::object m_func;
		};

		template <typename Class, typename Type, boost::optional<Type> Class::*Ptr>
		struct optional_property
		{
			static luabind::object getter(lua_State *vm, const Class& obj)
			{
				if (!(obj.*Ptr))
					return luabind::object();
				else
					return luabind::object(vm, *(obj.*Ptr));
			}

			static void setter(Class& obj, const luabind::object& val)
			{
				if (luabind::type(val) == LUA_TNIL)
					obj.*Ptr = xd::any;
				else
					obj.*Ptr = luabind::object_cast<Type>(val);
			}
		};
	}
}

#endif
