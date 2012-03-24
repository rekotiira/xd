#ifndef H_XD_ASSET_MANAGER
#define H_XD_ASSET_MANAGER

#include <xd/handle.hpp>
#include <xd/weak_handle.hpp>
#include <xd/asset_serializer.hpp>
#include <boost/any.hpp>
#include <unordered_map>
#include <map>
#include <cstddef>

#ifdef BOOST_NO_VARIADIC_TEMPLATES
#include <boost/preprocessor/iteration/iterate.hpp>
#endif

namespace xd
{
	class asset_manager
	{
	public:
#ifndef BOOST_NO_VARIADIC_TEMPLATES
		template <typename T, typename... Args>
		typename T::handle load(Args&&... args)
		{
			// create serializer for asset type and get cache key
			asset_serializer<T> serializer;
			auto cache_key = serializer(std::forward<Args>(args)...);
			// check if it's in the persistent map
			auto& persistent_assets = get_persistent_asset_map<T>();
			auto it = persistent_assets.find(cache_key);
			if (it != persistent_assets.end())
				return it->second;
			// otherwise check if it exists in the non-persistent map
			auto& loaded_assets = get_asset_map<T>();
			auto it2 = loaded_assets.find(cache_key);
			if (it2 != loaded_assets.end()) {
				// make sure it has not expired
				if (auto handle = it2->second.lock())
					return handle;
				// it has expired, remove it
				loaded_assets.erase(it2);
			}
			// not loaded in either map, create it
			typename T::handle resource(new T(std::forward<Args>(args)...));
			loaded_assets.insert(std::make_pair(cache_key, resource));
			return resource;
		}

		template <typename T, typename... Args>
		typename T::handle load_persistent(Args&&... args)
		{
			// create serializer for asset type and get cache key
			asset_serializer<T> serializer;
			auto cache_key = serializer(std::forward<Args>(args)...);
			// check if it's in the persistent map
			auto& persistent_assets = get_persistent_asset_map<T>();
			auto it = persistent_assets.find(cache_key);
			if (it != persistent_assets.end())
				return it->second;
			// otherwise check if it exists in the non-persistent map
			auto& loaded_assets = get_asset_map<T>();
			auto it2 = loaded_assets.find(cache_key);
			if (it2 != loaded_assets.end()) {
				// make sure it has not expired
				if (auto handle = it2->second.lock()) {
					// push it to the persistent asset map and remove from non-persistent map
					persistent_assets.insert(std::make_pair(cache_key, handle));
					loaded_assets.erase(it2);
					return handle;
				}
				// it has expired, remove it
				loaded_assets.erase(it2);
			}
			// not loaded in either map, create it
			typename T::handle resource(new T(std::forward<Args>(args)...));
			persistent_assets.insert(std::make_pair(cache_key, resource));
			return resource;
		}

		template <typename T, typename... Args>
		void release(Args&&... args)
		{
			// create serializer for asset type and get cache key
			asset_serializer<T> serializer;
			auto cache_key = serializer(std::forward<Args>(args)...);

			// check if it's in the persistent map
			auto& persistent_assets = get_persistent_asset_map<T>();
			auto it = persistent_assets.find(cache_key);
			if (it != persistent_assets.end())
				persistent_assets.erase(it);

			// check if it's in the non-persistent map
			auto& loaded_assets = get_asset_map<T>();
			auto it2 = loaded_assets.find(cache_key);
			if (it2 != loaded_assets.end())
				loaded_assets.erase(it2);
		}
#else
		// generate xd::entity::entity overloads with file iteration (up to XD_MAX_ARITY parameters)
		#define BOOST_PP_ITERATION_PARAMS_1 (3, (1, XD_MAX_ARITY, <xd/detail/iterate_asset_manager.hpp>))
		#include BOOST_PP_ITERATE()
#endif

		template <typename T>
		void release(const handle<T>& resource)
		{
			// get the persistent map
			auto& persistent_assets = get_persistent_asset_map<T>();
			// iterate through each loaded asset
			for (auto it = persistent_assets.begin(); it != persistent_assets.end(); ++it) {
				// check whether to remove this resource
				if (resource.get() == it->second.get()) {
					persistent_assets.erase(it);
					break;
				}
			}

			// get the non-persistent map
			auto& loaded_assets = get_asset_map<T>();
			// iterate through each loaded asset
			for (auto it2 = loaded_assets.begin(); it2 != loaded_assets.end(); ++it2) {
				// check whether to remove this resource
				if (auto handle = it2->second.lock()) {
					if (resource.get() == handle.get()) {
						loaded_assets.erase(it2);
						break;
					}
				}
			}
		}

	private:
		std::map<std::size_t, boost::any> m_asset_type_map;
		std::map<std::size_t, boost::any> m_persistent_asset_type_map;

		template <typename T>
		std::unordered_map<typename asset_serializer<T>::key_type, typename T::weak_handle>& get_asset_map()
		{
			typedef std::unordered_map<typename asset_serializer<T>::key_type, typename T::weak_handle> asset_map_type;
			std::size_t hash = typeid(T).hash_code();
			auto it = m_asset_type_map.find(hash);
			if (it == m_asset_type_map.end())
				m_asset_type_map.insert(std::make_pair(hash, asset_map_type()));
			return *boost::any_cast<asset_map_type>(&m_asset_type_map[hash]);
		}

		template <typename T>
		std::unordered_map<typename asset_serializer<T>::key_type, typename T::handle>& get_persistent_asset_map()
		{
			typedef std::unordered_map<typename asset_serializer<T>::key_type, typename T::handle> asset_map_type;
			std::size_t hash = typeid(T).hash_code();
			auto it = m_persistent_asset_type_map.find(hash);
			if (it == m_persistent_asset_type_map.end())
				m_persistent_asset_type_map.insert(std::make_pair(hash, asset_map_type()));
			return *boost::any_cast<asset_map_type>(&m_persistent_asset_type_map[hash]);
		}
	};
}

#endif
