// Lesson 02 tests — you don't edit this file. Run:  .\tools\run.ps1 02
// The guide for this lesson is LESSON.md in this folder.
//
// In this lesson YOU write parts of the headers, so this file has to compile before those parts exist.
// Each step's tests are a template that first asks "does the type have what this step needs yet?"
// (that's the `requires` line). If not, the step shows a message instead of a compile error.
// You'll learn how this works in the templates lesson; for now, just read the messages.
#include <capture/model/circle_status.h>
#include <capture/model/timecode.h>
#include <capture/model/uuid.h>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <set>
#include <string>
#include <utility>

#include "check.h"

using namespace capture::model;

// ---- Step 1-3: CircleStatus ----------------------------------------------------------------------

template <class S = CircleStatus>
constexpr bool has_all_statuses = requires {
    S::NotCaptured;
    S::Pending;
    S::Captured;
    S::CircleA;
    S::CircleB;
    S::Burnt;
};

template <class S = CircleStatus>
void step1_enum() {
    if constexpr (has_all_statuses<S>) {
        // The numbers are part of the spec: later lessons save them to a database.
        CHECK(static_cast<int>(S::NotCaptured) == 0);
        CHECK(static_cast<int>(S::Pending) == 1);
        CHECK(static_cast<int>(S::Captured) == 2);
        CHECK(static_cast<int>(S::CircleA) == 3);
        CHECK(static_cast<int>(S::CircleB) == 4);
        CHECK(static_cast<int>(S::Burnt) == 5);
    } else {
        lab::fail("CircleStatus is missing values: add all six from the spec in circle_status.h");
    }
}

template <class S = CircleStatus>
void step2_to_string() {
    if constexpr (has_all_statuses<S>) {
        CHECK(to_string(S::NotCaptured) == "not_captured");
        CHECK(to_string(S::Pending) == "pending");
        CHECK(to_string(S::Captured) == "captured");
        CHECK(to_string(S::CircleA) == "circle_a");
        CHECK(to_string(S::CircleB) == "circle_b");
        CHECK(to_string(S::Burnt) == "burnt");
        CHECK(to_string(static_cast<S>(99)) == "unknown");  // a number that isn't one of the six
    } else {
        lab::fail("finish Step 1 first");
    }
}

template <class S = CircleStatus>
void step3_parse() {
    if constexpr (has_all_statuses<S>) {
        CHECK(parse_circle_status("not_captured") == S::NotCaptured);
        CHECK(parse_circle_status("pending") == S::Pending);
        CHECK(parse_circle_status("captured") == S::Captured);
        CHECK(parse_circle_status("circle_a") == S::CircleA);
        CHECK(parse_circle_status("circle_b") == S::CircleB);
        CHECK(parse_circle_status("burnt") == S::Burnt);
        CHECK(!parse_circle_status("Burnt"));   // exact match only
        CHECK(!parse_circle_status("circle"));
        CHECK(!parse_circle_status("unknown"));
        CHECK(!parse_circle_status(""));
    } else {
        lab::fail("finish Step 1 first");
    }
}

// ---- Step 4-6: Timecode --------------------------------------------------------------------------

template <class T = Timecode>
constexpr bool timecode_basics = requires(const T t) {
    T(30, 0);
    { t.frame_rate() } -> std::same_as<int>;
    { t.total_frames() } -> std::same_as<int>;
    { t == t } -> std::same_as<bool>;
};

// Same members, but callable only on a non-const Timecode: means the getters aren't marked const.
template <class T = Timecode>
constexpr bool timecode_basics_not_const = requires(T t) {
    t.frame_rate();
    t.total_frames();
};

template <class T = Timecode>
void step4_timecode() {
    if constexpr (timecode_basics<T>) {
        const T start(30, 0);
        CHECK(start.frame_rate() == 30);
        CHECK(start.total_frames() == 0);
        const T later(120, 4500);
        CHECK(later.frame_rate() == 120);
        CHECK(later.total_frames() == 4500);
        CHECK(T(60, 10) == T(60, 10));
        CHECK(T(60, 10) != T(60, 11));
        CHECK(T(60, 10) != T(30, 10));  // same frame count, different rate: not the same time
    } else if constexpr (timecode_basics_not_const<T>) {
        lab::fail("frame_rate() and total_frames() can't be called on a const Timecode: add const after the ()");
    } else {
        lab::fail("Timecode needs: constructor (frame_rate, total_frames), frame_rate(), total_frames(), ==");
    }
}

template <class T = Timecode>
void step5_to_string() {
    if constexpr (timecode_basics<T> && requires(const T t) { { t.to_string() } -> std::same_as<std::string>; }) {
        CHECK(T(30, 0).to_string() == "00:00:00:00");
        CHECK(T(30, 29).to_string() == "00:00:00:29");
        CHECK(T(30, 30).to_string() == "00:00:01:00");
        CHECK(T(30, 30 * 60).to_string() == "00:01:00:00");
        CHECK(T(30, 30 * 3600 + 30 * 60 * 2 + 30 * 3 + 4).to_string() == "01:02:03:04");
        CHECK(T(120, 119).to_string() == "00:00:00:119");      // 120 fps: frames go up to 119
        CHECK(T(60, 60 * 3600 * 25).to_string() == "25:00:00:00");  // hours keep counting past 24
    } else if constexpr (timecode_basics<T>) {
        lab::fail("Timecode has no to_string() yet (it must return std::string and be const)");
    } else {
        lab::fail("finish Step 4 first");
    }
}

