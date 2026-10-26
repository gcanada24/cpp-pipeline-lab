// Capture++ — take file naming.            Lesson 01: read lessons/01-take-names/LESSON.md
//
// Every capture file on disk is named   <SHOOT>_T<NNN>_<PERFORMER>.<ext>
//   e.g.  D:/mocap/brawl/BRAWL_DAY2_T003_JDoe.fbx
//
// This header DECLARES what exists (names, types, rules). take_name.cpp DEFINES how each function works.
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

    // STEP 6: add one line here so two TakeNames can be compared with ==
};

// STEP 2 — True if ext is ".fbx" or ".c3d", ignoring upper/lower case.
//   ".fbx" -> true    ".FBX" -> true    ".mov" -> false    "fbx" -> false    "" -> false
bool is_take_extension(std::string_view ext);

// STEP 3 — The filename part of a path: everything after the last '/' or '\'.
//   "D:/mocap/a.fbx" -> "a.fbx"    "a.fbx" -> "a.fbx"    "D:/mocap/" -> ""
std::string_view file_name(std::string_view path);

// STEP 4 — The number in a take token: 'T' followed by digits only, value >= 1.
//   "T003" -> 3    "T1250" -> 1250    "T000" -> nothing    "T12a" -> nothing    "003" -> nothing
std::optional<int> parse_take_number(std::string_view token);

// STEP 5 — Parses a full path or bare filename into a TakeName (uses steps 2-4).
//   Returns std::nullopt if any rule is broken:
//     SHOOT      everything before the last two '_'; not empty
//     T<NNN>     parse_take_number() succeeds
//     PERFORMER  not empty
//     ext        is_take_extension()
std::optional<TakeName> parse_take_name(std::string_view path);

// STEP 7 — The reverse: builds a filename (no folders), take number padded to at least 3 digits.
//   {"BRAWL", 7, "JDoe", ".fbx"}  ->  "BRAWL_T007_JDoe.fbx"
std::string format_take_name(const TakeName& name);

}  // namespace capture::naming
