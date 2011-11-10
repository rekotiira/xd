#ifndef H_XD_SYSTEM_INPUT
#define H_XD_SYSTEM_INPUT

#include <string>
#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>
#include <xd/common.hpp>
#include <xd/types.hpp>

namespace xd
{
	enum input_type
	{
		INPUT_KEYBOARD,
		INPUT_MOUSE
	};

	struct key
	{
		input_type type;
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
		input_filter(input_type type, boost::optional<int> mod = boost::none) : type(type), modifiers(mod) {}
		input_filter(const key& pkey, boost::optional<int> mod = boost::none) : physical_key(pkey), modifiers(mod) {}
		input_filter(std::string vkey, boost::optional<int> mod = boost::none) : virtual_key(vkey), modifiers(mod) {}

		boost::optional<input_type> type;
		boost::optional<key> physical_key;
		boost::optional<std::string> virtual_key;
		boost::optional<int> modifiers;

		bool operator()(const input_args& args)
		{
			if (type != boost::none && *type != args.physical_key.type)
				return false;
			if (physical_key != boost::none && *physical_key != args.physical_key)
				return false;
			if (virtual_key != boost::none && *virtual_key != args.virtual_key)
				return false;
			if (modifiers != boost::none && (*modifiers & args.modifiers) != *modifiers)
				return false;
			return true;
		}
	};

	// utility function to create key
	inline key KEY(int code)
	{
		key k;
		k.type = INPUT_KEYBOARD;
		k.code = code;
		return k;
	}

	// utility function to create mouse
	inline key MOUSE(int code)
	{
		key k;
		k.type = INPUT_MOUSE;
		k.code = code;
		return k;
	}

	// pre-defined keyboard keys
	extern XD_API const key KEY_LEFT;
	extern XD_API const key KEY_RIGHT;
	extern XD_API const key KEY_UP;
	extern XD_API const key KEY_DOWN;
	extern XD_API const key KEY_ENTER;
	extern XD_API const key KEY_SPACE;
	extern XD_API const key KEY_ESC;
	extern XD_API const key KEY_A;
	extern XD_API const key KEY_B;
	extern XD_API const key KEY_C;
	extern XD_API const key KEY_D;
	extern XD_API const key KEY_E;
	extern XD_API const key KEY_F;
	extern XD_API const key KEY_G;
	extern XD_API const key KEY_H;
	extern XD_API const key KEY_I;
	extern XD_API const key KEY_J;
	extern XD_API const key KEY_K;
	extern XD_API const key KEY_L;
	extern XD_API const key KEY_M;
	extern XD_API const key KEY_N;
	extern XD_API const key KEY_O;
	extern XD_API const key KEY_P;
	extern XD_API const key KEY_Q;
	extern XD_API const key KEY_R;
	extern XD_API const key KEY_S;
	extern XD_API const key KEY_T;
	extern XD_API const key KEY_U;
	extern XD_API const key KEY_V;
	extern XD_API const key KEY_W;
	extern XD_API const key KEY_X;
	extern XD_API const key KEY_Y;
	extern XD_API const key KEY_Z;
	extern XD_API const key KEY_0;
	extern XD_API const key KEY_1;
	extern XD_API const key KEY_2;
	extern XD_API const key KEY_3;
	extern XD_API const key KEY_4;
	extern XD_API const key KEY_5;
	extern XD_API const key KEY_6;
	extern XD_API const key KEY_7;
	extern XD_API const key KEY_8;
	extern XD_API const key KEY_9;

	// pre-defined mouse keys
	extern XD_API const key MOUSE_LEFT;
	extern XD_API const key MOUSE_RIGHT;
	extern XD_API const key MOUSE_MIDDLE;
	extern XD_API const key MOUSE_1;
	extern XD_API const key MOUSE_2;
	extern XD_API const key MOUSE_3;
	extern XD_API const key MOUSE_4;
	extern XD_API const key MOUSE_5;
	extern XD_API const key MOUSE_6;
	extern XD_API const key MOUSE_7;
	extern XD_API const key MOUSE_8;

	// modifiers
	/*extern XD_API const int mod_none;
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
	extern XD_API const int mod_mode;*/
}

// specialize hash<> for xd::key
namespace std
{
	template <>
	struct hash<xd::key>
	{
		size_t operator()(const xd::key& k) const
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, k.type);
			boost::hash_combine(seed, k.code);
			return seed;
		}
	};
}

#endif
