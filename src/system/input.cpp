#include <xd/system/input.h>
#include <SDL/SDL.h>

namespace xd
{
	// pre-defined keyboard keys
	const key key_left = keyb(SDLK_LEFT);
	const key key_right = keyb(SDLK_RIGHT);
	const key key_up = keyb(SDLK_UP);
	const key key_down = keyb(SDLK_DOWN);
	const key key_space = keyb(SDLK_SPACE);
	const key key_esc = keyb(SDLK_ESCAPE);
	const key key_a = keyb(SDLK_a);
	const key key_b = keyb(SDLK_b);
	// ...
	const key key_z = keyb(SDLK_z);
	const key key_0 = keyb(SDLK_0);
	const key key_1 = keyb(SDLK_1);
	const key key_2 = keyb(SDLK_2);
	const key key_3 = keyb(SDLK_3);
	const key key_4 = keyb(SDLK_4);
	const key key_5 = keyb(SDLK_5);
	const key key_6 = keyb(SDLK_6);
	const key key_7 = keyb(SDLK_7);
	const key key_8 = keyb(SDLK_8);
	const key key_9 = keyb(SDLK_9);

	// pre-defined mouse keys
	const key mouse_left = mouseb(SDL_BUTTON_LEFT);
	const key mouse_right = mouseb(SDL_BUTTON_RIGHT);
	const key mouse_middle = mouseb(SDL_BUTTON_MIDDLE);
	const key mouse_extra1 = mouseb(SDL_BUTTON_X1);
	const key mouse_extra2 = mouseb(SDL_BUTTON_X2);

	// modifiers
	const int mod_none = KMOD_NONE;
	const int mod_lshift = KMOD_LSHIFT;
	const int mod_rshift = KMOD_RSHIFT;
	const int mod_lctrl = KMOD_LCTRL;
	const int mod_rctrl = KMOD_RCTRL;
	const int mod_lalt = KMOD_LALT;
	const int mod_ralt = KMOD_RALT;
	const int mod_lmeta = KMOD_LMETA;
	const int mod_rmeta = KMOD_RMETA;
	const int mod_num = KMOD_NUM;
	const int mod_caps = KMOD_CAPS;
	const int mod_mode = KMOD_MODE;
}
