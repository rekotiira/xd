#include <xd/lua/types.hpp>
#include <xd/lua/script.hpp>
#include <xd/lua/vm.hpp>
#include <luabind/luabind.hpp>
/*
xd::lua::script::script(vm& vm, int ref)
	: m_vm(vm)
	, m_script_ref(ref)
{
}

xd::lua::script::~script()
{
	luaL_unref(m_vm.lua_state(), LUA_REGISTRYINDEX, m_script_ref);
}

void xd::lua::script::run()
{
	// push the function at the top of the stack
	lua_rawgeti(m_vm.lua_state(), LUA_REGISTRYINDEX, m_script_ref);

	// get the function from registry
	luabind::object func(m_vm.registry(m_script_ref));

	// call it
	luabind::call_function<void>(func);
}
*/