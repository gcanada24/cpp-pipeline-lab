// Capture++ — a take's circle status.       Lesson 02: read lessons/02-take-details/LESSON.md
//
// On set, the director "circles" the takes worth keeping. Every take has exactly one status.
#pragma once

#include <optional>
#include <string_view>

namespace capture::model {

// STEP 1 — YOU write the values. The spec, in order, with the number each one must have:
//   NotCaptured = 0   take is planned, nothing recorded yet
//   Pending     = 1   recording now, or waiting to be reviewed
//   Captured    = 2   recorded and reviewed, not circled
//   CircleA     = 3   circled: the director's first choice
//   CircleB     = 4   circled: a backup choice
//   Burnt       = 5   recorded but unusable
// The numbers matter: later lessons save statuses to a database as numbers, so they must never change.
enum class CircleStatus {
};

// STEP 2 — The name used when saving a status as text:
//   NotCaptured -> "not_captured"   Pending -> "pending"     Captured -> "captured"
//   CircleA     -> "circle_a"       CircleB -> "circle_b"    Burnt    -> "burnt"
//   any other number stored in a CircleStatus -> "unknown"
std::string_view to_string(CircleStatus status);

// STEP 3 — The reverse: "circle_a" -> CircleStatus::CircleA. Exact match only ("Burnt" -> nothing).
std::optional<CircleStatus> parse_circle_status(std::string_view text);

}  // namespace capture::model
