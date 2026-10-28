// Capture++ — a unique ID for every entity.     Lesson 02: read lessons/02-take-details/LESSON.md
//
// A Uuid is a 128-bit number, stored as two 64-bit halves. Every entity in Capture++ (Lesson 03 on) gets
// one, so the database can tell two takes apart even when their names match.
// Written as text in the standard 8-4-4-4-12 form: "01234567-89ab-cdef-fedc-ba9876543210".
#pragma once

#include <cstdint>  // std::uint64_t
#include <string>

namespace capture::model {

class Uuid {
public:
    // STEP 7 — YOU declare these here (and define them in uuid.cpp):
    //   - a default constructor: makes the "nil" Uuid, both halves 0
    //   - a constructor taking (std::uint64_t high, std::uint64_t low)
    //   - std::uint64_t high()   the first 64 bits                   (const)
    //   - std::uint64_t low()    the last 64 bits                    (const)
    //   - bool is_nil()          true when both halves are 0         (const)
    //   - == : equal when both halves match
    //
    // STEP 8 — std::string to_string()   lower-case hex, 8-4-4-4-12, 36 characters     (const)
    //
    // STEP 9 — static Uuid generate()    a new random Uuid

private:
    // STEP 7 — YOU declare the data members here: the two halves, each starting at 0.
};

}  // namespace capture::model
