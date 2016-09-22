#include <algorithm>
#include <array>
#include <cstddef>

constexpr int add(std::array<int, 3> const& a)
{
    return a[0] + a[1] + a[2];
}

template <std::size_t Index, std::size_t Size>
struct accumulate_aux
{
    static constexpr int add(std::array<int, Size> const& a, int value)
    {
        return accumulate_aux<Index + 1, Size>::add(a, value + a[Index]);
    }
};

template <std::size_t Size>
struct accumulate_aux<Size, Size>
{
    static constexpr int add(std::array<int, Size> const&, int value)
    {
        return value;
    }
};

template <std::size_t Size>
constexpr int accumulate(std::array<int, Size> const& a)
{
#if __cplusplus < 201402L
    return accumulate_aux<0, Size>::add(a, 0);
#else
    int result(0);
    for (std::size_t i(0); i != Size; ++i) {
        result += a[i];
    }
    return result;
#endif
}

int main()
{
    constexpr std::array<int, 3> a{ 1, 2, 3 };
    constexpr int sum1 = a[0] + a[1] + a[2];
    constexpr int sum2 = add(a);
    constexpr int sum3 = accumulate(a);
    return sum1 != sum2 || sum2 != sum3;
}
