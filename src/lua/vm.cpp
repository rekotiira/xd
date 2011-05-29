#include <xd/lua.h>
#include <fstream>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <xd/system.h>
#include <xd/common.h>
#include <xd/graphics.h>
#include <luabind/operator.hpp>
#include <boost/optional.hpp>

namespace xd { namespace lua { namespace detail {

	int on_panic(lua_State *vm)
	{
		throw panic_error(lua_tostring(vm, -1));
	}

} } }

xd::lua::vm::vm()
{
	m_vm = luaL_newstate();
	luaL_openlibs(m_vm);
	luabind::open(m_vm);
	lua_atpanic(m_vm, &detail::on_panic);
}

xd::lua::vm::~vm()
{
	lua_close(m_vm);
}

lua_State *xd::lua::vm::get_vm()
{
	return m_vm;
}

xd::lua::script_handle xd::lua::vm::load(const std::string& code)
{
	int status = luaL_loadbuffer(m_vm, code.c_str(), code.size(), "");
	if (status != 0) {
		std::string error_message = lua_tostring(m_vm, -1);
		lua_pop(m_vm, 1);
		throw new script_load_failed(error_message);
	}

	// register the loaded function in the registry
	int ref = luaL_ref(m_vm, LUA_REGISTRYINDEX);

	// return the handle
	return script_handle(new script(m_vm, ref));
}

xd::lua::script_handle xd::lua::vm::load_file(const std::string& filename)
{
	int status = luaL_loadfile(m_vm, filename.c_str());
	if (status != 0) {
		std::string error_message = lua_tostring(m_vm, -1);
		lua_pop(m_vm, 1);
		throw script_load_failed(error_message);
	}

	// register the loaded function in the registry
	int ref = luaL_ref(m_vm, LUA_REGISTRYINDEX);

	// return the handle
	return script_handle(new script(m_vm, ref));
}

/*
** The rest of the stuff is about
** binding the default xd stuff into
** lua
*/

#include <iostream>

namespace xd { namespace lua { namespace wrapper {

	bool window_pressed_key1(const window& window, const key& key)
	{
		return window.pressed(key);
	}

	bool window_pressed_key2(const window& window, const key& key, int modifiers)
	{
		return window.pressed(key, modifiers);
	}

	bool window_pressed_virtual1(const window& window, const std::string& key)
	{
		return window.pressed(key);
	}

	bool window_pressed_virtual2(const window& window, const std::string& key, int modifiers)
	{
		return window.pressed(key, modifiers);
	}

	bool window_triggered_key1(const window& window, const key& key)
	{
		return window.triggered(key);
	}

	bool window_triggered_key2(const window& window, const key& key, int modifiers)
	{
		return window.triggered(key, modifiers);
	}

	bool window_triggered_virtual1(const window& window, const std::string& key)
	{
		return window.triggered(key);
	}

	bool window_triggered_virtual2(const window& window, const std::string& key, int modifiers)
	{
		return window.triggered(key, modifiers);
	}

	event_link window_bind_input_event1(window& window, std::string event_name, const luabind::object& obj)
	{
		return window.bind_input_event(event_name, function<bool>(obj, true));
	}

	event_link window_bind_input_event2(window& window, std::string event_name, const luabind::object& obj,
		const luabind::object& filter)
	{
		input_filter real_filter;
		if (luabind::type(filter) != LUA_TNIL)
			real_filter = luabind::object_cast<input_filter>(filter);
		return window.bind_input_event(event_name, function<bool>(obj, true), real_filter);
	}

	event_link window_bind_input_event3(window& window, std::string event_name, const luabind::object& obj,
		const luabind::object& filter, event_placement placement)
	{
		input_filter real_filter;
		if (luabind::type(filter) != LUA_TNIL)
			real_filter = luabind::object_cast<input_filter>(filter);
		return window.bind_input_event(event_name, function<bool>(obj, true), real_filter, placement);
	}

	input_filter input_filter_constructor1()
	{
		return input_filter();
	}

	input_filter input_filter_constructor2(input type)
	{
		return input_filter(type);
	}

	input_filter input_filter_constructor3(input type, int modifiers)
	{
		return input_filter(type, modifiers);
	}

	input_filter input_filter_constructor4(const key& physical_key)
	{
		return input_filter(physical_key);
	}

	input_filter input_filter_constructor5(const key& physical_key, int modifiers)
	{
		return input_filter(physical_key, modifiers);
	}

	input_filter input_filter_constructor6(const std::string& virtual_key)
	{
		return input_filter(virtual_key);
	}

	input_filter input_filter_constructor7(const std::string& virtual_key, int modifiers)
	{
		return input_filter(virtual_key, modifiers);
	}

	// typedefs for optional parameters
	typedef optional_property<input_args, std::string, &input_args::virtual_key> input_args_virtual_key;
	typedef optional_property<input_filter, input, &input_filter::type> input_filter_type;
	typedef optional_property<input_filter, key, &input_filter::physical_key> input_filter_physical_key;
	typedef optional_property<input_filter, std::string, &input_filter::virtual_key> input_filter_virtual_key;
	typedef optional_property<input_filter, int, &input_filter::modifiers> input_filter_modifiers;

	void test(luabind::object o)
	{
		boost::function<void(void)> func = function<void>(o);
		func();
	}

	// events
	typedef xd::event_callback<luabind::object, function<bool> > event_callback;
	typedef xd::event_info<luabind::object, function<bool> > event_info;
	typedef xd::event_bus<luabind::object, function<bool> > event_bus;

