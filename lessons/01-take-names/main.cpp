// Lesson 01 tests — take file naming.
// You edit:  core/include/capture/naming/take_name.h   and   core/src/naming/take_name.cpp
// Run:       .\tools\run.ps1 01
// Don't edit this file, except uncommenting the TODO(3) block.
#include <capture/naming/take_name.h>

#include "check.h"

using namespace capture::naming;

static bool parses_to(std::string_view path, std::string_view shoot, int take, std::string_view performer,
                      std::string_view extension) {
    auto r = parse_take_name(path);
    return r && r->shoot == shoot && r->take == take && r->performer == performer && r->extension == extension &&
           r->path == path;
}

int main() {
    lab::section("TODO(1) is_take_extension");
    CHECK(is_take_extension(".fbx"));
    CHECK(is_take_extension(".FBX"));
    CHECK(is_take_extension(".C3d"));
    CHECK(!is_take_extension(".mov"));
    CHECK(!is_take_extension(".fbx2"));
    CHECK(!is_take_extension("fbx"));
    CHECK(!is_take_extension(""));

    lab::section("TODO(2) parse_take_name: valid");
    CHECK(parses_to("BRAWL_T003_JDoe.fbx", "BRAWL", 3, "JDoe", ".fbx"));
    CHECK(parses_to("BRAWL_DAY2_T012_ASmith.c3d", "BRAWL_DAY2", 12, "ASmith", ".c3d"));
    CHECK(parses_to("D:/mocap/brawl/BRAWL_T001_JDoe.FBX", "BRAWL", 1, "JDoe", ".FBX"));
    CHECK(parses_to("D:\\mocap\\brawl\\BRAWL_T001_JDoe.fbx", "BRAWL", 1, "JDoe", ".fbx"));
    CHECK(parses_to("STUNT_T1250_RLee.fbx", "STUNT", 1250, "RLee", ".fbx"));

    lab::section("TODO(2) parse_take_name: rejected");
    CHECK(!parse_take_name("BRAWL_003_JDoe.fbx"));                   // no T
    CHECK(!parse_take_name("BRAWL_TABC_JDoe.fbx"));                  // not digits
    CHECK(!parse_take_name("BRAWL_T12a_JDoe.fbx"));                  // junk after the digits
    CHECK(!parse_take_name("BRAWL_T_JDoe.fbx"));                     // no digits
    CHECK(!parse_take_name("BRAWL_T000_JDoe.fbx"));                  // take 0
    CHECK(!parse_take_name("BRAWL_T99999999999_JDoe.fbx"));          // doesn't fit in an int
    CHECK(!parse_take_name("BRAWL_T003.fbx"));                       // no performer
    CHECK(!parse_take_name("BRAWL_T003_.fbx"));                      // empty performer
    CHECK(!parse_take_name("_T003_JDoe.fbx"));                       // empty shoot
    CHECK(!parse_take_name("BRAWL_T003_JDoe.mov"));                  // bad extension
    CHECK(!parse_take_name("BRAWL_T003_JDoe"));                      // no extension
    CHECK(!parse_take_name("D:/mocap/BRAWL_T003_JDoe.fbx/"));        // a folder, not a file
    CHECK(!parse_take_name(""));

    lab::section("TODO(4) format_take_name");
    CHECK(format_take_name({"BRAWL", 7, "JDoe", ".fbx", ""}) == "BRAWL_T007_JDoe.fbx");
    CHECK(format_take_name({"STUNT", 1250, "RLee", ".c3d", ""}) == "STUNT_T1250_RLee.c3d");
    {
        // Round trip: format(parse(x)) gives back the filename part of x.
        auto r = parse_take_name("D:/mocap/BRAWL_DAY2_T012_ASmith.c3d");
        CHECK(r && format_take_name(*r) == "BRAWL_DAY2_T012_ASmith.c3d");
    }

    // lab::section("TODO(3) operator==");        // uncomment this block after TODO(3)
    // auto a = parse_take_name("BRAWL_T003_JDoe.fbx");
    // auto b = parse_take_name("BRAWL_T003_JDoe.fbx");
    // auto c = parse_take_name("BRAWL_T004_JDoe.fbx");
    // CHECK(a && b && c);
    // CHECK(*a == *b);
    // CHECK(*a != *c);

    return lab::report();
}
