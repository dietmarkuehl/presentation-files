#include <algorithm>
#include <array>
#include <iterator>
#include <iostream>
#include <type_traits>
#include <utility>
#include <cstddef>

namespace cf
{
    template <typename T>
    constexpr std::remove_reference_t<T>&& move(T&& ref) noexcept(true) {
        return static_cast<std::remove_reference_t<T>&&>(ref);
    }

    template <typename T, std::size_t Size>
    struct array {
        T values[Size];

        constexpr T*       begin()       { return this->values; }
        constexpr T*       end()         { return this->values + Size; }
        constexpr T const* begin() const { return this->values; }
        constexpr T const* end() const   { return this->values + Size; }

        constexpr T&       operator[](std::size_t index)       { return this->values[index]; }
        constexpr T const& operator[](std::size_t index) const { return this->values[index]; }
    };

    template <typename... T>
    constexpr cf::array<std::common_type_t<T...>, sizeof...(T)> make_array(T&&... values) {
        return cf::array<std::common_type_t<T...>, sizeof...(T)>{{ std::forward<T>(values)... }};
    }
    
    template <typename T>
    constexpr void swap(T& a, T& b)
        noexcept(noexcept(T(std::declval<T&&>()))
                 && noexcept(std::declval<T&>() = std::declval<T&&>()))
    {
        T tmp{std::move(a)};
        a = std::move(b);
        b = std::move(tmp);
    }
    
    template <typename RndIt>
    constexpr void insertion_sort(RndIt begin, RndIt end) {
        if (1 < end - begin) {
            for (auto it = begin + 1; it != end; ++it) {
                for (auto mit = it; begin != mit && mit[0] < mit[-1]; --mit) {
                    cf::swap(mit[-1], mit[0]);
                }
            }
        }
    }

    template <typename RndIt>
    constexpr auto distance(RndIt begin, RndIt end) {
        return end - begin;
    }
    template <typename RndIt, typename Value>
    constexpr RndIt partition(RndIt begin, RndIt end, Value const& pivot) {
        RndIt pbegin = begin, pend = end;
        while (true) {
            while (pbegin != pend && *pbegin < pivot)
                ++pbegin;
            while (pbegin != pend && !(*--pend < pivot));
            if (pbegin == pend) break;
            swap(*pbegin, *pend);
            ++pbegin;
        }
        return pbegin;
    }

    template <typename RndIt>
    constexpr void sort(RndIt begin, RndIt end) {
        if (distance(begin, end) <= 1) return;
        RndIt pivot = end - 1;
        RndIt mid = partition(begin, pivot, *pivot);
        swap(*mid, *pivot);
        sort(begin, mid);
        sort(mid + 1, end);
    }

    template <typename Range>
    constexpr Range sort(Range&& range) {
        sort(range.begin(), range.end());
        return range;
    }

#if 0
    template <typename RndIt>
    constexpr void sort(RndIt begin, RndIt end) {
        // cf::insertion_sort(begin, end);
        qsort(begin, end);
    }
    enum none {};
    constexpr cf::array<none, 0> sort_values() { return cf::array<none, 0>(); }
    template <typename... T>
    constexpr cf::array<std::common_type_t<T...>, sizeof...(T)> sort_values(T&&... value) {
        return cf::sort(cf::array<std::common_type_t<T...>, sizeof...(T)>{ value... });
    }
#else
    using std::common_type_t;
    enum none {};
    auto constexpr sort_values() { return cf::array<none, 0>(); }
    template <typename... T>
    auto constexpr sort_values(T&&... value) {
        using type = common_type_t<T...>;
        array<type, sizeof...(T)> array{{ value... }};
        sort(array.begin(), array.end());
        return array;
    }
#endif
}

template <typename A>
constexpr void print(char const* name, A&& array)
{
    std::cout << name << ": [";
    std::copy(array.begin(), array.end(), std::ostream_iterator<int>(std::cout, " "));
    std::cout << "]\n";
}

constexpr auto t0  = cf::sort_values(1, 9, 2, 8, 3, 7, 4, 6, 5 );
int main()
{
     print("nine", t0);
     enum { four = t0[4] };

     constexpr auto t1  = cf::sort(cf::make_array(1, 9, 2, 8, 3, 7, 4, 6, 5 ));
     print("nine(array)", t1);

     print("empty", cf::sort_values());
     print("one", cf::sort_values(1));
     print("two", cf::sort_values(2, 1));
}
