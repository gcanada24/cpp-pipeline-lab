// Capture++ — take file naming (definitions).   Lesson 01: read lessons/01-take-names/LESSON.md
// The rules for each function are written above its declaration in capture/naming/take_name.h
#include "capture/naming/take_name.h"

#include <cctype>    // std::tolower                (step 2)
#include <array>
#include <string_view>
#include <charconv>  // std::from_chars             (step 4)
#include <iostream>
#include <format>

namespace capture::naming {
namespace {  // private to this file: constants and helper functions
    using namespace std::string_view_literals;
    constexpr std::array kValidExt{ ".fbx"sv, ".c3d"sv };

    std::string to_lower_copy(std::string_view text) {
        std::string result;
        result.reserve(text.size());

        for (char c : text) {
            unsigned char as_unsigned = static_cast<unsigned char>(c);
            int lowered = std::tolower(as_unsigned);
            result.push_back(static_cast<char>(lowered));
        }
        return result;
    }

// STEP 2: your take-extension constant list and to_lower_copy helper go here

}  // namespace

// STEP 2 ------------------------------------------------------------------------------------------
bool is_take_extension(std::string_view ext) {
    if (ext != "") {
        std::string lowered_text = to_lower_copy(ext
        );
        for (std::string_view extension : kValidExt) {
            if (lowered_text == extension) {
                return true;
            }
        }

    }
    return false;
}

// STEP 3 ------------------------------------------------------------------------------------------
std::string_view file_name(std::string_view path) {
    std::size_t last_slash = path.find_last_of("/\\");
    if (last_slash == std::string_view::npos) {
        return path;
    }
    return path.substr(last_slash + 1);

}

// STEP 4 ------------------------------------------------------------------------------------------
std::optional<int> parse_take_number(std::string_view token) {
    if (token.starts_with("T")) {
        int value = 0;
        const char* begin = token.data()+1;
        const char* end = token.data() + token.size();
        std::from_chars_result take_num_result = std::from_chars(begin, end, value);
        if (take_num_result.ptr == end && take_num_result.ec == std::errc{} && value > 0) {
            return value;
        }

    }
    return std::nullopt;
}

// STEP 5 ------------------------------------------------------------------------------------------
std::optional<TakeName> parse_take_name(std::string_view path) {
    std::string_view file_name_sv = file_name(path);
    auto ext_pos = file_name_sv.find(".");
    if (ext_pos == std::string_view::npos) {
        return std::nullopt;
    }
    std::string_view file_ext_sv = file_name_sv.substr(ext_pos, file_name_sv.size());
    if (!is_take_extension(file_ext_sv)) {
        return std::nullopt;
    }
    auto last = file_name_sv.rfind('_');
    if (last == std::string_view::npos) {
        return std::nullopt;
    }
    std::string_view performer_sv = file_name_sv.substr(last + 1, ext_pos - last - 1);
    if (performer_sv.empty()) {
        return std::nullopt;
    }
    auto previous = file_name_sv.rfind('_', last - 1);
    if (previous == std::string_view::npos) {
        return std::nullopt;
    }
    std::string_view take_number_sv = file_name_sv.substr(previous + 1, last - previous - 1);
    std::optional<int> take_number = parse_take_number(take_number_sv);
    if (!take_number) {
        return std::nullopt;
    }
    std::string_view take_name_sv = file_name_sv.substr(0, previous);
    if (take_name_sv.empty()) {
        return std::nullopt;
    }
    TakeName takeName;
    takeName.path = std::string(path);
    takeName.extension = std::string(file_ext_sv);
    takeName.performer = std::string(performer_sv);
    takeName.shoot = std::string(take_name_sv);
    takeName.take = *take_number;
    return takeName;
}

// STEP 7 ------------------------------------------------------------------------------------------
std::string format_take_name(const TakeName& name) {
    std::string formatted_take = std::format("{}_T{:03}_{}{}", name.shoot, name.take, name.performer, name.extension);
    return formatted_take;
}

}  // namespace capture::naming
