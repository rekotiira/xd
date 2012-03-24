#ifndef H_XD_DETAIL_NOOP_DELETER
#define H_XD_DETAIL_NOOP_DELETER

namespace xd { namespace detail {

        template <typename T>
        struct noop_deleter
        {
                void operator()(T*& ptr)
                {
                }
        };

} }

#endif
