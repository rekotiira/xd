#include <GL/glew.h>
#include <GL/glfw.h>
#include <xd/system/window.hpp>
#include <xd/system/exceptions.hpp>

// detail stuff, hidden from user
namespace
{
	xd::window *window_instance = nullptr;

	void on_key_proxy(int key, int action)
	{
		window_instance->on_input(xd::INPUT_KEYBOARD, key, action);
	}

	void on_mouse_proxy(int key, int action)
	{
		window_instance->on_input(xd::INPUT_MOUSE, key, action);
	}
};

xd::window::window(const std::string& title, int width, int height, const window_options& options)
	: m_width(width)
	, m_height(height)
	, m_in_update(false)
{
	// check if there's already a window alive
	if (window_instance) {
		throw xd::window_creation_failed();
	}

	// initialize glfw
	if (glfwInit() == GL_FALSE) {
		throw xd::window_creation_failed();
	}
	
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, !options.allow_resize);
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, options.antialiasing_level);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, options.major_version);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, options.minor_version);

	if (glfwOpenWindow(m_width, m_height, 8, 8, 8, 8, options.depth_bits, options.stencil_bits,
		options.fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW) == GL_FALSE)
	{
		glfwTerminate();
		throw xd::window_creation_failed();
	}

	glfwSetWindowTitle(title.c_str());
	if (options.display_cursor)
		glfwEnable(GLFW_MOUSE_CURSOR);
	else
		glfwDisable(GLFW_MOUSE_CURSOR);

	glfwEnable(GLFW_SYSTEM_KEYS);
	glfwDisable(GLFW_AUTO_POLL_EVENTS);
	glfwDisable(GLFW_STICKY_KEYS);
	glfwDisable(GLFW_STICKY_MOUSE_BUTTONS);
	glfwDisable(GLFW_KEY_REPEAT);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		glfwCloseWindow();
		glfwTerminate();
		throw xd::window_creation_failed();
	}

	glfwSwapInterval(1);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	// intialize ticks
	m_current_ticks = m_last_ticks = m_last_fps_update = static_cast<boost::uint32_t>(glfwGetTime() * 1000);
	m_fps = m_frame_count = 0;

	// register input callbacks
	glfwSetKeyCallback(&on_key_proxy);
	glfwSetMouseButtonCallback(&on_mouse_proxy);

	window_instance = this;
}

xd::window::~window()
{
	glfwCloseWindow();
	glfwTerminate();
	window_instance = nullptr;
}

void xd::window::on_input(input_type type, int key, int action)
{
	// construct event arguments
	input_args args;
	if (type == INPUT_KEYBOARD)
		args.physical_key = KEY(key);
	else
		args.physical_key = MOUSE(key);
	args.modifiers = 0;

	// find associated virtual key
	xd::window::key_table_t::iterator i = m_key_to_virtual.find(args.physical_key);
	if (i != m_key_to_virtual.end()) {
		args.virtual_key = i->second;
	}

	// add to triggered keys if keydown event and launch the event
	if (action == GLFW_PRESS) {
		m_triggered_keys.insert(args.physical_key);
		m_tick_handler_triggered_keys.insert(args.physical_key);
		m_input_events["key_down"](args);
	} else {
		m_input_events["key_up"](args);
	}
}

void xd::window::update()
{
	// this is used to keep track which triggered keys list triggered() uses
	m_in_update = true;

	// clear the triggered keys
	m_triggered_keys.clear();

	// trigger input callbacks
	glfwPollEvents();

	// update ticks
	m_last_ticks = m_current_ticks;
	m_current_ticks = static_cast<boost::uint32_t>(glfwGetTime() * 1000);

	// invoke tick handler if necessary
	if (m_tick_handler) {
		m_tick_handler_counter += delta_ticks();
		while (m_tick_handler_counter >= m_tick_handler_interval) {
			m_tick_handler();
			m_tick_handler_counter -= m_tick_handler_interval;
			m_tick_handler_triggered_keys.clear();
		}
	}

	// calculate fps
	while (m_current_ticks >= (m_last_fps_update + 1000)) {
		m_fps = m_frame_count;
		m_frame_count = 0;
		m_last_fps_update += 1000;
	}

	// increase frame count
	m_frame_count++;
	m_in_update = false;
}

void xd::window::clear()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void xd::window::swap()
{
	glfwSwapBuffers();
}

bool xd::window::closed() const
{
	return (glfwGetWindowParam(GLFW_OPENED) == GL_FALSE);
}

int xd::window::width() const
{
	int width;
	glfwGetWindowSize(&width, nullptr);
	return width;
}

