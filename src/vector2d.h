#pragma once
#include <cmath>
#include <string>
#include <type_traits>

template <class T>
struct V2D {
    static_assert(std::is_arithmetic<T>::value, "V2D<type> must be numeric");
    
    T x;
    T y;

    // Vector/Vector operations
    constexpr V2D operator+(const V2D& other) const {
        return V2D{x + other.x, y + other.y};
    }

    constexpr V2D operator-(const V2D& other) const {
        return V2D{x - other.x, y - other.y};
    }

    constexpr V2D operator*(const V2D& other) const {
        return V2D{x * other.x, y * other.y};
    }

    constexpr V2D operator/(const V2D& other) const {
        return V2D{x / other.x, y / other.y};
    }

    constexpr bool operator==(const V2D& other) const {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const V2D& other) const {
        return !(*this == other);
    }

    constexpr V2D operator-() const {
        return V2D{-x, -y};
    }

    constexpr V2D& operator=(const V2D& other) {
        x = other.x;
        y = other.y;
        return *this; //allows chaining
    }

    constexpr V2D& operator+=(const V2D& other) {
        return *this = *this + other;
    }

    constexpr V2D& operator-=(const V2D& other) {
        return *this = *this - other;
    }

    constexpr V2D& operator*=(const V2D& other) {
        return *this = *this * other;
    }

    constexpr V2D& operator/=(const V2D& other) {
        return *this = *this / other;
    }

    // Vector/Scalar operations
    constexpr V2D operator+(const T& other) const {
        return V2D{x + other, y + other};
    }
    
    constexpr V2D operator-(const T& other) const {
        return V2D{x - other, y - other};
    }

    constexpr V2D operator*(const T& other) const {
        return V2D{x * other, y * other};
    }

    constexpr V2D operator/(const T& other) const {
        return V2D{x / other, y / other};
    }

    //note that the assignment operators don't need to be symmetrical
    constexpr V2D operator+=(const T& other) {
        return *this = *this + other;
    }

    constexpr V2D operator-=(const T& other) {
        return *this = *this - other;
    }

    constexpr V2D operator*=(const T& other) {
        return *this = *this * other;
    }

    constexpr V2D operator/=(const T& other) {
        return *this = *this / other;
    }

    // Misc operations
    auto mag() const {
        // auto because I can't work out what type this is lol
        return std::sqrt(x * x + y * y);
    }

    V2D norm() const {
        return V2D{x / mag(), y / mag()};
    }

    V2D abs() const {
        return V2D{std::abs(x), std::abs(y)};
    }

    std::string str() const {
        return std::string("(") + std::to_string(x) + "," + std::to_string(y) + ")";
    }

    // Allow casting from other V2D types
    template <class F>
    constexpr operator V2D<F>() const {
        return {static_cast<F>(x), static_cast<F>(y)};
    }
};


// Vector/Scalar operations, the symmetric versions
template <class T>
constexpr V2D<T> operator+(const T& b, const V2D<T>& a) {return a + b;}

template <class T>
constexpr V2D<T> operator-(const T& b, const V2D<T>& a) {return a - b;}

template <class T>
constexpr V2D<T> operator*(const T& b, const V2D<T>& a) {return a * b;}

template <class T>
constexpr V2D<T> operator/(const T& b, const V2D<T>& a) {return a / b;}

// Handy typedefs that are ready to rock!
typedef V2D<int> Vi2D;
typedef V2D<float> Vf2D;
