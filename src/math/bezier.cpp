#include "math/bezier.hpp"
#include "math/interpolate.hpp"

using namespace jam;


Point Bezier::get_point(uint seg_idx, real t) const
{
    // Figure out where to read control points from
    const uint   idx = seg_idx * 3;
    const Vec3 * ptr = control_points.raw();

    // precompute powers
    const real 
        i  = 1.f - t, 
        t2 = t   * t, 
        t3 = t2  * t,
        i2 = i   * i,
        i3 = i2  * i;

    // Fetch control points
    const Vec3 
        & p0 = ptr[idx    ],
        & p1 = ptr[idx + 1],
        & p2 = ptr[idx + 2],
        & p3 = ptr[idx + 3];

    // Fetch segment data
    const SegmentData
        & s0 = segments_data[seg_idx    ],
        & s1 = segments_data[seg_idx + 1];

    // Compute the interpolated point
    // We reimplement the bezier_cubic here to avoid extra copies
    Vec3 pos;
    for (uint i = 0; i < 3; ++i)
    {
        pos.coords[i] =
            p0.coords[i] * (      i3    ) +
            p1.coords[i] * (3.f * i2 * t) +
            p2.coords[i] * (3.f * t2 * i) +
            p3.coords[i] * (      t3    );
    }
    Vec3 normal = Vec3::lerp(s0.normal, s1.normal, t);
    real width  = jam ::lerp(s0.width , s1.width , t);

    return Point(pos, normal, width);
}