template <class T = Timecode>
void step6_operators() {
    if constexpr (timecode_basics<T> && requires(const T t) {
                      { t + 1 } -> std::same_as<T>;
                      { t - t } -> std::same_as<int>;
                  }) {
        const T start(30, 10);
        CHECK(start + 5 == T(30, 15));
        CHECK(start + 0 == start);
        CHECK((start + 1) + 1 == T(30, 12));
        CHECK(start + 30 == T(30, 40));
        CHECK(start.total_frames() == 10);           // + makes a new Timecode; start is unchanged
        CHECK(T(30, 100) - T(30, 40) == 60);
        CHECK(T(30, 40) - T(30, 100) == -60);
        CHECK(T(120, 500) - T(120, 500) == 0);
    } else if constexpr (timecode_basics<T>) {
        lab::fail("Timecode needs operator+ (Timecode + int -> Timecode) and operator- (Timecode - Timecode -> int), both const");
    } else {
        lab::fail("finish Step 4 first");
    }
}

// ---- Step 7-9: Uuid ------------------------------------------------------------------------------

template <class U = Uuid>
constexpr bool uuid_basics = requires(const U u) {
    U();
    U(std::uint64_t{1}, std::uint64_t{2});
    { u.high() } -> std::same_as<std::uint64_t>;
    { u.low() } -> std::same_as<std::uint64_t>;
    { u.is_nil() } -> std::same_as<bool>;
    { u == u } -> std::same_as<bool>;
};

template <class U = Uuid>
void step7_uuid() {
    if constexpr (uuid_basics<U>) {
        const U nil;
        CHECK(nil.is_nil());
        CHECK(nil.high() == 0);
        CHECK(nil.low() == 0);
        const U id(1, 2);
        CHECK(id.high() == 1);
        CHECK(id.low() == 2);
        CHECK(!id.is_nil());
        CHECK(!U(0, 1).is_nil());
        CHECK(!U(1, 0).is_nil());
        CHECK(U(1, 2) == U(1, 2));
        CHECK(U(1, 2) != U(2, 1));
        CHECK(U(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF).high() == 0xFFFFFFFFFFFFFFFF);  // the full 64 bits
    } else {
        lab::fail("Uuid needs: default constructor (nil), constructor (high, low), high(), low(), is_nil(), ==, all const");
    }
}

template <class U = Uuid>
void step8_to_string() {
    if constexpr (uuid_basics<U> && requires(const U u) { { u.to_string() } -> std::same_as<std::string>; }) {
        CHECK(U().to_string() == "00000000-0000-0000-0000-000000000000");
        CHECK(U(0x0123456789abcdef, 0xfedcba9876543210).to_string() == "01234567-89ab-cdef-fedc-ba9876543210");
        CHECK(U(1, 2).to_string() == "00000000-0000-0001-0000-000000000002");
        CHECK(U(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF).to_string() == "ffffffff-ffff-ffff-ffff-ffffffffffff");
        CHECK(U(7, 7).to_string().size() == 36);
    } else if constexpr (uuid_basics<U>) {
        lab::fail("Uuid has no to_string() yet (it must return std::string and be const)");
    } else {
        lab::fail("finish Step 7 first");
    }
}

template <class U = Uuid>
void step9_generate() {
    if constexpr (uuid_basics<U> && requires { { U::generate() } -> std::same_as<U>; }) {
        const U a = U::generate();
        const U b = U::generate();
        CHECK(!a.is_nil());
        CHECK(a != b);

        constexpr std::size_t kCount = 1000;
        std::set<std::pair<std::uint64_t, std::uint64_t>> seen;
        for (std::size_t i = 0; i < kCount; ++i) {
            const U id = U::generate();
            seen.insert({id.high(), id.low()});
        }
        CHECK(seen.size() == kCount);  // 1000 ids, no repeats

        bool any_high_bit = false;     // random 64-bit numbers use the top half too
        for (const auto& [high, low] : seen) {
            if (high > 0xFFFFFFFF || low > 0xFFFFFFFF) {
                any_high_bit = true;
            }
        }
        CHECK(any_high_bit);
    } else if constexpr (uuid_basics<U>) {
        lab::fail("Uuid has no static generate() yet (it must return a Uuid)");
    } else {
        lab::fail("finish Step 7 first");
    }
}

int main() {
    lab::section("Step 1: CircleStatus values");
    step1_enum();
    lab::section("Step 2: to_string(CircleStatus)");
    step2_to_string();
    lab::section("Step 3: parse_circle_status");
    step3_parse();
    lab::section("Step 4: Timecode class");
    step4_timecode();
    lab::section("Step 5: Timecode::to_string");
    step5_to_string();
    lab::section("Step 6: Timecode + and -");
    step6_operators();
    lab::section("Step 7: Uuid class");
    step7_uuid();
    lab::section("Step 8: Uuid::to_string");
    step8_to_string();
    lab::section("Step 9: Uuid::generate");
    step9_generate();
    return lab::report();
}
