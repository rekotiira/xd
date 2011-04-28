#ifndef H_XD_SYSTEM_WINDOW
#define H_XD_SYSTEM_WINDOW

#include <SDL/SDL.h>
#include <string>
#include <xd/system/types.h>
#include <xd/common/event_bus.h>
#include <xd/graphics/transform_geometry.h>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/function.hpp>

namespace xd
{
	class window : public boost::noncopyable
	{
	public:
		// event bus typedefs
		typedef event_bus<input_args, input_filter>::callback_t input_event_callback_t;

		// public interface
		window(const char *title);
		virtual ~window();

		void update();
		void clear();
		void swap();

		bool closed();

		void bind_key(const key& physical_key, std::string virtual_key);
		void unbind_key(const key& key);
		void unbind_key(const std::string& key);

		bool pressed(const key& key, int modifiers = 0);
		bool pressed(const std::string& key, int modifiers = 0);

		bool triggered(const key& key, int modifiers = 0);
		bool triggered(const std::string& key, int modifiers = 0);

		bool modifier(int modifiers);

		event_link bind_input_event(std::string event_name, input_event_callback_t callback,
			const input_filter& filter = input_filter(), event_placement place = event_prepend);
		void unbind_input_event(std::string event_name, event_link link);

		// an utility template function for member function callbacks, so user doesn't have to use boost::bind directly
		template <typename T>
		event_link bind_input_event(std::string event_name, bool (T::*callback)(const input_args&), T* instance,
			const input_filter& filter = input_filter(), event_placement place = event_prepend)
		{
			return bind_input_event(event_name, boost::bind(callback, instance, _1), filter, place);
		}

	private:
		SDL_Window* m_window;
		SDL_GLContext m_context;

		// was window closed
		bool m_closed;

		// internal typedefs
		typedef boost::unordered_set<key> key_set_t;
		typedef boost::unordered_map<key, std::string> key_table_t;
		typedef boost::unordered_map<std::string, key_set_t> virtual_table_t;
		typedef boost::unordered_set<key> trigger_keys_t;

		// virtual key conversions
		key_table_t m_key_to_virtual;
		virtual_table_t m_virtual_to_key;

		// key triggers
		trigger_keys_t m_triggered_keys;

		// event busses
		event_bus<input_args, input_filter> m_input_events;
	};
}

#endif
