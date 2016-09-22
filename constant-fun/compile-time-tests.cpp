#include <iterator>
#include <iostream>
#include <cstddef>

namespace cfun
{
    template <typename FwdIt, typename Value>
    constexpr FwdIt find(FwdIt it, FwdIt end, Value value) {
#if 1
        while (it != end && bool(value != *it)) ++it;
#else
        while (bool(value != *it) && it != end) ++it;
#endif
        return it;
    }

    constexpr bool test_find() {
        int array[] = { 1, 2, 3 };
        return array + 0 == find(array, array + 0, 2)
            && array + 0 == find(array, array + 3, 1)
            && array + 1 == find(array, array + 3, 2)
            && array + 2 == find(array, array + 3, 3)
            && array + 3 == find(array, array + 3, 4)
            ;
    }

    static_assert(cfun::test_find(), "cfun::find() failed");
}

namespace dfund {
template <typename FwdIt, typename Value>
constexpr FwdIt find(FwdIt it, FwdIt end, Value value) {
    while (it != end && bool(value != *it)) ++it;
    //while (bool(value != *it) && it != end) ++it;
    return it;
}
constexpr bool test_find() {
    int a[] = { 1 }; return a+1 == find(a, a+1, 4);
}
static_assert(test_find(), "find failed");
}

int main() {
    std::cout << "find test: " << (cfun::test_find()? "pass": "fail") << "\n";
}
