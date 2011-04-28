#ifndef H_XD_SYSTEM_TYPES
#define H_XD_SYSTEM_TYPES

#include <SDL/SDL.h>
#include <string>
#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>

namespace xd
{
	enum input_type
	{
		keyboard,
		mouse
	};

	enum key_code
	{
		// keyboard
		key_left  = SDLK_LEFT,
		key_right = SDLK_RIGHT,
		key_up    = SDLK_UP,
		key_down  = SDLK_DOWN,
		key_esc   = SDLK_ESCAPE,
		key_space = SDLK_SPACE,
		key_a     = SDLK_a,
		key_b     = SDLK_b,
		key_c     = SDLK_c,
		key_d     = SDLK_d,
		key_e     = SDLK_e,
		key_f     = SDLK_f,
		key_g     = SDLK_g,
		key_h     = SDLK_h,
		key_i     = SDLK_i,
		key_j     = SDLK_j,
		key_k     = SDLK_k,
		key_l     = SDLK_l,
		key_m     = SDLK_m,
		key_n     = SDLK_n,
		key_o     = SDLK_o,
		key_p     = SDLK_p,
		key_q     = SDLK_q,
		key_r     = SDLK_r,
		key_s     = SDLK_s,
		key_t     = SDLK_t,
		key_u     = SDLK_u,
		key_v     = SDLK_v,
		key_w     = SDLK_w,
		key_x     = SDLK_x,
		key_y     = SDLK_y,
		key_z     = SDLK_z,
		key_0     = SDLK_0,
		key_1     = SDLK_1,
		key_2     = SDLK_2,
		key_3     = SDLK_3,
		key_4     = SDLK_4,
		key_5     = SDLK_5,
		key_6     = SDLK_6,
		key_7     = SDLK_7,
		key_8     = SDLK_8,
		key_9     = SDLK_9,

		// mouse
		mouse_left   = SDL_BUTTON_LEFT,
		mouse_middle = SDL_BUTTON_RIGHT,
		mouse_right  = SDL_BUTTON_MIDDLE,
		mouse_extra1 = SDL_BUTTON_X1,
		mouse_extra2 = SDL_BUTTON_X2
	};

	enum key_mod
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
		input_type type;
		key_code code;
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
		input_filter(input_type itype, boost::optional<int> mod = any) : type(itype), modifiers(mod) {}
		input_filter(const key& pkey, boost::optional<int> mod = any) : physical_key(pkey), modifiers(mod) {}
		input_filter(std::string vkey, boost::optional<int> mod = any) : virtual_key(vkey), modifiers(mod) {}

		boost::optional<input_type> type;
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
}

#endif
