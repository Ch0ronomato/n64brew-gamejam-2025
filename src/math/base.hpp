#pragma once

#include <math.h>

namespace jam
{
    /// Unsigned integer type used for indexing
    typedef unsigned int uint;

    // Floating point type used for common operations
    typedef float real;

    /// @brief Epsilon value for floating point comparisons
    constexpr real EPSILON = 1e-6f;

    /// @brief Approximate equality comparison for 'real' type
    inline bool approx_equal(real a, real b, real epsilon = EPSILON)
    {
        return abs(a - b) <= epsilon;
    }
}
