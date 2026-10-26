// Capture++ — take file naming (definitions). Declarations and rules: capture/naming/take_name.hpp
#include "capture/naming/take_name.hpp"

#include <charconv>  // std::from_chars

namespace capture::naming {

// TODO(1)
bool is_take_extension(std::string_view ext) {
    (void)ext;  // delete once you use ext (silences the "unused parameter" warning)
    return false;
}

// TODO(2)  Suggested order: strip folders -> split off extension -> find the last two '_'
//          -> validate shoot, take, performer -> build the TakeName.
std::optional<TakeName> parse_take_name(std::string_view path) {
    (void)path;
    return std::nullopt;
}

// TODO(4)
std::string format_take_name(const TakeName& name) {
    (void)name;
    return {};
}

}  // namespace capture::naming
