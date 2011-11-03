#ifndef H_XD_SYSTEM_WINDOW
#define H_XD_SYSTEM_WINDOW

#include <string>
#include <xd/system/input.h>
#include <xd/common/event_bus.h>
#include <xd/graphics/transform_geometry.h>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/function.hpp>
#include <boost/cstdint.hpp>

struct SDL_Window;

#ifndef XD_STATIC
// disable warning about boost::noncopyable not being dll-exportable
// as well as the private members that can't be accessed by client
#pragma warning(disable: 4275 4251)
#endif

namespace xd
{
	class XD_API window : public boost::noncopyable
	{
	public:
		// typedefs
		typedef event_bus<input_args, input_filter>::callback_t input_event_callback_t;
		typedef boost::function<void ()> tick_callback_t;

		// public interface
		window(const std::string& title, int width, int height);
		virtual ~window();

		void update();
		void clear();
		void swap();

		bool closed() const;
		int width() const;
		int height() const;

		// ticks stuff
		int delta_ticks() const;
		float delta_time() const;
		void register_tick_handler(tick_callback_t callback, boost::uint32_t interval);
		void unregister_tick_handler();
		int fps() const;
		int frame_count() const;

		void bind_key(const key& physical_key, const std::string& virtual_key);
		void unbind_key(const key& key);
		void unbind_key(const std::string& key);

		bool pressed(const key& key, int modifiers = 0) const;
		bool pressed(const std::string& key, int modifiers = 0) const;

		bool triggered(const key& key, int modifiers = 0) const;
		bool triggered(const std::string& key, int modifiers = 0) const;

		bool modifier(int modifiers) const;

		event_link bind_input_event(const std::string& event_name, input_event_callback_t callback,
			const input_filter& filter = input_filter(), event_placement place = event_prepend);
		void unbind_input_event(const std::string& event_name, event_link link);

		// an utility template function for member function callbacks, so user doesn't have to use boost::bind directly
		template <typename T>
		event_link bind_input_event(const std::string& event_name, bool (T::*callback)(const input_args&), T* instance,
			const input_filter& filter = input_filter(), event_placement place = event_prepend)
		{
			return bind_input_event(event_name, boost::bind(callback, instance, _1), filter, place);
		}

	private:
		SDL_Window* m_window;
		void *m_context;

		// was window closed
		bool m_closed;

		// window width/height
		int m_width;
		int m_height;

		// keep track of ticks
		boost::uint32_t m_current_ticks;
		boost::uint32_t m_last_ticks;
		boost::uint32_t m_tick_handler_counter;
		boost::uint32_t m_tick_handler_interval;
		tick_callback_t m_tick_handler;

		// fps stuff
		int m_fps;
		int m_frame_count;
		boost::uint32_t m_last_fps_update;

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
