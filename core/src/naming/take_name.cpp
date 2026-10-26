// Capture++ — take file naming (definitions).   Lesson 01: read lessons/01-take-names/LESSON.md
// The rules for each function are written above its declaration in capture/naming/take_name.h
#include "capture/naming/take_name.h"

#include <cctype>    // std::tolower                (step 2)
#include <charconv>  // std::from_chars             (step 4)

namespace capture::naming {
namespace {  // private to this file: constants and helper functions

// STEP 2: your take-extension constant list and to_lower_copy helper go here

}  // namespace

// STEP 2 ------------------------------------------------------------------------------------------
bool is_take_extension(std::string_view ext) {
    (void)ext;  // delete this line once you use ext (it silences the "unused parameter" warning)
    return false;
}

// STEP 3 ------------------------------------------------------------------------------------------
std::string_view file_name(std::string_view path) {
    (void)path;
    return {};
}

// STEP 4 ------------------------------------------------------------------------------------------
std::optional<int> parse_take_number(std::string_view token) {
    (void)token;
    return std::nullopt;
}

// STEP 5 ------------------------------------------------------------------------------------------
std::optional<TakeName> parse_take_name(std::string_view path) {
    (void)path;
    return std::nullopt;
}

// STEP 7 ------------------------------------------------------------------------------------------
std::string format_take_name(const TakeName& name) {
    (void)name;
    return {};
}

}  // namespace capture::naming
