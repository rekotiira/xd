#include <SDL/SDL.h>
#include <xd/system.h>

xd::window::window(const std::string& title, int width, int height)
	: m_closed(false)
	, m_width(width)
	, m_height(height)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_width, m_height, SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL);
	if (!m_window) {
		throw xd::window_creation_failed();
	}
	m_context = SDL_GL_CreateContext(m_window);

	/*GLenum err = glewInit();
	if (err != GLEW_OK) {
		SDL_GL_DeleteContext(m_context);
		SDL_DestroyWindow(m_window);
		throw xd::window_creation_failed();
	}*/

	SDL_GL_SetSwapInterval(1);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	// intialize ticks
	m_current_ticks = m_last_ticks = m_last_fps_update = SDL_GetTicks();
	m_fps = m_frame_count = 0;
}

xd::window::~window()
{
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);

	SDL_Quit();
}

void xd::window::update()
{
	// clear the triggered keys
	m_triggered_keys.clear();

	// clear closed flag
	m_closed = false;

	// check all events
	SDL_Event evt;
	while (SDL_PollEvent(&evt)) {
		switch (evt.type) {
			case SDL_QUIT:
			{
				// window was closed
				m_closed = true;
				break;
			}
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			{
				// don't process key repeats (at least for now)
				if (evt.type == SDL_KEYDOWN && evt.key.repeat) {
					break;
				}

				// construct event arguments
				input_args args;
				if (evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) {
					args.physical_key = keyb(evt.key.keysym.sym);
				} else {
					args.physical_key = mouseb(evt.button.button);
				}
				args.modifiers = SDL_GetModState();

				// find associated virtual key
				xd::window::key_table_t::iterator i = m_key_to_virtual.find(args.physical_key);
				if (i != m_key_to_virtual.end()) {
					args.virtual_key = i->second;
				}

				// add to triggered keys if keydown event and launch the event
				if (evt.type == SDL_KEYDOWN || evt.type == SDL_MOUSEBUTTONDOWN) {
					m_triggered_keys.insert(args.physical_key);
					m_input_events["key_down"](args);
				} else {
					m_input_events["key_up"](args);
				}
				break;
			}
		}
	}

	// update ticks
	m_last_ticks = m_current_ticks;
	m_current_ticks = SDL_GetTicks();

	// invoke tick handler if necessary
	if (m_tick_handler) {
		m_tick_handler_counter += delta_ticks();
		while (m_tick_handler_counter >= m_tick_handler_interval) {
			m_tick_handler();
			m_tick_handler_counter -= m_tick_handler_interval;
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
}

void xd::window::clear()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void xd::window::swap()
{
	SDL_GL_SwapWindow(m_window);
}

bool xd::window::closed() const
{
	return m_closed;
}

int xd::window::width() const
{
	return m_width;
}

int xd::window::height() const
{
	return m_height;
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
	if (key.type == xd::keyboard) {
		Uint8 *state = SDL_GetKeyboardState(0);
		if (state[SDL_GetScancodeFromKey(key.code)] && modifier(modifiers))
			return true;
	}
	if (key.type == xd::mouse) {
		if ((SDL_GetMouseState(0, 0) & SDL_BUTTON(key.code)) != 0 && modifier(modifiers))
			return true;
	}
	return false;
}

bool xd::window::pressed(const std::string& key, int modifiers) const
{
	// find if this virtual key is bound
	xd::window::virtual_table_t::const_iterator i = m_virtual_to_key.find(key);
	if (i != m_virtual_to_key.end()) {
		// get keystate
		Uint8 *state = SDL_GetKeyboardState(0);
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
	return ((modifiers & SDL_GetModState()) == modifiers);
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