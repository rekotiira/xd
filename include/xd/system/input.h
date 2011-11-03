#ifndef H_XD_SYSTEM_TYPES
#define H_XD_SYSTEM_TYPES

#include <string>
#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>
#include <xd/common.h>

namespace xd
{
	enum input
	{
		keyboard,
		mouse
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
	extern XD_API const key key_left;
	extern XD_API const key key_right;
	extern XD_API const key key_up;
	extern XD_API const key key_down;
	extern XD_API const key key_space;
	extern XD_API const key key_esc;
	extern XD_API const key key_a;
	extern XD_API const key key_b;
	// ...
	extern XD_API const key key_z;
	extern XD_API const key key_0;
	extern XD_API const key key_1;
	extern XD_API const key key_2;
	extern XD_API const key key_3;
	extern XD_API const key key_4;
	extern XD_API const key key_5;
	extern XD_API const key key_6;
	extern XD_API const key key_7;
	extern XD_API const key key_8;
	extern XD_API const key key_9;

	// pre-defined mouse keys
	extern XD_API const key mouse_left;
	extern XD_API const key mouse_right;
	extern XD_API const key mouse_middle;
	extern XD_API const key mouse_extra1;
	extern XD_API const key mouse_extra2;

	// modifiers
	extern XD_API const int mod_none;
	extern XD_API const int mod_lshift;
	extern XD_API const int mod_rshift;
	extern XD_API const int mod_lctrl;
	extern XD_API const int mod_rctrl;
	extern XD_API const int mod_lalt;
	extern XD_API const int mod_ralt;
	extern XD_API const int mod_lmeta;
	extern XD_API const int mod_rmeta;
	extern XD_API const int mod_num;
	extern XD_API const int mod_caps;
	extern XD_API const int mod_mode;
}

#endif