	event_link event_info_add1(event_info& info, const luabind::object& val)
	{
		return info.add(function<bool>(val, true));
	}

	event_link event_info_add2(event_info& info, const luabind::object& val, event_placement placement)
	{
		return info.add(function<bool>(val, true), placement);
	}

	event_link event_info_add3(event_info& info, const luabind::object& val, const luabind::object& filter)
	{
		return info.add(function<bool>(val, true), filter);
	}

	event_link event_info_add4(event_info& info, const luabind::object& val, const luabind::object& filter, event_placement placement)
	{
		return info.add(function<bool>(val, true), filter, placement);
	}
}}}

void xd::lua::vm::load_library(const std::string& module_name)
{
	using namespace luabind;

	module(m_vm, module_name.size() ? module_name.c_str() : 0)
	[
		// key struct
		class_<key>("key")
			.def(constructor<>())
			.def(self == other<key>())
			.def_readwrite("type", &key::type)
			.def_readwrite("code", &key::code),

		// input args
		class_<input_args>("input_args")
			.def(constructor<>())
			.def_readwrite("physical_key", &input_args::physical_key)
			.def_readwrite("modifiers", &input_args::modifiers)
			.property("virtual_key", &wrapper::input_args_virtual_key::getter,
				&wrapper::input_args_virtual_key::setter),

		// input filter
		class_<input_filter>("input_filter_impl")
			.property("type", &wrapper::input_filter_type::getter,
				&wrapper::input_filter_type::setter)
			.property("physical_key", &wrapper::input_filter_physical_key::getter,
				&wrapper::input_filter_physical_key::setter)
			.property("virtual_key", &wrapper::input_filter_virtual_key::getter,
				&wrapper::input_filter_virtual_key::setter)
			.property("modifiers", &wrapper::input_filter_modifiers::getter,
				&wrapper::input_filter_modifiers::setter),

		// input filter constructors
		def("input_filter", &wrapper::input_filter_constructor1),
		def("input_filter", &wrapper::input_filter_constructor2),
		def("input_filter", &wrapper::input_filter_constructor3),
		def("input_filter", &wrapper::input_filter_constructor4),
		def("input_filter", &wrapper::input_filter_constructor5),
		def("input_filter", &wrapper::input_filter_constructor6),
		def("input_filter", &wrapper::input_filter_constructor7),

		// window class
		class_<window>("window")
			.def(constructor<const std::string&, int, int>())
			.def("update", &window::update)
			.def("clear", &window::clear)
			.def("swap", &window::swap)
			.def("closed", &window::closed)
			.def("bind_key", &window::bind_key)
			.def("unbind_key", (void (window::*)(const key&))&window::unbind_key)
			.def("unbind_key", (void (window::*)(const std::string&))&window::unbind_key)
			.def("pressed", &wrapper::window_pressed_key1)
			.def("pressed", &wrapper::window_pressed_key2)
			.def("pressed", &wrapper::window_pressed_virtual1)
			.def("pressed", &wrapper::window_pressed_virtual2)
			.def("triggered", &wrapper::window_triggered_key1)
			.def("triggered", &wrapper::window_triggered_key2)
			.def("triggered", &wrapper::window_triggered_virtual1)
			.def("triggered", &wrapper::window_triggered_virtual2)
			.def("modifier", &window::modifier)
			.def("bind_input_event", &wrapper::window_bind_input_event1)
			.def("bind_input_event", &wrapper::window_bind_input_event2)
			.def("bind_input_event", &wrapper::window_bind_input_event3),

		// bind event_info
		class_<wrapper::event_info>("event_info")
			.def(constructor<>())
			.def("add", &wrapper::event_info_add1)
			.def("add", &wrapper::event_info_add2)
			.def("add", &wrapper::event_info_add3)
			.def("add", &wrapper::event_info_add4)
			.def("call", &wrapper::event_info::operator())
			.def(self(luabind::object())),

		// bind event_bus
		class_<wrapper::event_bus>("event_bus")
			.def(constructor<>())
			.def("get", &wrapper::event_bus::operator[]),

		// global functions
		def("test", &wrapper::test)
	];

	// get the xd namespace table
	object xd_table = globals(m_vm)["xd"];

	// bind constants
	xd_table["keyboard"] = keyboard;
	xd_table["mouse"] = mouse;

	xd_table["key_down"] = key_down;
	xd_table["key_left"] = key_left;
	xd_table["key_up"] = key_up;
	xd_table["key_right"] = key_right;
	xd_table["key_space"] = key_space;
	xd_table["key_esc"] = key_esc;

	xd_table["mouse_left"] = mouse_left;
	xd_table["mouse_middle"] = mouse_middle;
	xd_table["mouse_right"] = mouse_right;
	xd_table["mouse_extra1"] = mouse_extra1;
	xd_table["mouse_extra2"] = mouse_extra2;

	xd_table["mod_none"] = mod_none;
	xd_table["mod_lshift"] = mod_lshift;
	xd_table["mod_rshift"] = mod_rshift;
	xd_table["mod_lctrl"] = mod_lctrl;
	xd_table["mod_rctrl"] = mod_rctrl;
	xd_table["mod_lalt"] = mod_lalt;
	xd_table["mod_ralt"] = mod_ralt;
	xd_table["mod_lmeta"] = mod_lmeta;
	xd_table["mod_rmeta"] = mod_rmeta;
	xd_table["mod_num"] = mod_num;
	xd_table["mod_caps"] = mod_caps;
	xd_table["mod_mode"] = mod_mode;

	xd_table["event_prepend"] = event_prepend;
	xd_table["event_append"] = event_append;
}
