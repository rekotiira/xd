#ifndef H_XD_DETAIL_RESOURCE_HANDLE
#define H_XD_DETAIL_RESOURCE_HANDLE

namespace xd { namespace detail {
        template <typename T>
        struct resource_handle_default_deleter
        {
                void operator()(T*& ptr)
                {
                        delete ptr;
                }
        };

        template <typename T>
        struct resource_handle_noop_deleter
        {
                void operator()(T*& ptr)
                {
                }
        };
} }

#endif
