constexpr int    x = 10;
constexpr double d = 3.1415;

constexpr int fx();
int fz();
constexpr int fy() { int rx = fx(); return rx; }
constexpr int fx() { return x; }

int main() {
    constexpr int mx = fy();
    return d;
}

