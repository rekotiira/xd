#ifndef H_XD_COMMON_ENTITY
#define H_XD_COMMON_ENTITY

#include <boost/any.hpp>
#include <unordered_map>
#include <functional>
#include <memory>
#include <list>
#include <map>
#include <type_traits>
#include <xd/common/ref_counted.h>
#include <xd/common/event_bus.h>

namespace xd {
    
	// forward declare entity so components can declare it as friend
	template <typename> class entity;

	namespace detail
	{
        template <typename T>
        class component_base : public xd::ref_counted
        {
		public:
            virtual ~component_base() {}
		private:
			template <typename> friend class entity;
            virtual void init(T&) {}
        };

        template <typename T>
		class logic_component : public virtual component_base<T>
		{
		private:
			template <typename> friend class entity;
			virtual void update(T&) = 0;
		};
        
        template <typename T>
		class render_component : public virtual component_base<T>
		{
		private:
			template <typename> friend class entity;
			virtual void render(T&) = 0;
		};
        
        template <typename T>
		class component : public logic_component<T>, public render_component<T>
		{
		};

		/*template <typename C, typename T>
		T first_argument_helper(bool (C::*callback)(const T&));
		
		template <typename C, typename T>
		T first_argument_helper(bool (C::*callback)(const T&) const);

		template <typename C>
		struct first_argument
		{
			typedef decltype(first_argument_helper(&C::operator())) type;
		};*/

		// used to disable template argument deduction
		template <typename T>
		struct identity
		{
			typedef T type;
		};
	}
    
    template <typename T>
	struct logic_component : detail::logic_component<T>
	{
		typedef boost::intrusive_ptr<detail::logic_component<T>> ptr;
	};

    template <typename T>
	struct render_component : detail::render_component<T>
	{
		typedef boost::intrusive_ptr<detail::render_component<T>> ptr;
	};
    
    template <typename T>
	struct component : detail::component<T>
	{
		typedef boost::intrusive_ptr<detail::component<T>> ptr;
	};
	
	class entity_base
	{
	protected:
		~entity_base() {}
	};
    
	template <typename Base = entity_base>
	class entity : public Base, public xd::ref_counted
	{
	public:
		// required for xd::factory
		typedef boost::intrusive_ptr<entity> ptr;
		// component ptr typedefs
		typedef typename component<entity<Base>>::ptr component_ptr;
		typedef typename logic_component<entity<Base>>::ptr logic_component_ptr;
		typedef typename render_component<entity<Base>>::ptr render_component_ptr;

		entity()
		{
		}

		entity(const Base& base)
			: Base(base)
		{
		}

		virtual ~entity()
		{
		}

		template <typename T>
		T& get()
		{
			std::size_t hash = typeid(T).hash_code();
			auto i = m_type_to_data.find(hash);
			if (i == m_type_to_data.end())
				i = m_type_to_data.insert(std::make_pair(hash, T())).first;
			return *boost::any_cast<T>(&i->second);
		}

		template <typename T>
		T& get(const std::string& key)
		{
			auto i = m_key_to_data.find(key);
			if (i == m_key_to_data.end())
				i = m_key_to_data.insert(std::make_pair(key, T())).first;
			return *boost::any_cast<T>(&i->second);
		}

		template <typename T>
		bool has()
		{
			std::size_t hash = typeid(T).hash_code();
			return m_key_to_data.find(hash) != m_type_to_data.end();
		}

		template <typename T>
		bool has(const std::string& key)
		{
			return m_key_to_data.find(key) != m_key_to_data.end();
		}

		template <typename T>
		void on(const std::string& name, std::function<bool (const T&)> callback, std::function<bool (const T&)> filter = nullptr)
		{
			get_event_bus<T>()[name].add(callback);
		}

		template <typename T>
		void on(const std::string& name, bool (*callback)(const T&), std::function<bool (const typename detail::identity<T>::type&)> filter = nullptr)
		{
			get_event_bus<T>()[name].add(callback);
		}

		template <typename T, typename C>
		void on(const std::string& name, bool (C::*callback)(const T&), C *obj, std::function<bool (const typename detail::identity<T>::type&)> filter = nullptr)
		{
			get_event_bus<T>()[name].add(std::bind(callback, obj, std::placeholders::_1));
		}

