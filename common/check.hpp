// Tiny test helper shared by every lesson. You don't need to understand it yet;
// by Lesson 9 (templates/macros) you will, and you could write it yourself.
#pragma once
#include <iostream>
#include <string_view>

namespace lab {

inline int g_pass = 0;
inline int g_fail = 0;

inline void check(bool ok, std::string_view expr, std::string_view file, int line) {
    if (ok) {
        ++g_pass;
    } else {
        ++g_fail;
        file = file.substr(file.find_last_of("/\\") + 1);  // just the filename, not the full path
        std::cout << "FAIL  " << file << "(" << line << "): " << expr << "\n";
    }
}

inline void section(std::string_view name) { std::cout << "-- " << name << "\n"; }

// Call at the end of main(): prints the score and returns the process exit code.
inline int report() {
    std::cout << "\n" << g_pass << "/" << (g_pass + g_fail) << " checks passed\n";
    return g_fail == 0 ? 0 : 1;
}

}  // namespace lab

// #expr turns the expression's source text into a string, so failures show what was tested.
#define CHECK(expr) ::lab::check(static_cast<bool>(expr), #expr, __FILE__, __LINE__)
