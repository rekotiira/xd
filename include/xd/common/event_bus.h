#ifndef H_XD_COMMON_EVENT_BUS
#define H_XD_COMMON_EVENT_BUS

#include <list>
#include <string>
#include <stdexcept>
#include <boost/unordered_map.hpp>
#include <boost/function.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/optional.hpp>
#include <xd/common/types.h>

#include <iostream>

namespace xd
{
	// where to place the event
	enum event_placement
	{
		event_prepend,
		event_append
	};

	// event links, used to remove events from batch
	typedef std::size_t event_link;

	// an event callback, holds one callback for an event
	template <typename Args, typename Filter>
	class event_callback
	{
	public:
		typedef boost::function<bool (const Args&)> callback_t;

		event_callback(callback_t callback)
			: m_callback(callback)
		{
		}

		event_callback(callback_t callback, const Filter& filter)
			: m_callback(callback)
			, m_filter(filter)
		{
		}

		bool operator()(const Args& args)
		{
			if (!m_filter || (*m_filter)(args)) {
				return m_callback(args);
			}
			return true;
		}

	private:
		callback_t m_callback;
		boost::optional<Filter> m_filter;
	};

	// specialization when no filter
	template <typename Args>
	class event_callback<Args, void>
	{
	public:
		typedef boost::function<bool (const Args&)> callback_t;

		event_callback(callback_t callback)
			: m_callback(callback)
		{
		}

		bool operator()(const Args& args)
		{
			return m_callback(args);
		}

	private:
		callback_t m_callback;
	};

	// event info class, holds list of callbacks for an event
	template <typename Args, typename Filter>
	class event_info_base
	{
	public:
		typedef event_callback<Args, Filter> event_callback_t;

		event_info_base()
			: m_counter(0)
		{
		}

		void remove(event_link link)
		{
			for (callback_list_t::iterator i = m_callbacks.begin(); i != m_callbacks.end(); ++i) {
				if (i->first == link) {
					m_callbacks.erase(i);
					return;
				}
			}

			// link wasn't found
			throw std::invalid_argument("link");
		}

		void operator()(const Args& args)
		{
			// iterate through all callbacks
			for (callback_list_t::iterator i = m_callbacks.begin(); i != m_callbacks.end(); ++i) {
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

	template <typename Args, typename Filter>
	class event_info : public event_info_base<Args, Filter>
	{
	public:
		event_link add(typename event_callback_t::callback_t callback, event_placement placement = event_prepend)
		{
			std::size_t link = m_counter++;
			if (placement == event_prepend)
				m_callbacks.push_front(std::make_pair(link, event_callback_t(callback)));
			else
				m_callbacks.push_back(std::make_pair(link, event_callback_t(callback)));
			return link;
		}

		event_link add(typename event_callback_t::callback_t callback, Filter filter, event_placement placement = event_prepend)
		{
			std::size_t link = m_counter++;
			if (placement == event_prepend)
				m_callbacks.push_front(std::make_pair(link, event_callback_t(callback, filter)));
			else
				m_callbacks.push_back(std::make_pair(link, event_callback_t(callback, filter)));
			return link;
		}
	};

	template <typename Args>
	class event_info<Args, void> : public event_info_base<Args, void>
	{
	public:
		event_link add(typename event_callback_t::callback_t callback, event_placement placement = event_prepend)
		{
			std::size_t link = m_counter++;
			if (placement == event_prepend)
				m_callbacks.push_front(std::make_pair(link, event_callback_t(callback)));
			else
				m_callbacks.push_back(std::make_pair(link, event_callback_t(callback)));
			return link;
		}
	};

	// event bus class, holds list of events
	template <typename Args, typename Filter = void>
	class event_bus : public boost::unordered_map<std::string, event_info<Args, Filter> >
	{
	public:
		typedef boost::function<bool (const Args&)> callback_t;

		virtual ~event_bus() {}
	};
}

#endif
