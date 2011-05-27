#include <xd/lua/script.h>
#include <xd/lua/vm.h>

xd::lua::script::script(lua_State *vm, int ref)
	: m_vm(vm)
	, m_script_ref(ref)
{
}

xd::lua::script::~script()
{
	luaL_unref(m_vm, LUA_REGISTRYINDEX, m_script_ref);
}

void xd::lua::script::run()
{
	// push the function at the top of the stack
	lua_rawgeti(m_vm, LUA_REGISTRYINDEX, m_script_ref);

	// call it
	lua_call(m_vm, 0, 0);
}