#pragma once
#ifndef VECTOR_HELPER_H
#define VECTOR_HELPER_H
//
    /// 2-dimensional vector template (T = int|float|double)
template <typename T> class Vec2 {
    public:
        enum { dim = 2 };
        typedef T value_type;

        Vec2() { }

        Vec2( T vx ) 
            : x(vx), y(vx){ }

        Vec2( T vx, T vy ) 
            : x(vx), y(vy){ }

        template <typename U> Vec2( const Vec2<U>& src )
            : x(static_cast<T>(src.x)), y(static_cast<T>(src.y)) { }

        template <typename U> explicit Vec2( const U *src ) 
            : x(static_cast<T>(src[0])), y(static_cast<T>(src[1])) { }

        T& operator[]( int index ) {
            return (&x)[index];
        }

        const T& operator[]( int index ) const {
            return (&x)[index];
        }

        template <typename U> void get( U *dst ) const {
            dst[0] = static_cast<U>(x);
            dst[1] = static_cast<U>(y);
        }

        bool operator==( const Vec2& v ) const {
            return (x == v.x) && (y == v.y);
        }

        bool operator!=( const Vec2& v ) const {
            return !this->operator==(v);
        }

        const Vec2& operator+=( const Vec2& v ) {
            x += v.x; 
            y += v.y; 
            return *this;
        }

        Vec2 operator+( const Vec2& v ) const {
            return Vec2(x + v.x, y + v.y);
        }

        const Vec2& operator-=( const Vec2& v ) {
            x -= v.x; 
            y -= v.y; 
            return *this;
        }

        Vec2 operator-( const Vec2& v ) const {
            return Vec2(x - v.x, y - v.y);
        }

        Vec2 operator-() const {
            return Vec2(-x, -y);
        }

        const Vec2& operator*=( T k ) {
            x *= k; 
            y *= k; 
            return *this;
        }

        Vec2<T> operator*( T k ) const {
            return Vec2<T>( x * k, y * k );
        }

        const Vec2& operator*=( const Vec2& v ) {
            x *= v.x; 
            y *= v.y; 
            return *this;
        }

        Vec2 operator*( const Vec2& v ) const {
            return Vec2( x * v.x, y * v.y );
        }

        const Vec2& operator/=( T k ) {
            x /= k; 
            y /= k; 
            return *this;
        }

        Vec2<T> operator/( T k ) const {
            return Vec2<T>( x / k, y / k );
        }

        const Vec2& operator/=( const Vec2& v ) {
            x /= v.x; 
            y /= v.y; 
            return *this;
        }

        Vec2 operator/( const Vec2& v ) const {
            return Vec2( x / v.x, y / v.y );
        }

        T x;              
        T y;            
};

template <typename T> Vec2<T> operator*(T k, const Vec2<T>& v) {
    return Vec2<T>(v.x * k, v.y * k);
}

template <typename T> T dot(const Vec2<T>& v1, const Vec2<T>& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

template <typename T> T length(const Vec2<T>& v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

template <typename T> T distance(const Vec2<T>& a, const Vec2<T>& b) {
    return length(a - b);
}

template <typename T> Vec2<T> normalize(const Vec2<T>& v) {
    return v / length<T>(v);
}

/*
   template<typename T> std::ostream& operator<<(std::ostream& os, const vec2<T>& v) {
   return (os << v.x << " " << v.y);
   }

   template<typename T> std::istream& operator>>(std::istream& is, vec2<T>& v) {
   return is >> v.x >> v.y;
   }
   */

typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;
typedef Vec2<float> Point2f;
typedef Vec2<double> Vec2d;
typedef Vec2<double> Point2d;
#endif
