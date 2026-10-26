// Capture++ — take file naming.
//
// Every capture file on disk is named   <SHOOT>_T<NNN>_<PERFORMER>.<ext>
//   e.g.  D:/mocap/brawl/BRAWL_DAY2_T003_JDoe.fbx
// This header is the *declaration* (what exists); take_name.cpp is the *definition* (how it works).
#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace capture::naming {

// The pieces of a take filename. Later lessons turn one of these into a Take entity.
struct TakeName {
    std::string shoot;      // "BRAWL_DAY2"  (may contain '_', never empty)
    int take = 0;           // 3             (>= 1)
    std::string performer;  // "JDoe"        (never empty)
    std::string extension;  // ".fbx"        (as written on disk, dot included)
    std::string path;       // the full path exactly as given

    // TODO(3): make two TakeNames comparable with ==   (one line in C++20)
};

// True if ext is ".fbx" or ".c3d", ignoring case (".FBX", ".C3d" are fine).
bool is_take_extension(std::string_view ext);

// Parses a path or bare filename. Folders in front ('/' or '\') are ignored.
// Returns std::nullopt if any rule is broken:
//   SHOOT      everything before the last two '_'; not empty
//   T<NNN>     'T' then digits only, nothing after them; value >= 1; leading zeros fine
//   PERFORMER  not empty
//   ext        is_take_extension()
std::optional<TakeName> parse_take_name(std::string_view path);

// The reverse: builds the filename (no folders) for a take, take number padded to at least 3 digits.
//   {shoot "BRAWL", take 7, performer "JDoe", extension ".fbx"}  ->  "BRAWL_T007_JDoe.fbx"
std::string format_take_name(const TakeName& name);

}  // namespace capture::naming
