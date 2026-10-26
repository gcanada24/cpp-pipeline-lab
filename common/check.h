// Tiny test helper shared by every lesson. You don't need to understand it yet;
// by the templates lesson you will, and you could write it yourself.
//
// Tests are grouped into steps with lab::section("Step 2: ..."). The report shows one line per step,
// and the failure details only for the first unfinished step, so you can focus on one thing at a time.
#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace lab {

struct Section {
    std::string name;
    int pass = 0;
    int fail = 0;
    std::vector<std::string> failures;
};

inline std::vector<Section> g_sections;

inline void section(std::string_view name) { g_sections.push_back({std::string(name)}); }

inline Section& current() {
    if (g_sections.empty()) section("Tests");
    return g_sections.back();
}

inline void check(bool ok, std::string_view expr, std::string_view file, int line) {
    Section& s = current();
    if (ok) {
        ++s.pass;
        return;
    }
    ++s.fail;
    file = file.substr(file.find_last_of("/\\") + 1);  // just the filename, not the full path
    s.failures.push_back(std::string(file) + "(" + std::to_string(line) + "): " + std::string(expr));
}

// Records a failure with a plain message (for checks that can't be written as an expression yet).
inline void fail(std::string_view message) {
    Section& s = current();
    ++s.fail;
    s.failures.push_back(std::string(message));
}

// Call at the end of main(): prints progress per step and returns the process exit code.
inline int report() {
    std::cout << "\nProgress\n";
    const Section* focus = nullptr;
    for (const Section& s : g_sections) {
        const bool done = s.fail == 0;
        std::string label = s.name;
        if (label.size() < 40) label.resize(40, ' ');
        std::cout << (done ? "  [x] " : "  [ ] ") << label << s.pass << "/" << (s.pass + s.fail);
        if (!done && !focus) {
            focus = &s;
            std::cout << "   <-- you are here";
        }
        std::cout << "\n";
    }
    if (!focus) {
        std::cout << "\nAll steps pass. Say \"check\" in chat for a review.\n";
        return 0;
    }
    std::cout << "\nFailing checks in \"" << focus->name << "\":\n";
    for (const std::string& f : focus->failures) std::cout << "  " << f << "\n";
    return 1;
}

}  // namespace lab

// #__VA_ARGS__ turns the expression's source text into a string, so failures show what was tested.
// (Variadic so expressions containing commas, like T{a, b} == T{a, b}, still work.)
#define CHECK(...) ::lab::check(static_cast<bool>(__VA_ARGS__), #__VA_ARGS__, __FILE__, __LINE__)
