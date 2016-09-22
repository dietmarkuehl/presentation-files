struct foo { int i; };
constexpr bool operator== (foo f1, foo f2) {
    return f1.i == f2.i;
}

inline bool compare(foo f1, foo f2) {
    return f1 == f2? 17: 42;
}

int main(int ac, char*[])
{
    (void)ac;
    return compare(foo{ac}, foo{2});
}