		template <typename T, typename C>
		void on(const std::string& name, bool (C::*callback)(const T&) const, C *obj, std::function<bool (const typename detail::identity<T>::type&)> filter = nullptr)
		{
			get_event_bus<T>()[name].add(std::bind(callback, obj, std::placeholders::_1));
		}

		template <typename T>
		void on(const std::string& name, T obj)
		{
			on(name, &T::operator(), &obj);
			//get_event_bus<typename detail::first_argument<T>::type>()[name].add(obj);
		}

		template <typename T, typename U>
		void on(const std::string& name, T obj, U filter)
		{
			on(name, &T::operator(), &obj, filter);
			//get_event_bus<typename detail::first_argument<T>::type>()[name].add(obj, filter);
		}
        
		template <typename T>
		void trigger(const std::string& name, const T& args)
		{
            get_event_bus<T>()[name](args);
		}
        
		void add_component(const logic_component_ptr& component, int priority = 0)
		{
			m_components[priority].logic_components.push_back(component);
			component->init(*this);
		}
        
		void add_component(const render_component_ptr& component, int priority = 0)
		{
			m_components[priority].render_components.push_back(component);
			component->init(*this);
		}
        
		void add_component(const component_ptr& component, int priority = 0)
		{
			m_components[priority].logic_components.push_back(component);
			m_components[priority].render_components.push_back(component);
			component->init(*this);
		}

		void del_component(const logic_component_ptr& component, int priority)
		{
			logic_component_list_t& components = m_components[priority].logic_components;
			auto i = components.find(component);
			if (i != components.end()) {
				components.erase(i);
			}
		}

		void del_component(const logic_component_ptr& component)
		{
			for (auto i = m_components.begin(); i != m_components.end(); ++i) {
				del_component(component, i->first);
			}
		}

		void del_component(const render_component_ptr& component, int priority)
		{
			logic_component_list_t& components = m_components[priority].logic_components;
			auto i = components.find(component);
			if (i != components.end()) {
				components.erase(i);
			}
		}

		void del_component(const render_component_ptr& component)
		{
			for (auto i = m_components.begin(); i != m_components.end(); ++i) {
				del_component(component, i->first);
			}
		}

		void del_component(const component_ptr& component, int priority)
		{
			components_set& components = m_components[priority];
			{
				auto i = components.logic_components.find(component);
				if (i != components.end()) {
					components.erase(i);
				}
			}
			{
				auto i = components.render_components.find(component);
				if (i != components.end()) {
					components.erase(i);
				}
			}
		}

		void del_component(const component_ptr& component)
		{
			for (auto i = m_components.begin(); i != m_components.end(); ++i) {
				del_component(component, i->first);
			}
		}

		void clear_components()
		{
			m_components.clear();
		}
        
		void update()
		{
			for (auto i = m_components.begin(); i != m_components.end(); ++i) {
				for (auto j = i->second.logic_components.begin(); j != i->second.logic_components.end(); ++j) {
					(*j)->update(*this);
				}
			}
		}

		void render()
		{
			for (auto i = m_components.begin(); i != m_components.end(); ++i) {
				for (auto j = i->second.render_components.begin(); j != i->second.render_components.end(); ++j) {
					(*j)->render(*this);
				}
			}
		}
        
	private:
		typedef std::list<logic_component_ptr> logic_component_list_t;
		typedef std::list<render_component_ptr> render_component_list_t;

		// data
		std::unordered_map<std::size_t, boost::any> m_type_to_data;
		std::unordered_map<std::string, boost::any> m_key_to_data;

		// internal struct to hold component lists per priority
		struct components_set
		{
			logic_component_list_t logic_components;
			render_component_list_t render_components;
		};

		// we have a list of logic and render components per priority
		std::map<int, components_set> m_components;
        
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
                // not found, insert an empty event_bus
                m_events[hash] = event_bus<T>();
            }
			// return the event bus
            return *boost::any_cast<event_bus<T>>(&m_events[hash]);
        }
	};
}

#endif