int xd::window::height() const
{
	int height;
	glfwGetWindowSize(nullptr, &height);
	return height;
}

int xd::window::ticks() const
{
	return m_current_ticks;
}

int xd::window::delta_ticks() const
{
	return m_current_ticks - m_last_ticks;
}

float xd::window::delta_time() const
{
	return static_cast<float>(delta_ticks()) / 1000.0f;
}

void xd::window::register_tick_handler(tick_callback_t callback, boost::uint32_t interval)
{
	m_tick_handler = callback;
	m_tick_handler_interval = interval;
	m_tick_handler_counter = 0;
}

void xd::window::unregister_tick_handler()
{
	m_tick_handler = 0;
}

int xd::window::fps() const
{
	return m_fps;
}

int xd::window::frame_count() const
{
	return m_frame_count;
}

void xd::window::bind_key(const xd::key& physical_key, const std::string& virtual_key)
{
	// find if the physical key is bound
	xd::window::key_table_t::iterator i = m_key_to_virtual.find(physical_key);
	if (i == m_key_to_virtual.end()) {
		// if it's not found, add it to the tables
		m_key_to_virtual[physical_key] = virtual_key;
		m_virtual_to_key[virtual_key].insert(physical_key);
	}
}

void xd::window::unbind_key(const xd::key& physical_key)
{
	// find if the physical key is bound
	xd::window::key_table_t::iterator i = m_key_to_virtual.find(physical_key);
	if (i != m_key_to_virtual.end()) {
		// erase physical key from the virtual key's list
		m_virtual_to_key[i->second].erase(physical_key);
		// erase the physical key itself
		m_key_to_virtual.erase(i);
	}
}

void xd::window::unbind_key(const std::string& virtual_key)
{
	// find if the virtual key is bound
	xd::window::virtual_table_t::iterator i = m_virtual_to_key.find(virtual_key);
	if (i != m_virtual_to_key.end()) {
		// erase all the keys associated to this virtual key
		for (xd::window::key_set_t::iterator j = i->second.begin(); j != i->second.end(); ++j) {
			m_key_to_virtual.erase(*j);
		}
		// erase the virtual key itself
		m_virtual_to_key.erase(i);
	}
}

bool xd::window::pressed(const xd::key& key, int modifiers) const
{
	if (key.type == xd::INPUT_KEYBOARD) {
		if (glfwGetKey(key.code))
			return true;
		/*Uint8 *state = SDL_GetKeyboardState(0);
		if (state[SDL_GetScancodeFromKey(key.code)] && modifier(modifiers))
			return true;*/
	}
	if (key.type == xd::INPUT_MOUSE) {
		if (glfwGetMouseButton(key.code))
				return true;
		/*if ((SDL_GetMouseState(0, 0) & SDL_BUTTON(key.code)) != 0 && modifier(modifiers))
			return true;*/
	}
	return false;
}

bool xd::window::pressed(const std::string& key, int modifiers) const
{
	// find if this virtual key is bound
	xd::window::virtual_table_t::const_iterator i = m_virtual_to_key.find(key);
	if (i != m_virtual_to_key.end()) {
		// iterate through each physical key
		for (xd::window::key_set_t::iterator j = i->second.begin(); j != i->second.end(); ++j) {
			if (pressed(*j, modifiers))
					return true;
		}
	}
	return false;
}

bool xd::window::triggered(const xd::key& key, int modifiers) const
{
	if (m_in_update)
		return (m_tick_handler_triggered_keys.find(key) != m_tick_handler_triggered_keys.end() && modifier(modifiers));
	else
		return (m_triggered_keys.find(key) != m_triggered_keys.end() && modifier(modifiers));
}

bool xd::window::triggered(const std::string& key, int modifiers) const
{
	// find if this virtual key is bound
	xd::window::virtual_table_t::const_iterator i = m_virtual_to_key.find(key);
	if (i != m_virtual_to_key.end()) {
		// iterate through each physical key
		for (xd::window::key_set_t::iterator j = i->second.begin(); j != i->second.end(); ++j) {
			if (triggered(*j, modifiers))
				return true;
		}
	}
	return false;
}

bool xd::window::modifier(int modifiers) const
{
	//return ((modifiers & SDL_GetModState()) == modifiers);
	return (modifiers == 0);
}

xd::event_link xd::window::bind_input_event(const std::string& event_name, input_event_callback_t callback,
	const input_filter& filter, event_placement place)
{
	return m_input_events[event_name].add(callback, filter, place);
}

void xd::window::unbind_input_event(const std::string& event_name, event_link link)
{
	m_input_events[event_name].remove(link);
}