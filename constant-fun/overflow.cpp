#include <iostream>
#include <limits>

template <int> struct aux {};

constexpr int add(int value0, int value1)
{
    int result = value0 + value1;
    return result;
}

int main()
{
    try {
        constexpr int result = add(2, 4);
        constexpr int int_max = std::numeric_limits<int>::max() - 2;
        // constexpr int excess = add(int_max, 1);
        //std::cout << "int_max=" << int_max << ' '
        //          << "add(int_max, 1)=" << add(int_max, 1) << ' '
        //          << '\n';
        return result? 0: 1;
    }
    catch (std::exception const& ex) {
        std::cout << "ERROR: " << ex.what() << '\n';
    }
}
