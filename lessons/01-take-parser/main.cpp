// Lesson 01 — Take filename parser
// Build + run from the repo root:   .\tools\run.ps1 01
//
// Turn "D:/mocap/BRAWL_DAY2_T003_JDoe.fbx" into a TakeRecord. Rules (same as you'd enforce at a stage):
//   <SHOOT>_T<NNN>_<PERFORMER>.<ext>
//   SHOOT      everything before the last two underscores; may contain '_'; not empty
//   T<NNN>     'T' then digits only; take >= 1; leading zeros fine (T003 -> 3)
//   PERFORMER  not empty
//   ext        .fbx or .c3d, any case (.FBX ok)
//   Folders in front of the filename are ignored ('/' or '\').
// On any violation parse_take returns an empty optional (std::nullopt).

#include <charconv>   // std::from_chars
#include <optional>   // std::optional, std::nullopt
#include <string>
#include <string_view>

#include "check.hpp"

struct TakeRecord {
    std::string shoot;
    int take = 0;
    std::string performer;
    std::string path;    // the full path exactly as given
    double fps = 120.0;

    // TODO(3): make two TakeRecords comparable with ==. One line in C++20.
    //          Then uncomment the "equality" section in main().
};

// TODO(1): helper — true if ext is ".fbx" or ".c3d", ignoring case.
bool is_take_extension(std::string_view ext) {
    (void)ext;  // remove this line once you use ext (it silences an "unused parameter" warning)
    return false;
}

// TODO(2): the parser. Suggested order: strip folders -> split off extension -> split from the right
//          on '_' twice -> validate shoot, take, performer -> build the record.
std::optional<TakeRecord> parse_take(std::string_view path, double fps = 120.0) {
    (void)path;
    (void)fps;
    return std::nullopt;
}

// ---------------------------------------------------------------------------------------------------
// Tests. Don't edit below this line (except uncommenting the equality section for TODO 3).
// ---------------------------------------------------------------------------------------------------
static bool parses_to(std::string_view path, std::string_view shoot, int take, std::string_view performer) {
    auto r = parse_take(path);
    return r && r->shoot == shoot && r->take == take && r->performer == performer && r->path == path &&
           r->fps == 120.0;
}

int main() {
    lab::section("extension");
    CHECK(is_take_extension(".fbx"));
    CHECK(is_take_extension(".FBX"));
    CHECK(is_take_extension(".C3d"));
    CHECK(!is_take_extension(".mov"));
    CHECK(!is_take_extension(".fbx2"));
    CHECK(!is_take_extension(""));

    lab::section("valid names");
    CHECK(parses_to("BRAWL_T003_JDoe.fbx", "BRAWL", 3, "JDoe"));
    CHECK(parses_to("BRAWL_DAY2_T012_ASmith.c3d", "BRAWL_DAY2", 12, "ASmith"));
    CHECK(parses_to("D:/mocap/brawl/BRAWL_T001_JDoe.FBX", "BRAWL", 1, "JDoe"));
    CHECK(parses_to("D:\\mocap\\brawl\\BRAWL_T001_JDoe.fbx", "BRAWL", 1, "JDoe"));
    CHECK(parse_take("BRAWL_T003_JDoe.fbx", 60.0).value_or(TakeRecord{}).fps == 60.0);

    lab::section("rejected names");
    CHECK(!parse_take("BRAWL_003_JDoe.fbx"));    // no T
    CHECK(!parse_take("BRAWL_TABC_JDoe.fbx"));   // not digits
    CHECK(!parse_take("BRAWL_T12a_JDoe.fbx"));   // trailing junk after digits
    CHECK(!parse_take("BRAWL_T_JDoe.fbx"));      // no digits
    CHECK(!parse_take("BRAWL_T000_JDoe.fbx"));   // take 0
    CHECK(!parse_take("BRAWL_T003.fbx"));        // no performer
    CHECK(!parse_take("BRAWL_T003_.fbx"));       // empty performer
    CHECK(!parse_take("_T003_JDoe.fbx"));        // empty shoot
    CHECK(!parse_take("BRAWL_T003_JDoe.mov"));   // bad extension
    CHECK(!parse_take("BRAWL_T003_JDoe"));       // no extension
    CHECK(!parse_take(""));

    // lab::section("equality");                  // TODO(3): uncomment this block
    // auto a = parse_take("BRAWL_T003_JDoe.fbx");
    // auto b = parse_take("BRAWL_T003_JDoe.fbx");
    // CHECK(a && b && *a == *b);
    // CHECK(*a != *parse_take("BRAWL_T004_JDoe.fbx"));

    return lab::report();
}
