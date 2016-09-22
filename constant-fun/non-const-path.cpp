#include <iostream>
#include <stdexcept>

inline int f(int value)
{
    return value;
}

inline constexpr int g(int value)
{
    return value;
}

constexpr int call_non_constexpr(int value)
{
    return value % 2? f(value): g(value);
}

constexpr int return_even(int value)
{
    return value % 2? throw std::runtime_error("value isn't even"): value;
}

int main(int ac, char*[])
{
    try {
        constexpr int two = call_non_constexpr(2);
        constexpr int four = return_even(4);
        int five = return_even(5);
        return return_even(two + four + ac);
    }
    catch (std::exception const& ex) {
        std::cout << "ERROR: " << ex.what() << '\n';
    }
}
