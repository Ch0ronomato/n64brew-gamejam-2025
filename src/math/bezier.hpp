#pragma once

#include <math.h>
#include "math/vec3.hpp"
#include "collection.hpp"

namespace jam
{
    class Bezier;

    /// @brief Define data specific to a Bezier curve segment point
    struct SegmentData
    {
    public:
        /// @brief Normal at the control point
        Vec3 normal;

        /// @brief Width at the control point
        real width;

        /// @brief Default constructor
        inline SegmentData(): normal(0.f, 1.f, 0.f), width(1.f)
        {}

        /// @brief Default constructor
        inline SegmentData(const Vec3 & normal_, real width_):
            normal(normal_), width(width_)
        {}

        /// @brief Move constructor
        inline SegmentData(Vec3 && normal_, real width_):
            normal(normal_), width(width_)
        {}

        /// @brief Default destructor
        inline ~SegmentData() = default;
    };


    /// @brief Define a point structure for Bezier curve points
    struct Point
    {
        friend class Bezier;

    public:
        /// @brief Position of the point in 3D space
        Vec3 position;

        /// @brief Extra data associated with the point
        SegmentData data;

        /// @brief Default constructor
        inline Point(const Vec3 & pos, const Vec3 & normal, real width):
            position(pos), data(normal, width)
        {}

    protected:
        /// @brief Default constructor
        inline Point() {}
    };


    /// @brief Define a Bezier curve utility class
    class Bezier
    {
    protected:
        /// @brief List of control points defining the Bezier curve
        /// The control points are stored in a flat list as a chain of cubic 
        /// Bezier segments. The last point of each segment is the first point 
        /// of the next segment. And the last control point is a duplicate of 
        /// the first to allow proper looping.
        List<Vec3> control_points;

        /// @brief List of segment data associated with each control point
        List<SegmentData> segments_data;


    public:
        /// @brief Create a Bezier curve by specifying the number of segments
        /// @param segments_ Number of segments to allocate
        inline Bezier(uint segments_):
            control_points(segments_ * 3 + 1),
            segments_data(segments_ + 1)
        {}

        /// @brief Default destructor
        inline ~Bezier() = default;

        /// @brief Get the number of segments in the Bezier curve
        inline uint segment_count() const { return segments_data.len() - 1; }

        /// @brief Get an interpolated point on the Bezier curve
        /// @param segment Index of the segment to sample from
        /// @param weight Weight along the segment in [0, 1]
        /// @return Interpolated point at the specified segment and weight
        Point get_point(uint segment, real weight) const;
    };
}
