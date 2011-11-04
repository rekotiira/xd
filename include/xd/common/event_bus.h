#ifndef H_XD_COMMON_EVENT_BUS
#define H_XD_COMMON_EVENT_BUS

#include <list>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <boost/utility/enable_if.hpp>
#include <boost/optional.hpp>
#include <xd/common/types.h>

namespace xd
{
	// where to place the event
	enum event_placement
	{
		EVENT_PREPEND,
		EVENT_APPEND
	};

	// event links, used to remove events from batch
	typedef std::size_t event_link;

	// an event callback, holds one callback for an event
	template <typename Args>
	class event_callback
	{
	public:
		typedef std::function<bool (const Args&)> callback_t;
		typedef std::function<bool (const Args&)> filter_t;

		event_callback(callback_t callback)
			: m_callback(callback)
		{
		}

		event_callback(callback_t callback, const filter_t& filter)
			: m_callback(callback)
			, m_filter(filter)
		{
		}

		bool operator()(const Args& args) const
		{
			if (!m_filter || m_filter(args)) {
				return m_callback(args);
			}
			return true;
		}

	private:
		callback_t m_callback;
		filter_t m_filter;
	};

	// event info class, holds list of callbacks for an event
	template <typename Args>
	class event_info
	{
	public:
		typedef event_callback<Args> event_callback_t;

		event_info()
			: m_counter(0)
		{
		}

		event_link add(typename event_callback_t::callback_t callback, event_placement placement = EVENT_PREPEND)
		{
			std::size_t link = m_counter++;
			if (placement == EVENT_PREPEND)
				m_callbacks.push_front(std::make_pair(link, typename event_callback_t(callback)));
			else
				m_callbacks.push_back(std::make_pair(link, typename  event_callback_t(callback)));
			return link;
		}

		event_link add(typename event_callback_t::callback_t callback, typename event_callback_t::filter_t filter, event_placement placement = EVENT_PREPEND)
		{
			std::size_t link = m_counter++;
			if (placement == EVENT_PREPEND)
				m_callbacks.push_front(std::make_pair(link, typename  event_callback_t(callback, filter)));
			else
				m_callbacks.push_back(std::make_pair(link, typename event_callback_t(callback, filter)));
			return link;
		}

		void remove(event_link link)
		{
			for (auto i = m_callbacks.begin(); i != m_callbacks.end(); ++i) {
				if (i->first == link) {
					m_callbacks.erase(i);
					return;
				}
			}

			// link wasn't found
			throw std::invalid_argument("link");
		}

		void operator()(const Args& args) const
		{
			// iterate through all callbacks
			for (auto i = m_callbacks.begin(); i != m_callbacks.end(); ++i) {
				bool status = (i->second)(args); // call the callback
				if (!status)
					break;
			}
		}
	protected:
		typedef std::list<std::pair<event_link, event_callback_t> > callback_list_t;

		callback_list_t m_callbacks;
		std::size_t m_counter;
	};

	// event bus class, holds list of events
	template <typename Args>
	class event_bus
	{
	public:
		typedef std::function<bool (const Args&)> callback_t;
		typedef event_info<Args> event_info_t;

		virtual ~event_bus()
		{
		}

		event_info_t& operator[](const std::string& key)
		{
			return m_events[key];
		}

		const event_info_t& operator[](const std::string& key) const
		{
			return m_events[key];
		}

		event_info_t& get(const std::string& key)
		{
			return m_events[key];
		}

		const event_info_t& get(const std::string& key) const
		{
			return m_events[key];
		}
	
	private:
		std::unordered_map<std::string, event_info<Args>> m_events;
	};
}

#endif
