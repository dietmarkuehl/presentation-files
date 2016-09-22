#include "cf/string_view.h"
#include <iostream>
#include <sstream>

// ----------------------------------------------------------------------------

int main()
{
    using namespace cf::udl;
    
    static_assert(cf::string_view() == ""_sv, "string_view() is equal to empty string ");
    static_assert(cf::string_view().empty(), "string_view() is empty()");
    static_assert(!cf::string_view("abc").empty(), "string_view(\"abc\") is not empty()");
    static_assert(0u == cf::string_view().size(), "string_view() size is 0");
    static_assert(3u == cf::string_view("abc").size(), "string_view(\"abc\") size is 3");

    constexpr cf::string_view s0("0123456789");
    static_assert(cf::string_view(s0, 3) == "3456789"_sv, "string_view(s0, 3)");
    static_assert(cf::string_view(s0, 3, 3) == "345"_sv, "string_view(s0, 3, 3)");
    static_assert(cf::string_view(s0, 3, 3).size() == 3u, "string_view(s0, 3, 3).size()");

    static_assert(cf::string_view("01234\06789", 7).size() == 7u, "string_view(01234\06789, 7).size()");

    constexpr cf::string_view str("0123456789");
    std::cout << "'" << cf::string_view(str.begin() + 3, str.begin() + 6) << "'\n";
    static_assert(cf::string_view(str.begin() + 3, str.begin() + 6) == "345"_sv, "string_view(str + 3, str + 6)");

    static_assert( ("abcde0"_sv == "abcde0"_sv), "abcde0 == abcde0");
    static_assert(!("abcde0"_sv == "abcde1"_sv), "abcde0 == abcde1");
    static_assert(!("abcde0"_sv != "abcde0"_sv), "abcde0 != abcde0");
    static_assert( ("abcde0"_sv != "abcde1"_sv), "abcde0 != abcde1");

    static_assert( ("abcde0"_sv <  "abcde1"_sv), "abcde0 <  abcde1");
    static_assert(!("abcde1"_sv <  "abcde1"_sv), "abcde1 <  abcde1");
    static_assert(!("abcde2"_sv <  "abcde1"_sv), "abcde2 <  abcde1");

    static_assert( ("abcde0"_sv <= "abcde1"_sv), "abcde0 <= abcde1");
    static_assert( ("abcde1"_sv <= "abcde1"_sv), "abcde1 <= abcde1");
    static_assert(!("abcde2"_sv <= "abcde1"_sv), "abcde2 <= abcde1");

    static_assert(!("abcde0"_sv >  "abcde1"_sv), "abcde0 >  abcde1");
    static_assert(!("abcde1"_sv >  "abcde1"_sv), "abcde1 >  abcde1");
    static_assert( ("abcde2"_sv >  "abcde1"_sv), "abcde2 >  abcde1");

    static_assert(!("abcde0"_sv >= "abcde1"_sv), "abcde0 >= abcde1");
    static_assert( ("abcde1"_sv >= "abcde1"_sv), "abcde1 >= abcde1");
    static_assert( ("abcde2"_sv >= "abcde1"_sv), "abcde2 >= abcde1");

    std::ostringstream out;
    std::ostream&      ro = out << "abcdef"_sv;
    return &ro == &out && out.str() == "abcdef"? EXIT_SUCCESS: EXIT_FAILURE;
}

