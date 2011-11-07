#ifndef H_XD_COMMON_ENTITY
#define H_XD_COMMON_ENTITY

#include <boost/any.hpp>
#include <unordered_map>
#include <functional>
#include <memory>
#include <list>
#include <xd/common/event_bus.h>

namespace xd {
    
	namespace detail
	{
        template <typename T>
        struct component_base
        {
            virtual ~component_base() {}
            virtual void init(T&) {}
        };

        template <typename T>
		struct logic_component : virtual component_base<T>
		{
			virtual void update(T&) = 0;
		};
        
        template <typename T>
		struct render_component : virtual component_base<T>
		{
			virtual void render(T&) = 0;
		};
        
        template <typename T>
		struct component : logic_component<T>, render_component<T>
		{
		};
	}
    
    template <typename T>
	struct logic_component : detail::logic_component<T>
	{
		typedef std::shared_ptr<detail::logic_component<T>> ptr;
	};

    template <typename T>
	struct render_component : detail::render_component<T>
	{
		typedef std::shared_ptr<detail::render_component<T>> ptr;
	};
    
    template <typename T>
	struct component : detail::component<T>
	{
		typedef std::shared_ptr<detail::component<T>> ptr;
	};
    
	template <typename Base>
	class entity : public Base
	{
	public:
		typedef std::shared_ptr<entity> ptr;
        
		template <typename T>
		void on(const std::string& name, std::function<bool (const T&)> callback)
		{
			get_event_bus<T>()[name].add(callback);
		}
        
		template <typename T>
		void on(const std::string& name, std::function<bool (const T&)> callback, std::function<bool (const T&)> filter)
		{
			get_event_bus<T>()[name].add(callback, filter);
		}
        
		template <typename T>
		void trigger(const std::string& name, const T& args)
		{
            get_event_bus<T>()[name](args);
		}
        
		void add_component(const typename logic_component<entity<Base>>::ptr& component)
		{
			m_logic_components.push_back(component);
			component->init(*this);
		}
        
		void add_component(const typename render_component<entity<Base>>::ptr& component)
		{
			m_render_components.push_back(component);
			component->init(*this);
		}
        
		void add_component(const typename component<entity<Base>>::ptr& component)
		{
			m_logic_components.insert(component);
			m_render_components.insert(component);
			component->init(*this);
		}
        
		void update()
		{
			for (auto i = m_logic_components.begin(); i != m_logic_components.end(); ++i) {
				(*i)->update(*this);
			}
		}

		void render()
		{
			for (auto i = m_render_components.begin(); i != m_render_components.end(); ++i) {
				(*i)->render(*this);
			}
		}
        
	private:
		// we have logic and render components
        // TODO!! add priorities
		std::list<typename logic_component<entity<Base>>::ptr> m_logic_components;
		std::list<typename render_component<entity<Base>>::ptr> m_render_components;
        
		// the bound events
		std::unordered_map<std::size_t, boost::any> m_events;
        
        // utility function to return event_bus for given arg type
        template <typename T>
        event_bus<T>& get_event_bus()
        {
			// calculate hash of the argument type
			std::size_t hash = typeid(T).hash_code();
            // find from the events
            auto i = m_events.find(hash);
            if (i == m_events.end()) {
                std::cout << "notfound" << std::endl;
                // not found, insert an empty event_bus
                m_events[hash] = event_bus<T>();
            }
			// return the event bus
            return *boost::any_cast<event_bus<T>>(&m_events[hash]);
        }
	};
}

#endif
