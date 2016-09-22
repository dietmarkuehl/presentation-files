constexpr int mode = 0;
constexpr int round(int value) {
    return value + (value % 2? mode: 0);
}

int main()
{
    constexpr int e = round(1);
    return e;
}
