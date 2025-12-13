#pragma once

#include "math/base.hpp"

namespace jam
{
    union Vec2
    {
    public:
        // MARK: Members

        // Access as individual components or as an array
        struct
        {
            real x;
            real y;
        };
        real coords[2];

        /// @brief Create a zero-initialized vector
        inline Vec2(): x(0), y(0) {}

        /// @brief Create a vector with specified components
        inline Vec2(real x_, real y_): x(x_), y(y_) {}

        /// @brief Copy constructor
        inline Vec2(const Vec2 & copy): x(copy.x), y(copy.y) {}

        /// @brief Copy operator
        inline Vec2 operator=(const Vec2 & copy)
        {
            x = copy.x;
            y = copy.y;
            return *this;
        }

        /// @brief Default destructor
        ~Vec2() = default;


        // MARK: Operators

        /// @brief Negate the vector
        inline Vec2 operator-() const
        {
            return Vec2(-x, -y);
        }

        /// @brief Addition operator
        inline Vec2 operator+(const Vec2 & other) const
        {
            return Vec2(x + other.x, y + other.y);
        }

        /// @brief Subtraction operator
        inline Vec2 operator-(const Vec2 & other) const
        {
            return Vec2(x - other.x, y - other.y);
        }

        /// @brief Scalar multiplication operator
        inline Vec2 operator*(real scalar) const
        {
            return Vec2(x * scalar, y * scalar);
        }

        /// @brief Scalar division operator
        inline Vec2 operator/(real scalar) const
        {
            const real s = 1.0f / scalar;
            return Vec2(x * s, y * s);
        }

        /// @brief Addition assignment operator
        inline Vec2 & operator+=(const Vec2 & other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        /// @brief Subtraction assignment operator
        inline Vec2 & operator-=(const Vec2 & other)
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        /// @brief Scalar multiplication assignment operator
        inline Vec2 & operator*=(real scalar)
        {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        /// @brief Scalar division assignment operator
        inline Vec2 & operator/=(real scalar)
        {
            const real s = 1.0f / scalar;
            x *= s;
            y *= s;
            return *this;
        }

        /// @brief Cross product operator
        inline real cross(const Vec2 & other) const
        {
            return x * other.y + y * other.x;
        }

        /// @brief Dot product operator
        inline real dot(const Vec2 & other) const
        {
            return x * other.x + y * other.y;
        }

        /// @brief Check if the vector is zero
        inline bool is_zero() const { return x == 0.0f and y == 0.0f; }

        /// @brief Access element by index
        inline real operator[](uint index) const { return coords[index]; }

        /// @brief Access element by index
        inline real & operator[](uint index) { return coords[index]; }


        // MARK: Methods

        /// @brief Squared magnitude of the vector
        inline real mag_sqr() const { return x * x + y * y; }

        /// @brief Magnitude of the vector
        inline real mag() const { return sqrt(mag_sqr()); }

        /// @brief Normalize the vector
        inline Vec2 & normalize()
        {
            const real magsqr = mag_sqr();
            if (magsqr > 0.0f)
            {
                const real s = 1.0f / sqrt(magsqr);
                x *= s;
                y *= s;
            }
            return *this;
        }

        /// @brief Normalize the vector
        inline Vec2 normal()
        {
            Vec2 n = *this;
            n.normalize();
            return n;
        }

        /// @brief Return the projection of this vector onto another vector
        /// @note Resulting vector will be colinear to 'other'
        inline Vec2 projected_onto(const Vec2 & other)
        {
            return other * (dot(other) / other.mag_sqr());
        }

        /// @brief Return the rejection of this vector from another vector
        /// @note Resulting vector will be orthogonal to 'other'
        inline Vec2 rejected_from(const Vec2 & other)
        {
            return *this - projected_onto(other);
        }

        /// @brief Reflect this vector around a normal vector
        inline Vec2 reflected(const Vec2 & normal) const
        {
            return *this - normal * (2.0f * this->dot(normal));
        }


        // MARK: Comparison

        /// Check if the two vectors are approximately equal
        static inline bool approx_equal(const Vec2 & a, const Vec2 & b, real epsilon = EPSILON)
        {
            return jam::approx_equal(a.x, b.x, epsilon) and
                   jam::approx_equal(a.y, b.y, epsilon);
        }

        /// @brief Equality operator
        inline bool operator==(const Vec2 & other) const
        {
            return approx_equal(*this, other);
        }

        /// @brief Inequality operator
        inline bool operator!=(const Vec2 & other) const
        {
            return not approx_equal(*this, other);
        }

        /// @brief Compute squared distance between two vectors
        static inline real distance_squared(const Vec2 & a, const Vec2 & b)
        {
            return (a - b).mag_sqr();
        }

        /// @brief Compute squared distance between two vectors
        static inline real distance(const Vec2 & a, const Vec2 & b)
        {
            return (a - b).mag();
        }

        /// @brief Compute the angle between two vectors in radians
        static inline real angle(const Vec2 & a, const Vec2 & b)
        {
            return atan2(a.cross(b), a.dot(b));
        }

        /// @brief Check if the angle between two vectors is acute
        static inline bool acute(const Vec2 & a, const Vec2 & b)
        {
            return a.dot(b) > 0.0f;
        }

        /// @brief Check if the angle between two vectors is orthogonal
        static inline bool orthogonal(const Vec2 & a, const Vec2 & b)
        {
            return abs(a.dot(b)) <= EPSILON;
        }

        /// @brief Check if the angle between two vectors is obtuse
        static inline bool obtuse(const Vec2 & a, const Vec2 & b)
        {
            return a.dot(b) < 0.0f;
        }
    };
}