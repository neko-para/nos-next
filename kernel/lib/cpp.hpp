#pragma once

#define __DECLARE_THROW(name) \
    namespace std             \
    {                         \
    inline void name(auto...) \
    {                         \
    }                         \
    }

__DECLARE_THROW(__throw_bad_array_new_length)
__DECLARE_THROW(__throw_bad_alloc)
__DECLARE_THROW(__throw_length_error)

#define _REQUIRES_FREESTANDING_H

// #include <algorithm> // IWYU pragma: export
#include <list>   // IWYU pragma: export
#include <vector> // IWYU pragma: export
