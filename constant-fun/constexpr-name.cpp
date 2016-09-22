#include <initializer_list>
#include <cstddef>

using namespace std;

template <typename T>
constexpr size_t size(initializer_list<T> init) {
    /*constexpr*/ size_t s = init.size();            // not OK
    return s;
}
int main() {
    constexpr size_t s = size({ 1, 2, 3 });    // OK
}
