#if !BOOST_PP_IS_ITERATING
#	error "do not include this file directly"
#endif

#include <utility>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>

#define XD_N BOOST_PP_ITERATION()
#define XD_FORWARD(Z, N, D) std::forward<BOOST_PP_CAT(T, N)>(BOOST_PP_CAT(p, N))

template <typename R BOOST_PP_ENUM_TRAILING_PARAMS(XD_N, typename T)>
typename R::handle create(BOOST_PP_ENUM_BINARY_PARAMS(XD_N, T, && p))
{
	return xd::factory::create<R>(BOOST_PP_ENUM(XD_N, XD_FORWARD, _));
}

template <typename T BOOST_PP_ENUM_TRAILING_PARAMS(XD_N, typename T)>
typename T::handle load(BOOST_PP_ENUM_BINARY_PARAMS(XD_N, T, && p))
{
	// create serializer for asset type and get cache key
	asset_serializer<T> serializer;
	auto cache_key = serializer(BOOST_PP_ENUM(XD_N, XD_FORWARD, _));
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
	typename T::handle resource(new T(BOOST_PP_ENUM(XD_N, XD_FORWARD, _)));
	loaded_assets.insert(std::make_pair(cache_key, resource));
	return resource;
}

template <typename T BOOST_PP_ENUM_TRAILING_PARAMS(XD_N, typename T)>
typename T::handle load_persistent(BOOST_PP_ENUM_BINARY_PARAMS(XD_N, T, && p))
{
	// create serializer for asset type and get cache key
	asset_serializer<T> serializer;
	auto cache_key = serializer(BOOST_PP_ENUM(XD_N, XD_FORWARD, _));
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
	typename T::handle resource(new T(BOOST_PP_ENUM(XD_N, XD_FORWARD, _)));
	persistent_assets.insert(std::make_pair(cache_key, resource));
	return resource;
}

template <typename T BOOST_PP_ENUM_TRAILING_PARAMS(XD_N, typename T)>
void release(BOOST_PP_ENUM_BINARY_PARAMS(XD_N, T, && p))
{
	// create serializer for asset type and get cache key
	asset_serializer<T> serializer;
	auto cache_key = serializer(BOOST_PP_ENUM(XD_N, XD_FORWARD, _));

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

#undef XD_FORWARD
#undef XD_N
