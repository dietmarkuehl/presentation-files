#include "cf/pair.h"
#include "cf/string_view.h"
#include <array>
#include <type_traits>

template <typename P> using first_t  = typename P::first_type;
template <typename P> using second_t = typename P::second_type;

template <typename... T>
constexpr std::array<std::common_type_t<T...>, sizeof...(T)>
make_array0(T... members)
{
    return std::array<std::common_type_t<T...>, sizeof...(T)>{ members... };
}

template <typename... T>
constexpr std::array<cf::pair<first_t<std::common_type_t<T...>>,
                              second_t<std::common_type_t<T...>>>,
                     sizeof...(T)>
make_array1(T... members)
{
    return std::array<cf::pair<first_t<std::common_type_t<T...>>,
                               second_t<std::common_type_t<T...>>>,
                      sizeof...(T)>{ members... };
}

template <typename... T>
constexpr std::array<cf::pair<std::common_type_t<first_t<T>...>,
                              std::common_type_t<second_t<T>...>>,
                     sizeof...(T)>
make_array2(T... members)
{
    return std::array<cf::pair<std::common_type_t<first_t<T>...>,
                               std::common_type_t<second_t<T>...>>,
                      sizeof...(T)>{ members... };
}

template <typename... F, typename... S>
constexpr 
std::array<cf::pair<std::common_type_t<F...>,
                    std::common_type_t<S...>>, sizeof...(F)>
make_array3(cf::pair<F, S>... members)
{
    return std::array<cf::pair<std::common_type_t<F...>,
                               std::common_type_t<S...> >, sizeof...(F)>{ members... };
}

int main()
{
    using namespace cf::udl;
    constexpr auto s0 = make_array0(mp("2"_sv, 2), mp("3"_sv, 4), mp("foo"_sv, 1));
    constexpr auto s1 = make_array1(mp("2"_sv, 2), mp("3"_sv, 4), mp("foo"_sv, 1));
    constexpr auto s2 = make_array2(mp("2", 2), mp("3", 4), mp("foo"_sv, 1));
    constexpr auto t2 = make_array2(mp("2", 2), mp("3", 4), mp("foo"_sv, 1u));
    constexpr auto s3 = make_array3(mp("2", 2), mp("3", 4), mp("foo"_sv, 1));
    constexpr auto t3 = make_array3(mp("2", 2), mp("3", 4), mp("foo"_sv, 1u));
}
