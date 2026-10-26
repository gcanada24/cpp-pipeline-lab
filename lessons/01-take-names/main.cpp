// Lesson 01 tests — you don't edit this file. Run:  .\tools\run.ps1 01
// The guide for this lesson is LESSON.md in this folder.
#include <capture/naming/take_name.h>

#include <concepts>

#include "check.h"

using namespace capture::naming;

static bool parses_to(std::string_view path, std::string_view shoot, int take, std::string_view performer,
                      std::string_view extension) {
    auto r = parse_take_name(path);
    return r && r->shoot == shoot && r->take == take && r->performer == performer && r->extension == extension &&
           r->path == path;
}

// Step 6 is written as a template so this file still compiles before TakeName has operator==.
template <class T>
static void step6_equality() {
    if constexpr (std::equality_comparable<T>) {
        std::optional<T> a = parse_take_name("BRAWL_T003_JDoe.fbx");
        std::optional<T> b = parse_take_name("BRAWL_T003_JDoe.fbx");
        std::optional<T> c = parse_take_name("BRAWL_T004_JDoe.fbx");
        CHECK(T{"BRAWL", 3, "JDoe", ".fbx", "x"} == T{"BRAWL", 3, "JDoe", ".fbx", "x"});
        CHECK(T{"BRAWL", 3, "JDoe", ".fbx", "x"} != T{"BRAWL", 4, "JDoe", ".fbx", "x"});
        CHECK(a && b && *a == *b);
        CHECK(a && c && *a != *c);
    } else {
        lab::fail("TakeName can't be compared with == yet: add the operator== line inside the struct");
    }
}

int main() {
    lab::section("Step 2: is_take_extension");
    CHECK(is_take_extension(".fbx"));
    CHECK(is_take_extension(".FBX"));
    CHECK(is_take_extension(".C3d"));
    CHECK(!is_take_extension(".mov"));
    CHECK(!is_take_extension(".fbx2"));
    CHECK(!is_take_extension("fbx"));
    CHECK(!is_take_extension(""));

    lab::section("Step 3: file_name");
    CHECK(file_name("D:/mocap/brawl/BRAWL_T003_JDoe.fbx") == "BRAWL_T003_JDoe.fbx");
    CHECK(file_name("D:\\mocap\\brawl\\BRAWL_T003_JDoe.fbx") == "BRAWL_T003_JDoe.fbx");
    CHECK(file_name("D:/mixed\\slashes/a.fbx") == "a.fbx");
    CHECK(file_name("a.fbx") == "a.fbx");
    CHECK(file_name("D:/mocap/") == "");
    CHECK(file_name("") == "");

    lab::section("Step 4: parse_take_number");
    CHECK(parse_take_number("T003") == 3);
    CHECK(parse_take_number("T1") == 1);
    CHECK(parse_take_number("T1250") == 1250);
    CHECK(!parse_take_number("T000"));          // take must be >= 1
    CHECK(!parse_take_number("T"));             // no digits
    CHECK(!parse_take_number("TABC"));          // not digits
    CHECK(!parse_take_number("T12a"));          // junk after the digits
    CHECK(!parse_take_number("003"));           // no T
    CHECK(!parse_take_number("t003"));          // lower-case t is not allowed
    CHECK(!parse_take_number("T99999999999"));  // too big for an int
    CHECK(!parse_take_number(""));

    lab::section("Step 5: parse_take_name");
    CHECK(parses_to("BRAWL_T003_JDoe.fbx", "BRAWL", 3, "JDoe", ".fbx"));
    CHECK(parses_to("BRAWL_DAY2_T012_ASmith.c3d", "BRAWL_DAY2", 12, "ASmith", ".c3d"));
    CHECK(parses_to("D:/mocap/brawl/BRAWL_T001_JDoe.FBX", "BRAWL", 1, "JDoe", ".FBX"));
    CHECK(parses_to("D:\\mocap\\brawl\\BRAWL_T001_JDoe.fbx", "BRAWL", 1, "JDoe", ".fbx"));
    CHECK(!parse_take_name("BRAWL_003_JDoe.fbx"));             // bad take token
    CHECK(!parse_take_name("BRAWL_T003.fbx"));                 // only one '_': no performer
    CHECK(!parse_take_name("BRAWL_T003_.fbx"));                // empty performer
    CHECK(!parse_take_name("_T003_JDoe.fbx"));                 // empty shoot
    CHECK(!parse_take_name("BRAWL_T003_JDoe.mov"));            // bad extension
    CHECK(!parse_take_name("BRAWL_T003_JDoe"));                // no extension
    CHECK(!parse_take_name("D:/mocap/BRAWL_T003_JDoe.fbx/"));  // a folder, not a file
    CHECK(!parse_take_name(""));

    lab::section("Step 6: operator==");
    step6_equality<TakeName>();

    lab::section("Step 7: format_take_name");
    CHECK(format_take_name({"BRAWL", 7, "JDoe", ".fbx", ""}) == "BRAWL_T007_JDoe.fbx");
    CHECK(format_take_name({"STUNT", 1250, "RLee", ".c3d", ""}) == "STUNT_T1250_RLee.c3d");
    {
        auto r = parse_take_name("D:/mocap/BRAWL_DAY2_T012_ASmith.c3d");  // round trip
        CHECK(r && format_take_name(*r) == "BRAWL_DAY2_T012_ASmith.c3d");
    }

    return lab::report();
}
