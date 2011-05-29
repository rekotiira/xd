#ifndef H_XD_SYSTEM_TYPES
#define H_XD_SYSTEM_TYPES

#include <SDL/SDL.h>
#include <string>
#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>

namespace xd
{
	enum input
	{
		keyboard,
		mouse
	};

	/*enum keycode
	{
		// keyboard
		keycode_key_left  = SDLK_LEFT,
		keycode_key_right = SDLK_RIGHT,
		keycode_key_up    = SDLK_UP,
		keycode_key_down  = SDLK_DOWN,
		keycode_key_esc   = SDLK_ESCAPE,
		keycode_key_space = SDLK_SPACE,
		keycode_key_a     = SDLK_a,
		keycode_key_b     = SDLK_b,
		keycode_key_c     = SDLK_c,
		keycode_key_d     = SDLK_d,
		keycode_key_e     = SDLK_e,
		keycode_key_f     = SDLK_f,
		keycode_key_g     = SDLK_g,
		keycode_key_h     = SDLK_h,
		keycode_key_i     = SDLK_i,
		keycode_key_j     = SDLK_j,
		keycode_key_k     = SDLK_k,
		keycode_key_l     = SDLK_l,
		keycode_key_m     = SDLK_m,
		keycode_key_n     = SDLK_n,
		keycode_key_o     = SDLK_o,
		keycode_key_p     = SDLK_p,
		keycode_key_q     = SDLK_q,
		keycode_key_r     = SDLK_r,
		keycode_key_s     = SDLK_s,
		keycode_key_t     = SDLK_t,
		keycode_key_u     = SDLK_u,
		keycode_key_v     = SDLK_v,
		keycode_key_w     = SDLK_w,
		keycode_key_x     = SDLK_x,
		keycode_key_y     = SDLK_y,
		keycode_key_z     = SDLK_z,
		keycode_key_0     = SDLK_0,
		keycode_key_1     = SDLK_1,
		keycode_key_2     = SDLK_2,
		keycode_key_3     = SDLK_3,
		keycode_key_4     = SDLK_4,
		keycode_key_5     = SDLK_5,
		keycode_key_6     = SDLK_6,
		keycode_key_7     = SDLK_7,
		keycode_key_8     = SDLK_8,
		keycode_key_9     = SDLK_9,

		// mouse
		keycode_mouse_left   = SDL_BUTTON_LEFT,
		keycode_mouse_middle = SDL_BUTTON_RIGHT,
		keycode_mouse_right  = SDL_BUTTON_MIDDLE,
		keycode_mouse_extra1 = SDL_BUTTON_X1,
		keycode_mouse_extra2 = SDL_BUTTON_X2
	};*/

	enum mod
	{
		mod_none   = KMOD_NONE,
		mod_lshift = KMOD_LSHIFT,
		mod_rshift = KMOD_RSHIFT,
		mod_lctrl  = KMOD_LCTRL,
		mod_rctrl  = KMOD_RCTRL,
		mod_lalt   = KMOD_LALT,
		mod_ralt   = KMOD_RALT,
		mod_lmeta  = KMOD_LMETA,
		mod_rmeta  = KMOD_RMETA,
		mod_num    = KMOD_NUM,
		mod_caps   = KMOD_CAPS,
		mod_mode   = KMOD_MODE
	};

	struct key
	{
		input type;
		int code;
	};

	inline bool operator==(const key& lhs, const key& rhs)
	{
		return (lhs.type == rhs.type && lhs.code == rhs.code);
	}

	inline bool operator!=(const key& lhs, const key& rhs)
	{
		return !(lhs == rhs);
	}

	inline std::size_t hash_value(const key& k)
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, k.type);
		boost::hash_combine(seed, k.code);
		return seed;
	}

	struct input_args
	{
		key physical_key;
		boost::optional<std::string> virtual_key;
		int modifiers;
	};

	struct input_filter
	{
		input_filter() {}
		input_filter(input itype, boost::optional<int> mod = any) : type(itype), modifiers(mod) {}
		input_filter(const key& pkey, boost::optional<int> mod = any) : physical_key(pkey), modifiers(mod) {}
		input_filter(std::string vkey, boost::optional<int> mod = any) : virtual_key(vkey), modifiers(mod) {}

		boost::optional<input> type;
		boost::optional<key> physical_key;
		boost::optional<std::string> virtual_key;
		boost::optional<int> modifiers;

		bool operator()(const input_args& args)
		{
			if (type != xd::any && *type != args.physical_key.type)
				return false;
			if (physical_key != xd::any && *physical_key != args.physical_key)
				return false;
			if (virtual_key != xd::any && *virtual_key != args.virtual_key)
				return false;
			if (modifiers != xd::any && (*modifiers & args.modifiers) != *modifiers)
				return false;
			return true;
		}
	};

	// utility function to create key
	inline key keyb(int code)
	{
		key k;
		k.type = keyboard;
		k.code = code;
		return k;
	}

	// utility function to create mouse
	inline key mouseb(int code)
	{
		key k;
		k.type = mouse;
		k.code = code;
		return k;
	}

	// pre-defined keyboard keys
	const key key_left = keyb(SDLK_LEFT);
	const key key_right = keyb(SDLK_RIGHT);
	const key key_up = keyb(SDLK_UP);
	const key key_down = keyb(SDLK_DOWN);
	const key key_space = keyb(SDLK_SPACE);
	const key key_esc = keyb(SDLK_ESCAPE);
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
}

#endif
