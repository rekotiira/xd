#ifndef H_XD_DETAIL_IDENTITY
#define H_XD_DETAIL_IDENTITY

namespace xd { namespace detail {

	// used to disable template argument deduction
	template <typename T>
	struct identity
	{
		typedef T type;
	};

} }

#endif
