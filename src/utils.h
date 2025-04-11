#include <cmath>
#include <type_traits>

template <class T>
constexpr T round_away(const T& x) {
    static_assert(std::is_arithmetic<T>::value, "round_away argument must be number");
    return x < 0 ? std::floor(x) : std::ceil(x);
}
