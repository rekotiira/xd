#include <xd/window/application.h>
#include <xd/window.h>
#include <SDL/SDL.h>
#include <iostream>

xd::application::application()
{
	SDL_Init(SDL_INIT_EVERYTHING);
}

xd::application::~application()
{
	SDL_Quit();
}

void xd::application::update()
{
	SDL_Event evt;
	while (SDL_PollEvent(&evt)) {
		if (evt.type == SDL_QUIT) {
			std::cout << "ok: " << evt.window.windowID << std::endl;
		}
	}
}

void xd::application::add_window(xd::window* win)
{
	//std::cout << "win id: " << SDL_GetWindowID(win->m_window) << std::endl;
}

void xd::application::del_window(xd::window* win)
{
}