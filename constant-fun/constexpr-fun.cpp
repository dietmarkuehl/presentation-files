constexpr bool fun(bool value) {
    if (!value) {
        int* p = new int(17);
        delete p;
        throw "not!";
    }
    return value;
}

int main()
{
    bool b0 = fun(true);
}
